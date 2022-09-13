CC=gcc
CFLAG=-O2 -Werror -Wextra -Wall -Winline -std=c99 
SRC=src/AutoFree.c
INCLUDE=include/
EXE=auto-free-test
time ?= 1

NORMAL=\e[m
YELLOW=\033[1;33m

test:
	$(CC) test.c $(SRC) -I$(INCLUDE) -O2 -Wextra -Wall -Winline -std=c99 -g -fsanitize=address -fsanitize=leak -fsanitize=undefined

all: $(SRC) $(INCLUDE)
	$(CC) -o $(EXE) $(SRC) -I$(INCLUDE) $(CFLAG) -g -fsanitize=leak -D__AUTOFREE_TEST__ -pthread
	@echo "Using $(YELLOW)LeakSanitizer$(NORMAL) to detect memory leak."
	./$(EXE)

hooks:
	@scripts/install-git-hooks

obj: $(SRC)
	$(CC) -c $(SRC) -I$(INCLUDE) $(CFLAG)

.PHONY: clean
clean:
	rm -f *.o $(EXE) $(EXE)
