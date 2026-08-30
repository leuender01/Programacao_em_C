# Regras:
- Todo no e rumbro ou negrod
- a raiz e sempre negra
- todo no nulo e negro
- o pai de um no rumbro e sempre negro
- Qualquer caminho de um no ate um no nulo tem o mesmo numero de nos negros
- cada novo no inserido e RUMBRO
- insercao e igual a um ABB
- Apos a insercao ver se as propridades da rumbro negra se mantem
- Se o pai do novo no for negro todas as propridades se mantem
- Se o pai do novo no for rumbro rotações alterações precisam se feitas

## Caso 1
### Seo pai e o tio do novo no sao Rumbros 
- __Pai__ e tio ficam negros
- __Avo__ fica Rumbro
### Se o pai do avo for Rumbro, inicia novamente o processo de verificacao das cores

```OBS
    atualize o novo no para o avô
```
- O __pai__ e o __tio__ ficam Negros
- __Vo__ dica Rubro
- novo no = __avô__

## Caso 2
### O pai e rubro e o tio e negro

#### __Rotaçao Simples__direita
----__Pai__ do novo no e __filho__ esquerdo e o novo no e __filho__ esquerdo-----
- __Pai__ fica negro 
- __Avo__ fica Rubro
- Rotaciona o __avo__ para direita

### __Rotaçao Dupla__direira_esquerda
----__Pai__ do novo no e __filho__ esquerdo e o novo no e __filho__ direito-----
- Rotaciona __Pai__ para a esquerda
- o novo no = __filho__ esquerdo do novo no
- __Pai__ fica negro
- __Avo__ fica Rubro e Rotaciona o __avo__ para a direita
