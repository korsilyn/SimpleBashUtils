#ifndef S21_GREP_H
#define S21_GREP_H

#include <getopt.h>
#include <regex.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
  int e;  // Lois
  int i;  // Lois
  int v;  // Lois
  int c;  // Outputting every line, need fix
  int l;  // Lois
  int n;  // Lois
  int h;  // Same as c flag
  int s;  // Lois
  int f;  // Lois
  int o;  // Not working at all, maybe another func
} opts;

int getOpts(int argc, char **argv, opts *flags, char patterns[1000]);
void printData(int argc, char **argv, opts *flags, char patterns[1000]);
void printAux(char *filename, int currentLine, int multipleFiles,
              int patternFound, int *filenamePrinted, opts *flags);
void printO(char line[10000], regex_t regex, opts *flags, int currentLine,
            int multipleFiles, char *filename);
void fillPatterns(char patterns[1000], char *optarg, char flag, int *success);

#endif
