# Code Review — Servidor WebSocket em C

> Projeto revisado: `websocket.c`, `Tcp.c`, `Queue.c`, `Queue.h`, `live_serve.h`, `capturar_signal.c`, `websocket_teste.c`
>
> **Observação**: `hash.h`/`hash.c` não foram enviados (só são referenciados via `#include "hash.h"`). Recomendo revisar também esses arquivos quando disponíveis, especialmente se a ideia é fazer a hash guardar `websocket_id` + `connection_fd` juntos (ver seção final).

---

## 🔴 Críticos (crash / corrupção de memória)

### 1. Use-after-free em `freeQueuen` (Queue.c)
```c
struct Queuedata *temp = p->first;
while (temp != NULL) {
    free(temp->strig);
    free(temp);
    temp = temp->prox;   // ⚠️ lê memória já liberada
}
```
Acessa `temp->prox` **depois** de já ter dado `free(temp)`. Isso é UB clássico: na prática pode até "funcionar" às vezes (glibc não zera memória liberada imediatamente), mas é uma bomba-relógio — pode causar crash, loop infinito ou parar de liberar o resto da lista, dependendo do que sobrar no heap.

**Fix:**
```c
struct Queuedata *temp = p->first;
while (temp != NULL) {
    struct Queuedata *prox = temp->prox;  // salva antes
    free(temp->strig);
    free(temp);
    temp = prox;
}
```

### 2. Buffer overflow em `enviar_mensagem_websocket` (websocket.c)
```c
unsigned char frame[1024];
...
} else if (len >= 126 && len <= 65535) {
    frame[1] = 126;
    frame[2] = (len >> 8) & 0xFF;
    frame[3] = len & 0xFF;
    frame_len = 4;
}
...
memcpy(frame + frame_len, mensagem, len);   // ⚠️ len pode ser até 65535, frame só tem 1024
```
Se `mensagem` tiver mais de ~1020 bytes, esse `memcpy` estoura o array `frame[1024]` na stack — **stack buffer overflow** real, não teórico. Isso pode corromper o retorno da função (crash) ou, em cenário pior, ser explorável.

**Fix:** validar `len < sizeof(frame) - frame_len` antes do memcpy (ou alocar dinamicamente `malloc(frame_len + len)`).

### 3. "Zumbi" de conexão fechada em loop (2ª ocorrência, no handshake)
Além do caso já identificado em `pronto_msg < 0` (faltando `websocket_ative = 0` + `break`), tem uma variante no **início** da função `websocket_read`:
```c
size_t bytes_lidos = read(websocket->connection_fd, websocket->buffer, sizeof(websocket->buffer) - 1);
if(bytes_lidos <= 0)
{
    printf("erro na conexao\n");
    websocket->websocket_ative = 0;
    close(websocket->connection_fd);
    continue;                         // sai do while externo (ok, active=0)
}
```
Fecha o fd e sai do `while` externo corretamente — só que ao cair fora do loop, o código chega no bloco de limpeza final da função:
```c
pthread_mutex_lock(&block);
deleteHash(&clientes_threads, websocket->connection_fd);
pthread_mutex_unlock(&block);
close(websocket->connection_fd);   // ⚠️ close() de novo no MESMO fd
free(websocket);
```
Isso é um **double close()** no mesmo número de fd. Se nesse meio-tempo outra thread já abriu uma conexão nova que reaproveitou esse número (bem provável sob carga), fecha-se a conexão de outro cliente — o mesmo efeito colateral do bug de reconexão já identificado, só que disparado no handshake em vez de no meio da sessão.

**Fix:** depois de fechar no bloco de erro, pular direto pro `free(websocket); return NULL;` em vez de deixar cair no bloco de limpeza genérico (ou marcar `connection_fd = -1` após o close, e sempre checar `if(fd >= 0) close(fd)`).

---

## 🟠 Concorrência

### 4. Duas mutex diferentes protegendo a mesma hash table
- `websocket_serve` usa `clientes_mutex` para chamar `insertHash`.
- `websocket_read` usa `block` para chamar `deleteHash`.

São **mutexes diferentes protegendo a mesma estrutura** (`clientes_threads`). Isso não garante exclusão mútua nenhuma entre inserir e remover — pode causar corrupção da hash (ex.: thread nova inserindo enquanto thread antiga remove, mexendo nos mesmos ponteiros/buckets ao mesmo tempo).

**Fix:** usar **um único mutex** para todo acesso a `clientes_threads` (pode ser o próprio `block`, já que ele já é global e usado no resto do sistema).

### 5. `pthread_mutex_t block;` nunca inicializado (websocket_teste.c)
```c
pthread_mutex_t block;   // sem PTHREAD_MUTEX_INITIALIZER nem pthread_mutex_init
```
Comportamento indefinido por padrão POSIX. Só "funciona" no Linux/glibc por coincidência de implementação (zero-init compatível).

**Fix:**
```c
pthread_mutex_t block = PTHREAD_MUTEX_INITIALIZER;
```

### 6. `captura_signal` não é async-signal-safe
```c
void captura_signal(int sinal){
    printf(...);                         // não é async-signal-safe
    pthread_mutex_lock(&block);          // idem — pode dar deadlock
    rodando = 0;
    pthread_mutex_unlock(&block);
}
```
Um handler de sinal só pode chamar um conjunto restrito de funções "async-signal-safe" (a POSIX define a lista — `printf` e `pthread_mutex_lock` **não estão nela**). Se o SIGINT chegar exatamente enquanto a própria thread já segura o mutex `block`, há **deadlock instantâneo** (mutex não é recursivo). E `printf` chamado dentro de um sinal que interrompeu outro `printf` já em andamento pode corromper o buffer do stdio.

**Fix:** o padrão é só setar uma flag `volatile sig_atomic_t` sem nenhuma outra chamada:
```c
volatile sig_atomic_t rodando_sinal = 0;

void captura_signal(int sinal){
    rodando_sinal = 1;
}
```
E no loop principal, checar essa flag e fazer o `printf`/lock ali, fora do handler.

### 7. Qualificador `volatile` inconsistente em `flag`
```c
// websocket.c
extern int flag;
// websocket_teste.c
volatile int flag = -1;
```
Declaração e definição com qualificadores diferentes é UB em C. Deixar os dois `volatile int flag;`.

---

## 🟡 Vazamentos de memória / recursos

### 8. Threads de cliente nunca dão `detach`
Sem `pthread_detach(websocket_id)` logo após o `pthread_create` bem-sucedido em `websocket_serve`, cada thread de cliente que termina fica em estado "zumbi" de recursos (pilha + descritor de thread) até alguém dar join — que nunca acontece nesse fluxo. Isso é vazamento constante, ativado a cada reconexão.

*(Nota: `pthread_join` no `websocket_teste.c` é sobre a thread do `websocket_serve`, não sobre as threads de cliente — variáveis com mesmo nome (`websocket_id`) mas em escopos diferentes. Esse join pode continuar existindo normalmente.)*

### 9. `accept()` grava no struct errado
```c
cliente->client_size = sizeof(cliente->client);
cliente->connection_fd = accept(websocket->socket_fd,
                                 (struct sockaddr*)&websocket->client,   // ⚠️ deveria ser cliente->client
                                 &websocket->client_size);               // ⚠️ deveria ser cliente->client_size
```
Seta-se `cliente->client_size`, mas o `accept()` na verdade escreve o endereço do peer em `websocket->client` (a struct "template" compartilhada), não em `cliente->client`. Como só existe uma thread chamando `accept()` (a própria `websocket_serve`), não é uma race, mas é logicamente errado: cada `cliente` individual nunca guarda o endereço real de quem conectou nele — sempre fica com o que foi copiado do `websocket` template (que pode estar zerado/desatualizado). Se em algum momento for usar `cliente->client` pra logar IP do cliente, vai pegar lixo.

**Fix:**
```c
cliente->client_size = sizeof(cliente->client);
cliente->connection_fd = accept(websocket->socket_fd,
                                 (struct sockaddr*)&cliente->client,
                                 &cliente->client_size);
```

---

## 🔵 Bugs de protocolo WebSocket

### 10. `opcodesData` não trata payload estendido (126/127)
```c
playload_len = websocket->buffer[1] & 0x7F;
```
Os valores **126** e **127** nesse campo não são tamanhos literais — são "flags" dizendo "o tamanho real vem nos próximos 2 ou 8 bytes". O código trata qualquer valor de 0–127 como tamanho literal e sempre assume que a máscara começa no offset 2. Resultado: qualquer mensagem cujo comprimento (por acaso) caia nessa faixa "especial", ou qualquer mensagem realmente grande (que o navegador *precisa* enviar com length estendido), é **decodificada errado** — máscara XOR aplicada com offset errado, texto vira lixo.

**Fix mínimo:** decodificar os 3 casos:
```c
int len_field = websocket->buffer[1] & 0x7F;
int mask_index;
uint64_t playload_len;

if (len_field <= 125) {
    playload_len = len_field;
    mask_index = 2;
} else if (len_field == 126) {
    playload_len = (websocket->buffer[2] << 8) | websocket->buffer[3];
    mask_index = 4;
} else { // 127
    mask_index = 10;
    playload_len = /* ler 8 bytes big-endian */;
}
```
E validar `playload_len` contra o tamanho real do `websocket->buffer` antes de copiar, pra não estourar `mensagem[1024]` se algum dia isso vier a aceitar tamanhos maiores.

### 11. Variáveis redeclaradas no mesmo escopo em `opcodesData` — provável erro de compilação
```c
void opcodesData(Transport *websocket, int opcode){
    unsigned char mask[4];              // declarada aqui
    switch (opcode) {
        case 0x01:
            ...
            unsigned char mask[4];      // ⚠️ redeclaração, mesmo escopo (switch sem chaves por case)
            ...
        case 0x09:
            ...
            int data_index = mask_index + 4;  // ⚠️ "data_index" já existe fora do switch
```
Como não há `{ }` isolando cada `case`, tudo está no mesmo bloco léxico do `switch` — isso deveria dar erro de compilação (`redefinition of 'mask'` / `'data_index'`).

**Fix:** isolar cada `case` com chaves:
```c
switch (opcode) {
    case 0x01: {
        int playload_len = ...;
        unsigned char mask[4];
        ...
        break;
    }
    case 0x09: {
        ...
        break;
    }
}
```

---

## ⚪ Qualidade / robustez (impacto menor, mas vale corrigir)

| # | Local | Problema |
|---|---|---|
| 12 | `Tcp.c` | A função sempre faz `bind()` + `listen()` independente do parâmetro `tipo` — passar `"client"` (como em `websocket_teste.c`) ainda cria um socket servidor. Nome da API engana; considere renomear ou realmente diferenciar o comportamento. |
| 13 | `Tcp.c` | `listen()` não verifica valor de retorno. |
| 14 | `Queue.c` – `Enqueue` | Depois do `strcpy`, faz `strcmp(string, newstring)` pra "validar a cópia" — isso é redundante (nunca vai divergir num `strcpy` bem-sucedido) e mascara o real propósito; pode remover. |
| 15 | `Queue.c` – `Dequeue` | Quando o último elemento é removido, `p->tail` fica "pendurado" (dangling) apontando pro nó liberado. É mitigado porque o próximo `Enqueue` reseta tudo quando `size==0`, mas é um code smell que pode morder se `tail` for usado em outro lugar no futuro. |
| 16 | `Queue.c` – `peekQueue` | Não checa fila vazia (`p->first == NULL`) antes de desreferenciar — chamada numa fila vazia causa segfault. |
| 17 | `websocket.c` – `websocket_write` | Faz `select()` no `STDERR_FILENO` mas lê do `stdin` com `fgets`. Deveria ser `STDIN_FILENO`. |
| 18 | Geral | Nenhum `pthread_mutex_destroy()` para `block`/`clientes_mutex` no encerramento (impacto baixo, processo termina de qualquer forma, mas é boa prática). |
| 19 | `websocket_teste.c` | Loop principal faz polling com `usleep(200)` (200 microssegundos!) — CPU sempre ativa. Um `pthread_cond_wait` seria mais eficiente que mutex + poll agressivo. |

---

## 📌 Sobre guardar `websocket_id` + `connection_fd` juntos na hash

Pelo uso que já existe (`insertHash(&clientes_threads, cliente->connection_fd, websocket_id)` e `deleteHash(&clientes_threads, connection_fd)`), a hash parece já ser `fd -> pthread_t`. Se a ideia é deixar isso mais explícito/rico (por exemplo, pra no futuro conseguir dar `pthread_cancel`/diagnóstico por conexão), uma estrutura de valor combinada ajuda:

```c
typedef struct {
    int connection_fd;
    pthread_t websocket_id;
} ClienteInfo;
```
A hash guardaria `fd -> ClienteInfo` (chave ainda é o fd, mas o valor carrega os dois campos, então qualquer lookup já devolve ambos de uma vez, sem precisar de duas estruturas separadas). Como `hash.h`/`hash.c` não foram enviados, vale revisar se a tabela atual já faz algo assim ou se guarda só o `pthread_t` cru como valor.

---

## Resumo de prioridade

1. **#1, #2, #3** — podem crashar ou corromper memória, corrigir primeiro.
2. **#4, #5, #6, #7** — condições de corrida e UB, causam bugs intermitentes difíceis de reproduzir.
3. **#8, #9** — vazamento de recursos e dado errado guardado (mas não derruba o processo).
4. **#10, #11** — limitam o protocolo (mensagens grandes quebram).
5. Resto — polimento.
