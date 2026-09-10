CC = gcc
CFLAGS = -Wall -Wextra -std=c11 -Iinclude -Iinclude/models

#archivos de fuente
OBJ = $(SRC:.c=.o)
SRC = src/main.c \
      src/models/catalog.c \
      src/models/code_list.c \
      src/models/course.c \
      src/models/group.c \
      src/models/schedule.c \
      src/models/student_history.c

#name del ejecutable
EXEC = cemestre

#comandos virtuales
.PHONY: all clean test

all: $(EXEC)

$(EXEC): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^

#compilacion de cada unidad
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

#limpieza de files compilados
clean:
	rm -f $(OBJ) $(EXEC) *.exe

#verificacion del entorno de compilación
test: all
	@echo "Entorno validado. Ejecutable listo"