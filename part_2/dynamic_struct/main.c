#include "abonent_note.h"
#include "abonent_note_ui.h"

int main() {
  abonent_control_t abonent_list_control = {0};
  InitAbonentList(&abonent_list_control);
  int option = 0;
  while ((option = AbonentNoteUi()) != EXIT) {
    PerformAction(option, &abonent_list_control);
  }
  DestroyAbonentList(&abonent_list_control);

  return 0;
}