#include "io.h"

static void PrintAllLibAndFunc(lib_controller_t *controller) {
  for (int i = 0; i < controller->size; i++) {
    printf("%d) %s\n", i, controller->libs[i].name);
    for (int j = 0; j < controller->libs[i].size; j++) {
      printf("  %d) %s\n", j, controller->libs[i].func[j].name);
    }
  }
}

static void PrintAllLib(lib_controller_t *controller) {
  for (int i = 0; i < controller->size; i++) {
    printf("%d) %s\n", i, controller->libs[i].name);
  }
}

static void PrintAllFuncInLib(lib_t *lib) {
  for (int j = 0; j < lib->size; j++) {
    printf("  %d) %s\n", j, lib->func[j].name);
  }
}

static void InputWrap(int received, int expected,
                      lib_controller_t *controller) {
  if (received != expected) {
    fprintf(stderr, "error: incorrected input\n");
    DestroyController(controller);
    exit(EXIT_FAILURE);
  }
}

static void ExecuteAction(int (*operation)(int, int),
                          lib_controller_t *controller) {
  int f, s;
  printf("enter two numbers to perform the operation\n");
  InputWrap(scanf("%d%d", &f, &s), 2, controller);
  printf("%d\n", operation(f, s));
}

int MenuLibInput(lib_controller_t *controller) {
  while (1) {
    char user_input[NAME_MAX];
    printf("enter library path for load or enter q for next step\n");
    InputWrap(scanf("%s", user_input), 1, controller);

    if (user_input[0] == 'q' && user_input[1] == '\0') {
      break;
    } else {
      if (SUCCESS == LibLoad(controller, user_input)) {
        while (1) {
          printf("enter func name to load\n");
          InputWrap(scanf("%s", user_input), 1, controller);
          if (user_input[0] == 'q' && user_input[1] == '\0') {
            break;
          } else {
            FuncLoad(&controller->libs[controller->size - 1], user_input);
          }
        }
      }
    }
  }
}

int MenuProcess(lib_controller_t *controller) {
  while (1) {
    int first = 0, second = 0;
    unsigned lib = 0, func = 0;

    printf("Change library and function (input format {Int Int})\n");

    PrintAllLibAndFunc(controller);
    InputWrap(scanf("%u %u", &lib, &func), 2, controller);

    if (lib > controller->size || func > controller->libs[lib].size) {
      printf("error: operation is not found\n");
    } else {
      ExecuteAction(controller->libs[lib].func[func].operation, controller);
    }
  }
}