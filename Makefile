CC=gcc
CFLAG=-O2 -Werror -Wextra -Wall -Winline -std=c99 -pthread
#FSAN=-fno-common -fno-omit-frame-pointer -fsanitize=leak -fsanitize=address
SRC=src/AutoFree.c
INCLUDE=include/
EXE=auto-free-test

NORMAL=\e[m
YELLOW=\033[1;33m

check: $(SRC) $(INCLUDE)
	$(CC) -o $(EXE) $(SRC) -I$(INCLUDE) $(CFLAG) -g -D__AUTOFREE_TEST__  $(FSAN)
	@echo "Using $(YELLOW)valgrind$(NORMAL) to detect memory leak."
	valgrind --leak-check=full --errors-for-leak-kinds=all --error-exitcode=1 ./$(EXE)

hooks:
	@scripts/install-git-hooks

obj: $(SRC)
	$(CC) -c $(SRC) -I$(INCLUDE) $(CFLAG)

.PHONY: clean
clean:
	rm -f *.o $(EXE)
