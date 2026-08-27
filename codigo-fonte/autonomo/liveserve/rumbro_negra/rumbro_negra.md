### Estrutura dos Nós e Variáveis Globais

```markdown
Estrutura Nó:
    - key: Valor do nó (inteiro)
    - color: Cor do nó (vermelho ou preto) [ENUM]
    - left, right, parent: Ponteiros para os nós filhos e pai

Variável Global:
    - NIL: Um nó especial que representa folhas (NIL = new Node(), cor=NIL)
```

### Estrutura da Árvore Vermelhos-Preto

```markdown
Estrutura Árvore Vermelhos-Preto:
    - root: Ponteiro para a raiz da árvore
```

### Função de Inserção

```markdown
Procedimento Insert(T : RBTree, z : Node):
    y := NIL
    x := T.root

    Enquanto x != NIL:
        y := x
        Se z.key < x.key Então
            x := x.left
        Senão
            x := x.right

    z.parent := y
    Se y == NIL:
        T.root := z
    Senaí z.key < y.key Então
        y.left := z
    Senão
        y.right := z

    z.left := NIL
    z.right := NIL
    z.color := RED

    InsertFixup(T, z)
```

### Função de Balanceamento Após Inserção (InsertFixup)

```markdown
Procedimento InsertFixup(T : RBTree, z : Node):
    Enquanto z.parent.color == RED:
        Se z.parent == z.parent.parent.left Então
            y := z.parent.parent.right
            Se y.color == RED Então
                z.parent.color := BLACK
                y.color := BLACK
                z.parent.parent.color := RED
                z := z.parent.parent
            Senão
                Se z == z.parent.right Então
                    z := z.parent
                    LeftRotate(T, z)
                RightRotate(T, z.parent.parent)
                z.parent.color := BLACK
                z.parent.parent.color := RED
        Senão
            y := z.parent.parent.left
            Se y.color == RED Then
                z.parent.color := BLACK
                y.color := BLACK
                z.parent.parent.color := RED
                z := z.parent.parent
            Senão
                Se z == z.parent.left Então
                    z := z.parent
                    RightRotate(T, z)
                LeftRotate(T, z.parent.parent)
                z.parent.color := BLACK
                z.parent.parent.color := RED

    T.root.color := BLACK
```

### Função de Busca

```markdown
Função Search(T : RBTree, k : int) : Node:
    x := T.root

    Enquanto x != NIL E k != x.key :
        Se k < x.key Então
            x := x.left
        Senão
            x := x.right

    Devolver x
```

### Função de Rotações

```markdown
Procedimento LeftRotate(T : RBTree, x : Node):
    y := x.right

    x.right := y.left
    Se y.left != NIL Então
        y.left.parent := x

    y.parent := x.parent
    Se x.parent == NIL Then
        T.root := y
    Senaí x == x.parent.left Then
        x.parent.left := y
    Senão
        x.parent.right := y

    y.left := x
    x.parent := y
```

```markdown
Procedimento RightRotate(T : RBTree, y : Node):
    x := y.left

    y.left := x.right
    Se x.right != NIL Then
        x.right.parent := y

    x.parent := y.parent
    Se y.parent == NIL Then
        T.root := x
    Senaí y == y.parent.right Then
        y.parent.right := x
    Senão
        y.parent.left := x

    x.right := y
    y.parent := x
```

### Resumo:

1. **Inserção**: Insere um novo nó na árvore e utiliza `InsertFixup` para manter as propriedades de balanceamento.
2. **Busca**: Percorre a árvore até encontrar o valor desejado ou chegar em uma folha (NIL).
3. **Rotações**: Utilizadas para realizar as rotações necessárias durante a inserção e remoção.

Este pseudocódigo fornece um esboço de como implementar uma Árvore Vermelhos-Preto em C. Você pode usar essas operações básicas
como base para expandir e implementar toda a estrutura da árvore Vermelhos-Preto.
