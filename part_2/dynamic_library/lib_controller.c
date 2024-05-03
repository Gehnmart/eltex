#include "lib_controller.h"

int LibLoad(lib_controller_t *controller, const char *filename) {
  void *library_handler = dlopen(filename, RTLD_LAZY);
  if (library_handler == NULL) {
    fprintf(
        stderr,
        "%s library not found, maybe incorected path or library not exist\n",
        filename);
    return FAILURE;
  }
  strncpy(controller->libs[controller->size].name, filename, USERNAME_MAX - 1);
  controller->libs[controller->size].handler = library_handler;
  controller->size++;

  return SUCCESS;
}

int FuncLoad(lib_t *library, const char *funcname) {
  printf("%s\n", funcname);
  void *func_handler = dlsym(library->handler, funcname);
  if (func_handler == NULL) {
    fprintf(
        stderr,
        "%s function not found, maybe incorected path or library not exist\n",
        dlerror());
    return FAILURE;
  }

  strncpy(library->func[library->size].name, funcname, USERNAME_MAX - 1);
  library->func[library->size].operation = func_handler;
  library->size++;

  return SUCCESS;
}

void DestroyController(lib_controller_t *controller) {
  for (int i = 0; i < controller->size; i++) {
    dlclose(controller->libs[i].handler);
  }
}