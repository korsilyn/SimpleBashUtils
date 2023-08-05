#include "s21_cat.h"

int main(int argc, char **argv) {
  if (argc > 1) {
    opts flags = {0};
    if (getOpts(argc, argv, &flags)) {
      output(argc, argv, &flags);
    }
  } else
    fprintf(stderr, "No arguments\n");
  return 0;
}

int getOpts(int argc, char **argv, opts *flags) {
  int opt;
  int option_index = 0;
  int success = 1;
  while ((opt = getopt_long(argc, argv, "beEnstTv", long_options,
                            &option_index)) != -1) {
    switch (opt) {
      case 'b':
        flags->b = 1;
        break;
      case 'e':
        flags->v = 1;
        flags->e = 1;
        break;
      case 'E':
        flags->e = 1;
        break;
      case 'n':
        flags->n = 1;
        break;
      case 's':
        flags->s = 1;
        break;
      case 't':
        flags->v = 1;
        flags->t = 1;
        break;
      case 'T':
        flags->t = 1;
        break;
      case 'v':
        flags->v = 1;
        break;
      default:
        fprintf(stderr, "Invalid option\n");
        success = 0;
        break;
    }
  }

  if (optind == argc) {
    fprintf(stderr, "No files were passed\n");
    success = 0;
  }

  return success;
}

void output(int argc, char **argv, opts *flags) {
  int emptyLine = 0;
  for (int i = optind; i < argc; i++) {
    FILE *fp = fopen(argv[i], "r");
    if (fp != NULL) {
      char line[10000];
      while (fgets(line, 10000, fp)) {
        if (line[0] == '\n')
          emptyLine++;
        else
          emptyLine = 0;
        if (emptyLine >= 2 && flags->s == 1) continue;
        numberLine(line, flags);
        filterLine(line, flags);
        // printf("%s", line);
      }
      //      if (argc - i - 1 > 0 && line[(strlen(line))] != '\n')
      //      printf("\n");
      fclose(fp);
    } else
      fprintf(stderr, "File not found\n");
  }
}

void numberLine(char line[10000], opts *flags) {
  static int lineNumber = 1;
  if (flags->b == 1) {
    if (line[0] != '\n' && line[0] != EOF) printf("%6d\t", lineNumber++);
  } else if (flags->n == 1) {
    printf("%6d\t", lineNumber++);
  }
}

void filterLine(char line[10000], opts *flags) {
  int symbol;
  for (unsigned long int i = 0; i < strlen(line); i++) {
    symbol = line[i];
    if (flags->v && symbol < 32 && symbol != 9 && symbol != 10) {
      printf("^%c", symbol + 64);
      continue;
    } else if (flags->v && symbol == 127) {
      printf("^%c", symbol - 64);
      continue;
    }
    if (flags->e && symbol == 10) {
      printf("$\n");
      continue;
    }
    if (flags->t && symbol == 9) {
      printf("^%c", symbol + 64);
      continue;
    }
    printf("%c", symbol);
  }
}
