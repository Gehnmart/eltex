#include <stdio.h>
#include <stdlib.h>

#include "calc.h"

void ExecuteAction(int (*operation)(int, int));
void InputWrap(int received, int expected);

int main() {
  while (1) {
    int first = 0, second = 0, operation = 0;
    printf(
        "1) Сложение\n"
        "2) Вычитание\n"
        "3) Умножение\n"
        "4) Деление\n"
        "5) Выход\n");

    InputWrap(scanf("%d", &operation), 1);

    switch (operation) {
      case 1:
        ExecuteAction(Add);
        break;
      case 2:
        ExecuteAction(Sub);
        break;
      case 3:
        ExecuteAction(Mul);
        break;
      case 4:
        ExecuteAction(Div);
        break;
      case 5:
        exit(EXIT_SUCCESS);
        break;
      default:
        printf("error: operation %d is not found\n", operation);
        break;
    }
  }

  return 0;
}

void ExecuteAction(int (*operation)(int, int)) {
  int f, s;
  InputWrap(scanf("%d%d", &f, &s), 2);
  printf("%d\n", operation(f, s));
}

void InputWrap(int received, int expected) {
  if (received != expected) {
    fprintf(stderr, "error: incorrected input\n");
    exit(EXIT_FAILURE);
  }
}