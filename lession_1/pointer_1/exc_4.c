#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BUFF_MAX 1024

char *strsearch(char *str, char *search) {
  char *res = NULL;

  for (int i = 0; str[i] && res == NULL; i++) {
    for (int k = 0; search[k] && res == NULL; k++) {
      if (search[k] == str[k + i]) {
        if (search[k + 1] == 0) {
          res = &str[i];
        }
      } else {
        break;
      }
    }
  }

  return res;
}

void lastNewLineRemove(char *str) {
  int len = strlen(str);

  if (len > 0 && str[len - 1] == '\n') {
    str[len - 1] = '\0';
  }
}

int main() {
  char str[BUFF_MAX] = {0};
  char search[BUFF_MAX] = {0};

  fgets(str, BUFF_MAX, stdin);
  fgets(search, BUFF_MAX, stdin);
  lastNewLineRemove(str);
  lastNewLineRemove(search);
  char *res = strsearch(str, search);
  if (res != NULL) printf("%s\n", res);
}
