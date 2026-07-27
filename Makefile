#=========Variaveis==========

CC= clang
FLAG= -Wall
CMD= clear
OBJ_DIR= ./objects
BIN_DIR= ./bin
COD_DIR= ./codigo-fonte
DIRETORIOS = $(OBJ_DIR) $(BIN_DIR)

ARVORE_RUMBRO_NEGRA_COD= $(COD_DIR)/arvores/arvore_rumbro_negra.c
ARVORE_RUMBRO_NEGRA_OBJ= $(COD_DIR)/arvores/arvore_rumbro_negra.o
ARVORE_RUMBRO_NEGRA_HEADER= $(COD_DIR)/arvores/arvore_rumbro_negra.h
QUEUE_NEGRA= $(OBJ_DIR)/Queue_negra.o

ARVORE_AVL_COD= $(COD_DIR)/arvores/arvore-binaria-de-busca.c $(COD_DIR)/arvores/arvore-de-busca-AVL.c $(COD_DIR)/estrutura_de_dados/Queue.c
ARVORE_AVL_OBJ := $(addprefix $(OBJ_DIR)/, $(notdir $(ARVORE_AVL_COD:.c=.o)))
ARVORE_AVL_HEADER = $(COD_DIR)/arvores/arvore-binaria-de-busca.h $(COD_DIR)/arvores/arvore-base.h

MERGE_SORT_COD= $(COD_DIR)/algoritimos_ordenação/merge_sort.c
MERGE_SORT_HEADER = $(COD_DIR)/arvores/merge_sort.h
MERGE_SORT_OBJ := $(addprefix $(OBJ_DIR)/, $(notdir $(MERGE_SORT_COD:.c=.o)))

HASH_COD = $(COD_DIR)/estrutura_de_dados/hash.c
HASH_HEADER = $(COD_DIR)/estrutura_de_dados/hash.h

TETRIS_JOGO = ./codigo-fonte/jogos/tetris.c
JOGO-DA-VELHA = ./codigo-fonte/jogos/jogo-da-velha.c
DOCKERVIL = ./codigo-fonte/autonomo/dockervil.c
SNAKE = ./codigo-fonte/jogos/snake.c
LIVE_SERVE = ./codigo-fonte/autonomo/live_serve.c

## Ainda em produção

##terminal: $(DIRETORIOS)
## 
##	$(CC) $(FLANG) -o ./bin/terminal ./codigo-fonte/autonomo/terminal.c -lSDL3 -lSDL3_ttf
##	./bin/terminal
##	./verificar.sh
snake: $(DIRETORIOS)

	$(CC) $(FLAG) $(SNAKE) -o ./bin/snake -lncurses
	./verificar.sh

live_serve: $(DIRETORIOS)

	$(CC) $(FLAG) $(LIVE_SERVE) -o ./bin/live_serve -pthread -lcrypto
	./verificar.sh

dockervil: $(DIRETORIOS)

	$(CC) $(FLAG) $(DOCKERVIL) -o  ./bin/dockervil -lncurses
	./bin/dockervil
	./verificar.sh

jogo-da-velha: $(DIRETORIOS)

	$(CC) $(FLAG) $(JOGO-DA-VELHA) -o ./bin/jogo-da-velha -lncurses
	./bin/jogo-da-velha
	./verificar.sh


tetris: $(DIRETORIOS) 

	$(CC) $(FLAG) $(TETRIS_JOGO) -o ./bin/tetris -lncurses
	./bin/tetris
	./verificar.sh

#Ainda em produção

#arvore_rumbro: $(DIRETORIOS) $(ARVORE_RUMBRO_NEGRA_COD) $(QUEUE_NEGRA)
#	$(CC) $(FLAG) $(ARVORE_RUMBRO_NEGRA_COD) $(QUEUE_NEGRA) -o $(BIN_DIR)/arvorerumbro
#	./verificar.sh
#	./$(BIN_DIR)/arvorerumbro

$(QUEUE_NEGRA): $(COD_DIR)/arvores/Queue_negra.h
	$(CC) -c $(COD_DIR)/arvores/Queue_negra.c -o $(QUEUE_NEGRA)

hash: $(DIRETORIOS)  $(HASH_HEADER)  
	
	$(CC) $(FLAG) $(HASH_COD) -o $(BIN_DIR)/hash
	./verificar.sh
	./$(BIN_DIR)/hash

##------------------ALGORITIMO_MERGE_SORT-------------

merge_sort: $(DIRETORIOS) $(MERGE_SORT_OBJ) 
	$(CC) $(FLAG)  $(MERGE_SORT_OBJ) $(COD_DIR)/algoritimos_ordenação/teste_merge.c -o $(BIN_DIR)/merge_sort
	./verificar.sh
	./$(BIN_DIR)/merge_sort

$(MERGE_SORT_OBJ):
	@mkdir -p $(OBJ_DIR)
	$(CC) -c $(MERGE_SORT_COD) -o $(MERGE_SORT_OBJ)

##--------------------aRVORE_AVL-----------------------
arvore_avl: $(DIRETORIOS) $(ARVORE_AVL_OBJ) 
	$(CC) $(FLAG) -g $(ARVORE_AVL_OBJ) $(COD_DIR)/arvores/teste_avl.c -o $(BIN_DIR)/arvoreAvl
	./verificar.sh
	./$(BIN_DIR)/arvoreAvl

$(ARVORE_AVL_OBJ): $(ARVORE_AVL_COD) $(ARVORE_AVL_HEADER)

	@mkdir -p $(OBJ_DIR)
	$(CC) -c $(COD_DIR)/arvores/arvore-binaria-de-busca.c -o  $(OBJ_DIR)/arvore-binaria-de-busca.o
	$(CC) -c $(COD_DIR)/arvores/arvore-de-busca-AVL.c -o  $(OBJ_DIR)/arvore-de-busca-AVL.o
	$(CC) -c $(COD_DIR)/estrutura_de_dados/Queue.c -o  $(OBJ_DIR)/Queue.o

$(DIRETORIOS):
	@mkdir $(OBJ_DIR)
	@mkdir $(BIN_DIR)
	@mkdir ./libs

clean:
	rm $(OBJ_DIR)/*.o
