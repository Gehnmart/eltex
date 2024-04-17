#include "abonent_note.h"
#include "abonent_note_ui.h"

int main() {
  abonent_control_t abonent_control = {0};
  InitAbonentList(&abonent_control);
  int option = 0;
  while ((option = AbonentNoteUi(&abonent_control)) != EXIT) {
    PerformAction(option, &abonent_control);
  }
  DestroyAbonentList(&abonent_control);

  return 0;
}