CC = gcc
CFLAGS = -O2 -Wall -Wextra -std=c11
BIN = trabalho.exe

SRC = main.c preparar_dados.c consultas.c comparadores.c fn_string.c arquivos.c
HDR = tipos.h preparar_dados.h consultas.h comparadores.h fn_string.h arquivos.h

all: $(BIN)

$(BIN): $(SRC) $(HDR)
	$(CC) $(CFLAGS) -o $(BIN) $(SRC)

run: $(BIN)
	./$(BIN)

clean:
	rm -f $(BIN)

.PHONY: all run clean
