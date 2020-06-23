SRC_DIR := src
OBJ_DIR := obj
BIN_DIR := .

SRC := $(wildcard $(SRC_DIR)/*.c)
OG_OBJ := $(SRC:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)
OBJ := $(filter-out obj/test.o, $(OG_OBJ))
OBJ := $(filter-out obj/vector_test.o, $(OBJ))
OBJ_TEST := $(filter-out obj/editor.o, $(OG_OBJ))
OBJ_TEST := $(filter-out obj/vector_test.o, $(OBJ_TEST))
OBJ_VECT := $(filter-out obj/editor.o, $(OG_OBJ))
OBJ_VECT := $(filter-out obj/test.o, $(OBJ_VECT))

CC      = gcc
CFLAGS  = -Wall -g -D_DEFAULT_SOURCE -std=c99 -Werror -pedantic
PROGS   = editor normal_test vector_test
LDFLAGS	= -lncurses

.SUFFIXES: .c .o
.PHONY: all clean
all: $(PROGS)

editor: $(OBJ)
	$(CC) $^ -o $@ $(LDFLAGS)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c | $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(BIN_DIR) $(OBJ_DIR):
	mkdir -p $@


normal_test: $(OBJ_TEST)
	$(CC) $^ -o $@ $(LDFLAGS)

vector_test: $(OBJ_VECT)
	$(CC) $^ -o $@ $(LDFLAGS)

test: normal_test vector_test
	./normal_test
	./vector_test
	@$(RM) -v normal_test vector_test

clean:
	@$(RM) -rv $(OBJ_DIR) $(PROGS)

-include $(OBJ:.o=.d)
