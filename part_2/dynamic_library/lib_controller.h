#ifndef DYNAMIC_LIBRARY_LIB_LOADER_H
#define DYNAMIC_LIBRARY_LIB_LOADER_H

#include <dlfcn.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define SUCCESS 0
#define FAILURE 0x1

#define NAME_MAX 64
#define FUNC_MAX 64
#define LIB_MAX 64

typedef struct {
  char name[NAME_MAX];
  int (*operation)(int, int);
} func_t;

typedef struct {
  func_t func[FUNC_MAX];
  int size;
  char name[NAME_MAX];
  void *handler;
} lib_t;

typedef struct {
  lib_t libs[LIB_MAX];
  int size;
} lib_controller_t;

int LibLoad(lib_controller_t *controller, const char *filename);
int FuncLoad(lib_t *library, const char *funcname);
void DestroyController(lib_controller_t *controller);

#endif  // DYNAMIC_LIBRARY_LIB_LOADER_H