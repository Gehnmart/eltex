#include "helpers.h"

int Min(int a, int b) { return a > b ? b : a; }

void PrintError(const char *action, int line) {
  fprintf(stderr, "ERROR LINE-%d %s: %s\n", line, action, strerror(errno));
}