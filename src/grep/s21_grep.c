#include "s21_grep.h"

int main(int argc, char **argv) {
  if (argc > 1) {
    opts flags = {0};
    char patterns[1000];
    if (getOpts(argc, argv, &flags, patterns)) {
      if (flags.o && (flags.l || flags.c)) flags.o = 0;
      if (flags.l && flags.c) flags.c = 0;
      if (flags.l && flags.h) flags.h = 0;
      // printf("%s\n\n", patterns);
      if (!(flags.o && flags.v)) printData(argc, argv, &flags, patterns);
    }
  } else
    fprintf(stderr, "No arguments\n");
  return 0;
}

int getOpts(int argc, char **argv, opts *flags, char patterns[1000]) {
  int opt;
  int success = 1;
  while ((opt = getopt(argc, argv, "e:ivclnhsf:o")) != -1) {
    switch (opt) {
      case 'e':
        flags->e = 1;
        fillPatterns(patterns, optarg, 'e', &success);
        if (!success) fprintf(stderr, "Usage: -e PATTERN\n");
        break;
      case 'i':
        flags->i = 1;
        break;
      case 'v':
        flags->v = 1;
        break;
      case 'c':
        flags->c = 1;
        break;
      case 'l':
        flags->l = 1;
        break;
      case 'n':
        flags->n = 1;
        break;
      case 'h':
        flags->h = 1;
        break;
      case 's':
        flags->s = 1;
        break;
      case 'f':
        flags->f = 1;
        fillPatterns(patterns, optarg, 'f', &success);
        if (!success) fprintf(stderr, "Usage: -f FILE\n");
        break;
      case 'o':
        flags->o = 1;
        break;
      default:
        fprintf(stderr, "Invalid option\n");
        success = 0;
        break;
    }
  }

  if (flags->e == 0 && flags->f == 0 && optind == argc) {
    fprintf(stderr, "No patterns were passed\n");
    success = 0;
  } else if (flags->e == 0 && flags->f == 0)
    strcpy(patterns, argv[optind++]);

  if (optind == argc) {
    fprintf(stderr, "No files were passed\n");
    success = 0;
  }

  return success;
}

void printData(int argc, char **argv, opts *flags, char patterns[1000]) {
  int multipleFiles = 0;
  if (argc - optind > 1) multipleFiles = 1;
  for (; optind < argc; optind++) {
    int currentLine = 1;
    int filenamePrinted = 0;
    int matchingLines = 0;
    int patternFound = 0;
    FILE *fp = fopen(argv[optind], "r");
    if (fp != NULL) {
      char line[10000];
      while (fgets(line, 10000, fp)) {
        regex_t regex;

        if (regcomp(&regex, patterns, flags->i ? REG_ICASE : 0) == 0) {
          if (regexec(&regex, line, 0, NULL, 0) == flags->v) {
            patternFound = 1;
            matchingLines++;
            printAux(argv[optind], currentLine, multipleFiles, patternFound,
                     &filenamePrinted, flags);
            if (!flags->c && !flags->l && !flags->o) {
              printf("%s", line);
              if (line[strlen(line) - 1] != '\n') printf("\n");
            } else if (!flags->c && !flags->l && !flags->v)
              printO(line, regex, flags, currentLine, multipleFiles,
                     argv[optind]);
          }
          regfree(&regex);
          currentLine++;
        }
      }
      if (flags->c && !flags->h && !patternFound &&
          (matchingLines || multipleFiles))
        printf("%s:", argv[optind]);
      if (flags->c) printf("%d\n", matchingLines);
      fclose(fp);
    } else if (!flags->s)
      fprintf(stderr, "File not found\n");
  }
}

void printAux(char *filename, int currentLine, int multipleFiles,
              int patternFound, int *filenamePrinted, opts *flags) {
  // Filename part
  if (patternFound && flags->l && !flags->h && !(*filenamePrinted)) {
    printf("%s\n", filename);
    *filenamePrinted = 1;
  } else if (!flags->c && !flags->l && !flags->h && multipleFiles)
    printf("%s:", filename);
  else if (!flags->l && !flags->h && multipleFiles && !(*filenamePrinted)) {
    printf("%s:", filename);
    *filenamePrinted = 1;
  }
  // Line number part
  if (patternFound && flags->n && !flags->l && !flags->c)
    printf("%d:", currentLine);
}

void fillPatterns(char patterns[1000], char *optarg, char flag, int *success) {
  static int currentFillness = 0;
  if (flag == 'e' && strlen(optarg) != 0) {
    if (currentFillness != 0) {
      patterns[currentFillness++] = '\\';
      patterns[currentFillness++] = '|';
    }
    patterns[currentFillness++] = '\\';
    patterns[currentFillness++] = '(';
    for (long unsigned int i = 0; i < strlen(optarg); i++)
      patterns[currentFillness++] = optarg[i];
    patterns[currentFillness++] = '\\';
    patterns[currentFillness++] = ')';
  } else if (flag == 'f' && strlen(optarg) != 0) {
    FILE *fp = fopen(optarg, "r");
    char symbol;
    char prevSymbol = '\n';
    int newPattern = 1;
    if (fp != NULL) {
      if (currentFillness != 0) {
        patterns[currentFillness++] = '\\';
        patterns[currentFillness++] = '|';
      }
      do {
        symbol = fgetc(fp);
        if (newPattern && symbol != EOF) {
          patterns[currentFillness++] = '\\';
          patterns[currentFillness++] = '(';
          newPattern = 0;
        }
        if (symbol == EOF && !newPattern) {
          patterns[currentFillness++] = '\\';
          patterns[currentFillness++] = ')';
          break;
        } else if (symbol == '\n' && prevSymbol == '\0') {
          prevSymbol = symbol;
          patterns[currentFillness++] = '\\';
          patterns[currentFillness++] = ')';
          patterns[currentFillness++] = '\\';
          patterns[currentFillness++] = '|';
          newPattern = 1;
        } else if (symbol == '\n' && prevSymbol == '\n') {
          prevSymbol = '\0';
          patterns[currentFillness++] = '\n';
          patterns[currentFillness++] = '\\';
          patterns[currentFillness++] = ')';
          patterns[currentFillness++] = '\\';
          patterns[currentFillness++] = '|';
          newPattern = 1;
        } else if (symbol != EOF) {
          prevSymbol = '\0';
          patterns[currentFillness++] = symbol;
        }
      } while (symbol != EOF);
      currentFillness -= 2;
      // patterns[currentFillness++] = '\\';
      // patterns[currentFillness++] = ')';
      patterns[currentFillness] = '\0';
      fclose(fp);
    } else {
      fprintf(stderr, "File with patterns not found\n");
      *success = 0;
    }
  } else
    *success = 0;
  patterns[currentFillness] = '\0';
}

void printO(char line[10000], regex_t regex, opts *flags, int currentLine,
            int multipleFiles, char *filename) {
  int firstTime = 1;
  while (regexec(&regex, line, 0, NULL, 0) == flags->v) {
    int end = strlen(line);
    char aux[10001];
    strcpy(aux, line);
    while (regexec(&regex, aux, 0, NULL, 0) == flags->v) {
      end--;
      aux[strlen(aux) - 1] = '\0';
    }
    aux[strlen(aux)] = line[strlen(aux)];
    int start = 0;
    while (regexec(&regex, aux, 0, NULL, 0) == flags->v && strlen(aux) > 0) {
      start++;
      for (int j = 0; aux[j] != '\0'; j++) aux[j] = aux[j + 1];
    }
    start--;
    int i = strlen(aux);
    for (; i != 0; i--) aux[i] = aux[i - 1];
    aux[0] = line[start];
    if (!firstTime) printAux(filename, currentLine, multipleFiles, 1, 0, flags);
    printf("%s\n", aux);
    i = start + 1;
    for (; line[i] != '\0'; i++) line[i - start - 1] = line[i];
    line[i - start - 1] = '\0';
    firstTime = 0;
  }
}
