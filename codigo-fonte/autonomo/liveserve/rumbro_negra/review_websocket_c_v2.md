# Code Review v2 — Servidor WebSocket em C

> Nova rodada, agora com `hash.c` e `hash.h` incluídos. `Tcp.c` não foi reenviado desta vez — os apontamentos #12/#13 da v1 (sobre `Tcp()` sempre fazer bind+listen independente do `tipo`, e `listen()` sem checar retorno) seguem valendo, assumindo que o arquivo não mudou.

---

## ✅ Corrigido desde a última revisão

Bom progresso — essa lista é grande:

- **Use-after-free em `freeQueuen`** → corrigido (salva `prox` antes do `free`).
- **`peekQueue` sem checar fila vazia** → corrigido.
- **Mutex `block` não inicializado** → corrigido (`PTHREAD_MUTEX_INITIALIZER`).
- **`flag` com `volatile` inconsistente** → corrigido (`volatile` nos dois lados).
- **Redeclaração de variáveis no `switch` de `opcodesData`** → corrigido (cada `case` tem seu próprio bloco `{ }`).
- **`accept()` gravando no struct errado** (`websocket->client` em vez de `cliente->client`) → corrigido.
- **`websocket_write` fazendo `select()` no fd errado** → corrigido (`STDIN_FILENO`).
- **Duas mutex diferentes protegendo a mesma hash** → corrigido: agora tanto `websocket_serve` (insert) quanto `websocket_read` (delete) usam `block`.
- **"Zumbi" de conexão fechada em loop infinito / double-close no handshake** (os bugs #1 e #3 da v1, que causavam o sintoma do `ws.close()` + reconexão fantasma) → **corrigidos**. A função `websocket_read` agora tem um único ponto de saída/limpeza no final, alcançado sempre via `break` explícito, então não há mais o `close()` duplicado no mesmo fd nem o loop que ficava vivo com o socket já fechado. Ótimo, esse era o bug mais grave da v1.

---

## 🔴 Crítico novo: crash no encerramento (`freehash` + threads detached)

Esse é o achado mais importante desta rodada, e nasce de uma interação entre duas partes que isoladamente pareciam corretas: o `pthread_detach()` que adicionamos em `websocket_serve`, e o `freehash()` de `hash.c`.

```c
// websocket_serve — cada cliente novo:
pthread_create(&websocket_id, NULL, websocket_read, (void *)cliente);
...
pthread_detach(websocket_id);   // thread agora é "detached"
```

```c
// hash.c — freehash(), chamado no encerramento do servidor:
void freehash(HASH *table){
    ...
    for(int i = 0; i < TAM_INICIAL; i++){
        if(table->no[i].key >= 0){
            close(table->no[i].key);
            pthread_join(table->no[i].value, NULL);   // ⚠️ join numa thread detached
        }
        ...
    }
    free(table->no);
    table->no = NULL;
}
```

Duas threads não podem ser "joined" depois de "detached" — chamar `pthread_join` numa thread detached é **comportamento indefinido pelo POSIX**. Na prática, no glibc, isso costuma retornar `EINVAL` quase imediatamente, **sem esperar a thread realmente terminar**. Ou seja: `freehash` fecha o fd do cliente à força, "tenta" dar join (mas isso não bloqueia de verdade), e segue em frente liberando a tabela:

```c
free(table->no);
table->no = NULL;
```

Só que a thread do cliente que estava rodando pode ainda não ter chegado ao fim do seu próprio ciclo de vida. Ao perceber que o fd foi fechado (ou que `rodando == 0`), ela vai tentar fazer sua limpeza normal:

```c
pthread_mutex_lock(&block);
deleteHash(&clientes_threads, websocket->connection_fd);  // ⚠️ table->no já é NULL aqui
...
```

Só que ela só consegue continuar depois que `websocket_serve` **libera o mutex `block`** (ele está segurando `block` durante toda a chamada de `freehash`). No momento em que a thread do cliente finalmente adquire o mutex, `table->no` já foi liberado e setado para `NULL` — e `deleteHash` não protege contra isso:

```c
int deleteHash(HASH *table, int key){
    if(key < 0 || table == NULL) return 1;   // não checa table->no == NULL
    unsigned long hash = key % TAM_INICIAL;
    if(table->no[hash].key >= 0 ...          // 💥 NULL pointer dereference
```

**Resultado prático:** se você der Ctrl+C (SIGINT) enquanto ainda existe pelo menos um cliente conectado cuja thread não terminou 100% seu ciclo antes de `freehash` rodar, o processo pode **crashar com SIGSEGV** durante o encerramento. É um bug intermitente — some ou aparece dependendo do timing entre a thread do cliente e a rotina de shutdown, o que é exatamente o tipo de bug difícil de reproduzir de propósito mas que vai aparecer em produção.

**Como pensar no fix (sem aplicar agora, já que você está estudando):**
- `freehash` não deveria dar `pthread_join`/`close` diretamente nas conexões — isso já é responsabilidade da própria thread do cliente ao perceber `rodando == 0`.
- O jeito correto de um shutdown limpo é: sinalizar `rodando = 0`, e o thread principal **esperar** (com alguma forma de sincronização — um contador de threads ativas + variável de condição, por exemplo) até que todas as threads de cliente tenham de fato retornado, e só então destruir a hash/tabelas.
- Alternativamente, não dar `pthread_detach` nas threads de cliente e fazer join explícito de cada uma no encerramento — mas aí você precisa guardar todos os `pthread_t` numa lista à parte pra poder dar join em todos no fim (a hash sozinha nãoávisa quando esvaziou).
- Em qualquer um dos dois casos, `deleteHash` também deveria defensivamente checar `table->no == NULL` antes de indexar, como uma segunda camada de proteção.

---

## 🔴 Outros críticos (já apontados na v1, ainda não corrigidos)

### `pthread_create(...) == -1` — checagem de erro que nunca dispara
```c
if((pthread_create(&websocket_id, NULL, websocket_read, (void *)cliente)) == -1){
```
`pthread_create` **nunca** retorna `-1`. Em sucesso retorna `0`; em falha retorna um código de erro positivo (`EAGAIN`, `EINVAL`, etc). Essa condição nunca é verdadeira, então se a criação da thread falhar de verdade, o código não percebe — e pior: `websocket_id` fica com valor indeterminado, que depois é usado em `insertHash(..., websocket_id)` e em `pthread_detach(websocket_id)`. Chamar `pthread_detach` sobre um `pthread_t` que nunca foi de fato inicializado por um `pthread_create` bem-sucedido é UB.

**Fix conceitual:** checar `!= 0` em vez de `== -1`.

### Buffer overflow em `enviar_mensagem_websocket` — ainda presente
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
memcpy(frame + frame_len, mensagem, len);   // len pode passar de 1024
```
Ainda sem correção. Vale notar que **atualmente essa função só é chamada de dentro de `websocket_write`, que por sua vez nunca é disparada via `pthread_create` em lugar nenhum do código** — ou seja, no momento é código morto / não alcançável em runtime. Mas o bug continua lá, esperando o dia em que alguém ligar essa thread.

### Payload estendido (126/127) ainda não tratado em `opcodesData`
```c
playload_len = websocket->buffer[1] & 0x7F;
```
Mesma limitação da v1: valores 126/127 nesse campo são um indicador de "o tamanho real vem nos próximos bytes", não um tamanho literal. Mensagens grandes continuam sendo decodificadas incorretamente. (Nesse ponto específico não há risco de overflow do buffer local, já que `& 0x7F` limita a no máximo 127 — mas o dado decodificado fica errado para qualquer payload que realmente precise de tamanho estendido.)

---

## 🟠 Guardas de inclusão invertidas — `hash.h` e `Queue.h`

Reparei que os dois headers usam o mesmo padrão:

```c
// hash.h
#ifdef HASH_H
#define TAM_INICIAL 1009
...
#endif
```
```c
// Queue.h
#ifdef QUEUEN_H
struct Queuedata { ... };
...
#endif
```

Isso é o **inverso** do include guard tradicional. O padrão correto é:
```c
#ifndef HASH_H
#define HASH_H
... conteúdo ...
#endif
```
No seu caso, quem decide se o conteúdo do header aparece ou não é **quem inclui o arquivo**, definindo a macro *antes* do `#include` (como você faz em `websocket.c`: `#define HASH_H` seguido de `#include "hash.h"`). Isso traz dois problemas:

1. **Se alguém esquecer de definir a macro antes de incluir**, o header não define nada, e os erros de compilação aparecem longe da causa raiz (em outro arquivo, tipo "`HASH` não declarado" ou "campo desconhecido"), tornando o debug bem mais difícil.
2. **Não protege contra inclusão dupla de verdade.** Como o próprio header nunca dá `#define HASH_H` (isso é feito por fora), se dois arquivos diferentes incluírem `hash.h` na mesma unidade de tradução, cada um após definir a macro por conta própria, o `struct node`/`HASH` pode acabar sendo redefinido — erro de compilação "redefinition of struct node".

**Fix conceitual:** trocar para o padrão tradicional `#ifndef X_H / #define X_H / ... / #endif`, com a própria macro sendo definida dentro do header, não por fora.

---

## 🟡 Problemas específicos de `hash.c`

### 1. Comparações `pthread_t < 0` / `>= 0` / `> 0` não fazem sentido
```c
int searchHashValue(HASH *table, pthread_t value){
    if(value < 0) return 1;
```
```c
int deleteHashValue(HASH *table, pthread_t value){
    if(value < 0 || table == NULL) return 1;
```
`pthread_t` é um tipo **opaco** pela especificação POSIX — não há garantia de que seja um tipo com sinal, nem sequer que seja um tipo aritmético (em algumas implementações pode ser uma `struct`). No Linux/glibc é tipicamente `unsigned long`, o que faz `value < 0` ser **sempre falso** (comparação de unsigned com zero) — código morto que o compilador provavelmente sinaliza com warning. O sentinela `-1` usado em `inithash` (`no[i].value = -1`) só "funciona" porque, ao ser um `unsigned long`, vira `ULONG_MAX`, e nenhuma thread real deve coincidir com esse valor — mas isso é uma dependência implícita e não-portável do comportamento específico do glibc/Linux, não algo garantido pela linguagem.

### 2. `insertHashValue` usa uma função hash diferente da usada por `insertHash`/`deleteHash`/`searchHash`
```c
int insertHash(HASH *table, int key, pthread_t value){
    ...
    unsigned long hash = key % TAM_INICIAL;      // indexa pela FD (key)
```
```c
int insertHashValue(HASH *table, int key, pthread_t value){
    ...
    unsigned long hash = value % TAM_INICIAL;    // indexa pelo THREAD ID (value)
```
Essas duas funções escrevem no **mesmo array** `table->no`, só que uma posiciona a entrada pelo bucket calculado a partir do fd, e a outra pelo bucket calculado a partir do thread id. Se algum dia você chamar as duas sobre a mesma `HASH`, elas vão brigar pelo mesmo espaço de buckets com critérios diferentes — uma entrada inserida via `insertHash` pode ser sobrescrita/colidida por uma inserida via `insertHashValue`, e os lookups (`searchHash` vs `searchHashValue`) vão ficar inconsistentes entre si.

Hoje isso não quebra nada porque **`insertHashValue`, `searchHash`, `searchHashValue` e `deleteHashValue` não são chamadas em lugar nenhum** do código atual (só `insertHash`/`deleteHash`/`freehash` são usadas de fato) — são código morto. Mas é uma armadilha esperando a hora de ser usada.

### 3. `inithash` não inicializa `table->debug`
```c
int inithash(HASH *table){
    ...
    table->size = 0;
    table->no = no;
    return 0;
}
```
Os campos `table->debug.colision` e `table->debug.normal` nunca são zerados explicitamente aqui. Hoje isso não causa problema porque `clientes_threads` é uma variável global (`HASH clientes_threads;`), e globais são zero-inicializadas automaticamente em C — mas se um dia você instanciar uma `HASH` na stack (variável local) e chamar `inithash` nela, esses contadores vão começar com lixo.

### 4. Sem redimensionamento dinâmico
`TAM_INICIAL` é fixo em 1009 buckets. Não é um bug (a busca em cadeia continua funcionando mesmo com muitas colisões), mas com um número grande de conexões simultâneas, o desempenho degrada de O(1) pra O(n) por bucket. Não é urgente para um projeto de aprendizado, só um ponto de atenção se for escalar depois.

---

## 🟢 Concorrência residual

### `captura_signal` ainda não é async-signal-safe
```c
void captura_signal(int sinal){
    printf("...");                    // não é async-signal-safe
    pthread_mutex_lock(&block);       // idem, risco de deadlock
    rodando = 0;
    pthread_mutex_unlock(&block);
}
```
Continua igual à v1 — ainda não corrigido. Se o `SIGINT` chegar exatamente enquanto a mesma thread já segura `block` (por exemplo, dentro do loop de `websocket_teste.c` que faz `pthread_mutex_lock(&block)` antes de checar a fila), você tem deadlock instantâneo, já que `block` não é um mutex recursivo.

### Corrida pequena quando `insertHash` falha
```c
if((pthread_create(&websocket_id, NULL, websocket_read, (void *)cliente)) == -1){
    ...
}
if(insertHash(&clientes_threads, cliente->connection_fd, websocket_id)){
    close(cliente->connection_fd);
    cliente->websocket_ative = 0;
}
```
Quando `insertHash` falha (por exemplo `malloc` sem memória — raro, mas possível), a thread `websocket_read` **já foi criada e já está rodando** usando esse mesmo `cliente`. Fechar o fd e setar `websocket_ative = 0` aqui, no thread principal, enquanto a thread do cliente pode estar simultaneamente lendo/usando esses mesmos campos, é uma corrida de dados (leitura/escrita não sincronizada da mesma memória). Na prática o impacto tende a ser pequeno (a thread do cliente provavelmente vai só perceber o erro e encerrar), mas formalmente ainda é comportamento indefinido. Gatilho raro (só acontece se `insertHash` falhar), então prioridade baixa.

---

## ⚪ Qualidade / estilo

| # | Local | Observação |
|---|---|---|
| 1 | `websocket_read` | O `while (websocket->websocket_ative)` mais externo agora só executa **uma única vez** de fato, porque todo caminho de código termina em `break` explícito. Funciona, mas fica confuso de ler como se fosse um laço — dá pra reescrever como um bloco sequencial simples (sem `while`) pra deixar a intenção mais clara. |
| 2 | `websocket_write` / `enviar_mensagem_websocket` | Continuam sem ser usadas em lugar nenhum (nenhum `pthread_create` chama `websocket_write`). Código morto por enquanto — tudo bem para uma fase de aprendizado, só não esqueça que o bug de overflow (#3 dos críticos) está latente aí caso decida ativar. |
| 3 | `hash.c` — `insertHashValue`, `searchHash`, `searchHashValue`, `deleteHashValue` | Não usadas atualmente. Se pretende mantê-las "pra usar depois", vale revisar o item 🟡#2 antes de ativá-las. |
| 4 | `Tcp.c` (não reenviado) | Apontamentos da v1 sobre `bind`+`listen` incondicional e falta de checagem de retorno do `listen()` seguem valendo, supondo que o arquivo não mudou. |

---

## 📌 Sobre a hash guardar `websocket_id` + `connection_fd`

Boa notícia: **isso já está implementado**. Cada `struct node` em `hash.c` guarda exatamente os dois campos que você pediu:
```c
struct node{
    int key;          // o connection_fd
    pthread_t value;  // o websocket_id (thread da conexão)
    struct node *colision;
};
```
E `insertHash(&clientes_threads, cliente->connection_fd, websocket_id)` grava os dois juntos, indexando pelo fd. Então, para o caso de uso atual (dado um fd, encontrar/remover a thread responsável por ele), a estrutura já atende.

Se a sua intenção for também **buscar pelo lado inverso** (dado um `websocket_id`, descobrir o fd) de forma eficiente, olhe o item 🟡#2 acima com atenção: `searchHashValue`/`insertHashValue` existem no código mas usam a mesma tabela com uma função de hash diferente — não é uma segunda tabela de verdade. Pra um índice reverso funcionar sem conflitar com o principal, você precisaria de uma **segunda instância de `HASH`** dedicada (outro array `no`, outra chamada de `inithash`), populada em paralelo pelo `websocket_id` como chave — não reaproveitar a mesma tabela com um hash diferente.

---

## Resumo de prioridade

1. **Crash no shutdown (`freehash` + threads detached + `deleteHash` sem checar `table->no == NULL`)** — o achado mais importante desta rodada, intermitente e ligado à ordem de encerramento.
2. **`pthread_create(...) == -1`** — checagem de erro que nunca funciona, pode gerar `pthread_t` inválido em uso.
3. **Guardas de inclusão invertidas** (`hash.h`, `Queue.h`) — não quebram nada hoje, mas vão gerar erros de compilação confusos assim que o projeto crescer.
4. **Buffer overflow em `enviar_mensagem_websocket`** e **payload estendido não tratado** — latentes, sem trigger hoje (código morto / mensagens curtas), mas continuam no código.
5. **Tipos/hash inconsistentes em `hash.c`** (comparações de `pthread_t` com `<0`, função hash diferente entre `insertHash`/`insertHashValue`) — código morto por enquanto, cuidado se for reativar.
6. `captura_signal` não signal-safe — mesmo risco da v1, ainda de pé.
7. Resto — polimento e clareza.
