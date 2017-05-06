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
INC_DIR=./include
BIN_DIR=./bin
SRC_DIR=./src

all: main

#main: 
#	$(CC) -o $(BIN_DIR)/main $(SRC_DIR)/main.c $(BIN_DIR)/support.o -Wall
main: 
	$(CC) -o $(BIN_DIR)/main $(SRC_DIR)/main2.c $(BIN_DIR)/support.o -Wall

clean:
	rm -rf $(LIB_DIR)/*.a $(BIN_DIR)/main 


