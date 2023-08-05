C=gcc
CFLAGS=-Wall -Werror -Wextra
BUILD_PATH=./

all: s21_grep

s21_grep: s21_grep.c s21_grep.h
	$C $(CFLAGS) s21_grep.c -o $(BUILD_PATH)s21_grep

rebuild: clean all

clean:
	rm $(BUILD_PATH)s21_grep
