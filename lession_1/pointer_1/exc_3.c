#include <stdio.h>

int main() {
  int num[10] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 0};
  int *ptr = num;
  for (int i = 0; i < 10; i++) printf("%d", *(ptr++));
}