#
# Makefile de EXEMPLO
#
# OBRIGATÓRIO ter uma regra "all" para geração da biblioteca e de uma
# regra "clean" para remover todos os objetos gerados.
#
# É NECESSARIO ADAPTAR ESSE ARQUIVO de makefile para suas necessidades.
#  1. Cuidado com a regra "clean" para não apagar o "support.o"
#
# OBSERVAR que as variáveis de ambiente consideram que o Makefile está no diretótio "cthread"
# 

CC=gcc
LIB_DIR=./lib
TEST_DIR=./testes
INC_DIR=./include
BIN_DIR=./bin
SRC_DIR=./src
CFLAGS = -Wall

all: main lib

main: cthread.o
	$(CC)  $(CFLAGS) -o $(BIN_DIR)/main $(BIN_DIR)/*.o
cthread.o: 
	$(CC) $(CFLAGS) -c $(SRC_DIR)/cthread.c -Wall -o $(BIN_DIR)/cthread.o

lib: cthread.o
	ar crs $(LIB_DIR)/libcthread.a $(BIN_DIR)/cthread.o $(BIN_DIR)/support.o

clean:
	find $(BIN_DIR) $(LIB_DIR) $(TEST_DIR) -type f ! -name 'support.o' ! -name '*.c' ! -name 'Makefile' -delete

