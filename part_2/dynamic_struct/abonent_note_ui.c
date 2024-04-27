#include "abonent_note_ui.h"

int AbonentNoteUi(AbonentControll *abonent_control) {
  int option = 0;
  printf(
      "1) Add abonent\n"
      "2) Delete abonent\n"
      "3) Search abonent for name\n"
      "4) Print all abonent\n"
      "5) Exit\n");
  ErrorHandler(abonent_control, scanf("%d", &option), 1);
  return option;
}