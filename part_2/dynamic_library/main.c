#include <dlfcn.h>
#include <stdio.h>
#include <stdlib.h>

#include "io.h"
#include "lib_controller.h"

void ExecuteAction(int (*operation)(int, int));

int main() {
  lib_controller_t controller = {0};

  MenuLibInput(&controller);
  MenuProcess(&controller);

Exit:
  DestroyController(&controller);
  return 0;
}