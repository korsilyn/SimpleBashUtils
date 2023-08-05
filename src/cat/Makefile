C=gcc
CFLAGS=-Wall -Werror -Wextra
BUILD_PATH=./

all: s21_cat

s21_cat: s21_cat.c s21_cat.h
	$C $(CFLAGS) s21_cat.c -o $(BUILD_PATH)s21_cat

rebuild: clean all

clean:
	rm $(BUILD_PATH)s21_cat
