#ifndef DYNAMIC_STRUCT_ABONENT_NOTE_UI_H
#define DYNAMIC_STRUCT_ABONENT_NOTE_UI_H

#include <stdio.h>

#include "abonent_note.h"
#include "additional.h"

enum MenuOptions {
  ADD_ABONENT = 1,
  DELETE_ABONENT,
  SEARCH_ABONENT_FOR_NAME,
  SHOW_ALL_ABONENTS,
  EXIT
};

int AbonentNoteUi(AbonentControll *abonent_control);

#endif  // DYNAMIC_STRUCT_ABONENT_NOTE_UI_H