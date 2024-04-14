#include "abonent_note_ui.h"

int AbonentNoteUi() {
  int option = 0, err;
  printf(
      "1) Add abonent\n"
      "2) Delete abonent\n"
      "3) Search abonent for name\n"
      "4) Print all abonent\n"
      "5) Exit\n");
  InputWrap(scanf("%d", &option), 1);
  return option;
}