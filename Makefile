CC=gcc
CFLAG=-O2 -Werror -Wextra -Wall -Winline -std=c99 -pthread
FSAN=-fsanitize=leak
SRC=src/AutoFree.c
INCLUDE=include/
EXE=auto-free-test

NORMAL=\e[m
YELLOW=\033[1;33m

all: $(SRC) $(INCLUDE)
	$(CC) -o $(EXE) $(SRC) -I$(INCLUDE) $(CFLAG) -g -D__AUTOFREE_TEST__  $(FSAN)
	@echo "Using $(YELLOW)LeakSanitizer$(NORMAL) to detect memory leak."
	@./$(EXE)

hooks:
	@scripts/install-git-hooks

obj: $(SRC)
	$(CC) -c $(SRC) -I$(INCLUDE) $(CFLAG)

.PHONY: clean
clean:
	rm -f *.o $(EXE)
