#ifndef DYNAMIC_STRUCT_ABONENT_NOTE_H
#define DYNAMIC_STRUCT_ABONENT_NOTE_H

#include <errno.h>
#include <malloc.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "additional.h"

#define BUFF_MAX 10
#define ABONENT_MAX 100
#define SUCCESS 1
#define FAIL 0

typedef struct {
  char name[BUFF_MAX];
  char second_name[BUFF_MAX];
  char tel[BUFF_MAX];
} abonent_t;

typedef struct {
  int len;
  abonent_t *abonent_list;
} abonent_control_t;

void InitAbonentList(abonent_control_t *abonent_list_control);
void DestroyAbonentList(abonent_control_t *abonent_list_control);
int PerformAction(int option, abonent_control_t *abonent_list_control);

#endif  // DYNAMIC_STRUCT_ABONENT_NOTE_H