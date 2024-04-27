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
} Abonent;

typedef struct {
  int len;
  Abonent *abonent_list;
} AbonentControll;

void InitAbonentList(AbonentControll *abonent_control);
void DestroyAbonentList(AbonentControll *abonent_control);
void ErrorHandler(AbonentControll *abonent_control, int received,
                  int expected);
int PerformAction(int option, AbonentControll *abonent_control);

#endif  // DYNAMIC_STRUCT_ABONENT_NOTE_H