# Algoritmo de Remoção em Árvore Rubro-Negra (Red-Black Tree Deletion)

A remoção em uma Árvore Rubro-Negra consiste em duas fases principais:
1. **Remoção Padrão de BST**: Encontrar o nó, substituí-lo pelo seu sucessor/antecessor se tiver dois filhos, e removê-lo do mapa.
2. **Correção do Balanceamento (`fixUp`)**: Se o nó removido (ou o nó que o substituiu fisicamente) era **PRETO**, a propriedade de altura negra é violada. Um algoritmo de correção rearranja as cores e rotaciona a árvore para eliminar o "duplo preto".

---

## 1. Pseudocódigo Geral da Remoção

```text
FUNCAO remover(raiz, chave)
    no = buscar(raiz, chave)
    SE no == NULO ENTAO RETORNE raiz

    // Guardar as cores originais para saber se quebrou o balanceamento
    cor_original_substituido = no.cor
    no_auxiliar = NULO

    SE no.esquerdo == NULO ENTAO
        no_auxiliar = no.direito
        transplantar(raiz, no, no.direito)
    SENAO SE no.direito == NULO ENTAO
        no_auxiliar = no.esquerdo
        transplantar(raiz, no, no.esquerdo)
    SENAO
        // Nó tem dois filhos: pegar o sucessor (menor da subárvore direita)
        sucessor = minimo(no.direito)
        cor_original_substituido = sucessor.cor
        no_auxiliar = sucessor.direito

        SE sucessor.pai == no ENTAO
            no_auxiliar.pai = sucessor // se no_auxiliar não for NULO (sentinela)
        SENAO
            transplantar(raiz, sucessor, sucessor.direito)
            sucessor.direito = no.direito
            sucessor.direito.pai = sucessor
        
        transplantar(raiz, no, sucessor)
        sucessor.esquerdo = no.esquerdo
        sucessor.esquerdo.pai = sucessor
        sucessor.cor = no.cor

    // Se a cor removida era preta, precisamos corrigir
    SE cor_original_substituido == PRETO ENTAO
        remover_fixup(raiz, no_auxiliar)

    RETORNE raiz
FIM_FUNCAO
```

---

## 2. O Algoritmo de Correção (`remover_fixup`)

O nó `x` começa como o "duplo preto". O laço roda enquanto `x` não for a raiz e sua cor for **PRETA**.

*   **Caso 1**: O irmão `w` de `x` é **VERMELHO**.
    *   *Ação*: Trocar as cores de `w` e do pai de `x`. Rotacionar o pai de `x` na direção de `x`. O novo irmão de `x` agora será preto. Cai em um dos casos abaixo.
*   **Caso 2**: O irmão `w` é **PRETO** e ambos os filhos de `w` são **PRETOS**.
    *   *Ação*: Mudar a cor de `w` para **VERMELHO**. Subir o problema de duplo preto para o pai (`x = x.pai`).
*   **Caso 3**: O irmão `w` é **PRETO**, o filho de `w` mais próximo de `x` é **VERMELHO** e o mais distante é **PRETO**.
    *   *Ação*: Trocar as cores de `w` e do seu filho vermelho próximo. Rotacionar `w` para longe de `x`. Isso transforma o Caso 3 no Caso 4.
*   **Caso 4**: O irmão `w` é **PRETO** e o filho de `w` mais distante de `x` é **VERMELHO**.
    *   *Ação*: Mudar a cor de `w` para a cor do pai de `x`. Mudar o pai de `x` para **PRETO**. Mudar o filho distante de `w` para **PRETO**. Rotacionar o pai de `x` na direção de `x`. O duplo preto é totalmente eliminado (`x = raiz`).

---

## 3. Implementação Completa em Python

Abaixo está o código executável contendo a lógica completa de remoção de acordo com o livro *Algoritmos: Teoria e Prática (Cormen)*.

```python
class No:
    def __init__(self, dado, cor="VERMELHO"):
        self.dado = dado
        self.cor = cor  # "VERMELHO" ou "PRETO"
        self.esquerdo = None
        self.direito = None
        self.pai = None

class ArvoreRubroNegra:
    def __init__(self):
        # Utiliza um nó sentinela para representar folhas (NULO) com cor PRETA
        self.NULO = No(0, cor="PRETO")
        self.raiz = self.NULO

    def rotacionar_esquerda(self, x):
        y = x.direito
        x.direito = y.esquerdo
        if y.esquerdo != self.NULO:
            y.esquerdo.pai = x
        y.pai = x.pai
        if x.pai == None:
            self.raiz = y
        elif x == x.pai.esquerdo:
            x.pai.esquerdo = y
        else:
            x.pai.direito = y
        y.esquerdo = x
        x.pai = y

    def rotacionar_direita(self, x):
        y = x.esquerdo
        x.esquerdo = y.direito
        if y.direito != self.NULO:
            y.direito.pai = x
        y.pai = x.pai
        if x.pai == None:
            self.raiz = y
        elif x == x.pai.direito:
            x.pai.direito = y
        else:
            x.pai.esquerdo = y
        y.direito = x
        x.pai = y

    def transplantar(self, u, v):
        if u.pai == None:
            self.raiz = v
        elif u == u.pai.esquerdo:
            u.pai.esquerdo = v
        else:
            u.pai.direito = v
        v.pai = u.pai

    def minimo(self, no):
        while no.esquerdo != self.NULO:
            no = no.esquerdo
        return no

    def buscar(self, no, chave):
        if no == self.NULO or chave == no.dado:
            return no
        if chave < no.dado:
            return self.buscar(no.esquerdo, chave)
        return self.buscar(no.direito, chave)

    def remover(self, chave):
        no = self.buscar(self.raiz, chave)
        if no == self.NULO:
            print(f"Chave {chave} não encontrada.")
            return

        cor_original = no.cor
        if no.esquerdo == self.NULO:
            x = no.direito
            self.transplantar(no, no.direito)
        elif no.direito == self.NULO:
            x = no.esquerdo
            self.transplantar(no, no.esquerdo)
        else:
            sucessor = self.minimo(no.direito)
            cor_original = sucessor.cor
            x = sucessor.direito
            if sucessor.pai == no:
                x.pai = sucessor
            else:
                self.transplantar(sucessor, sucessor.direito)
                sucessor.right = no.direito
                sucessor.direito.pai = sucessor
            
            self.transplantar(no, sucessor)
            sucessor.esquerdo = no.esquerdo
            sucessor.esquerdo.pai = sucessor
            sucessor.cor = no.cor

        if cor_original == "PRETO":
            self.remover_fixup(x)

    def remover_fixup(self, x):
        while x != self.raiz and x.cor == "PRETO":
            if x == x.pai.esquerdo:
                w = x.pai.direito # Irmão
                
                # Caso 1: Irmão é Vermelho
                if w.cor == "VERMELHO":
                    w.cor = "PRETO"
                    x.pai.cor = "VERMELHO"
                    self.rotacionar_esquerda(x.pai)
                    w = x.pai.direito
                
                # Caso 2: Irmão é Preto e ambos os filhos são pretos
                if w.esquerdo.cor == "PRETO" and w.direito.cor == "PRETO":
                    w.cor = "VERMELHO"
                    x = x.pai
                else:
                    # Caso 3: Irmão é Preto, filho esquerdo é Vermelho, direito é Preto
                    if w.direito.cor == "PRETO":
                        w.esquerdo.cor = "PRETO"
                        w.cor = "VERMELHO"
                        self.rotacionar_direita(w)
                        w = x.pai.direito
                    
                    # Caso 4: Irmão é Preto, filho direito é Vermelho
                    w.cor = x.pai.cor
                    x.pai.cor = "PRETO"
                    w.direito.cor = "PRETO"
                    self.rotacionar_esquerda(x.pai)
                    x = self.raiz
            else:
                # Lógica espelhada para quando x é o filho direito
                w = x.pai.esquerdo
                if w.cor == "VERMELHO":
                    w.cor = "PRETO"
                    x.pai.cor = "VERMELHO"
                    self.rotacionar_direita(x.pai)
                    w = x.pai.esquerdo

                if w.direito.cor == "PRETO" and w.esquerdo.cor == "PRETO":
                    w.cor = "VERMELHO"
                    x = x.pai
                else:
                    if w.esquerdo.cor == "PRETO":
                        w.direito.cor = "PRETO"
                        w.cor = "VERMELHO"
                        self.rotacionar_esquerda(w)
                        w = x.pai.esquerdo

                    w.cor = x.pai.cor
                    x.pai.cor = "PRETO"
                    w.esquerdo.cor = "PRETO"
                    self.rotacionar_direita(x.pai)
                    x = self.raiz
        x.cor = "PRETO"
