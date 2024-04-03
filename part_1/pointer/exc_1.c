#include <stdio.h>

int main() {
  int num = 0;
  char *ptr = (char *)&num;
  scanf("%d", (int *)(ptr + 2));
  printf("%d\n", num);
}