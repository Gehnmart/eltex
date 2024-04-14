#include "abonent_note.h"

void InitAbonentList(abonent_control_t *abonent_control) {
  abonent_t *abonent_list = calloc(1, sizeof(abonent_t));
  if (abonent_list == NULL) {
    perror("Out of memory");
    exit(EXIT_FAILURE);
  }

  abonent_control->abonent_list = abonent_list;
  abonent_control->len = 1;
}

void DestroyAbonentList(abonent_control_t *abonent_control) {
  if (abonent_control != NULL) {
    if (abonent_control->abonent_list != NULL) {
      free(abonent_control->abonent_list);
    }
  }
}

abonent_control_t *AddAbonentInList(abonent_control_t *abonent_control,
                                    int next_len) {
  abonent_t *new_abonent_list =
      realloc(abonent_control->abonent_list, sizeof(abonent_t) * next_len);
  if (new_abonent_list == NULL) {
    perror("Out of memory");
    exit(EXIT_FAILURE);
  }

  abonent_control->abonent_list = new_abonent_list;
  abonent_control->len = next_len;

  return abonent_control;
}

abonent_control_t *DeleteAbonentInList(abonent_control_t *abonent_control,
                                       int index) {
  memmove(&abonent_control->abonent_list[index],
          &abonent_control->abonent_list[index + 1],
          (abonent_control->len - index - 1) * sizeof(abonent_t));
  abonent_control->len--;

  abonent_t *new_abonent_list = realloc(
      abonent_control->abonent_list, abonent_control->len * sizeof(abonent_t));
  if (new_abonent_list == NULL) {
    perror("Out of memory\n");
    exit(EXIT_FAILURE);
  }
  abonent_control->abonent_list = new_abonent_list;

  return abonent_control;
}

int AddAbonent(abonent_control_t *abonent_control) {
  if (abonent_control->len > 100) {
    printf("Abonent list overflow");
    return FAIL;
  }
  abonent_control = AddAbonentInList(abonent_control, abonent_control->len + 1);

  printf("Enter name:\n");
  BuffClean();
  InputWrap(scanf("%9s",
                  abonent_control->abonent_list[abonent_control->len - 1].name),
            1);

  printf("Enter second_name:\n");
  BuffClean();
  InputWrap(
      scanf(
          "%9s",
          abonent_control->abonent_list[abonent_control->len - 1].second_name),
      1);

  printf("Enter tel:\n");
  BuffClean();
  InputWrap(
      scanf("%9s", abonent_control->abonent_list[abonent_control->len - 1].tel),
      1);

  BuffClean();

  return SUCCESS;
}

int DeleteAbonent(abonent_control_t *abonent_control) {
  char search_name[BUFF_MAX];
  printf("Enter name for delete abonent:\n");
  InputWrap(scanf("%9s", search_name), 1);

  for (int i = 0; i < abonent_control->len; i++) {
    if (abonent_control->abonent_list[i].name[0] != 0) {
      for (int k = 0; search_name[k]; k++) {
        if (abonent_control->abonent_list[i].name[k] == search_name[k]) {
          if (search_name[k + 1] == 0) {
            DeleteAbonentInList(abonent_control, i);
          }
        } else
          break;
      }
    }
  }

  return SUCCESS;
}

int SearchAbonent(abonent_control_t *abonent_control) {
  char search_name[BUFF_MAX];
  printf("Enter name for search abonent:\n");
  InputWrap(scanf("%9s", search_name), 1);

  for (int i = 0; i < abonent_control->len; i++) {
    if (abonent_control->abonent_list[i].name[0] != 0) {
      for (int k = 0; search_name[k]; k++) {
        if (abonent_control->abonent_list[i].name[k] == search_name[k]) {
          if (search_name[k + 1] == 0) {
            printf(
                "abonent: %d\n"
                "  name: %s\n"
                "  second: %s\n"
                "  tel: %s\n",
                i + 1, abonent_control->abonent_list[i].name,
                abonent_control->abonent_list[i].second_name,
                abonent_control->abonent_list[i].tel);
          }
        } else
          break;
      }
    }
  }

  return SUCCESS;
}

void PrintAllAbonent(abonent_control_t *abonent_control) {
  for (int i = 0; i < abonent_control->len; i++) {
    if (abonent_control->abonent_list[i].name[0] != 0) {
      printf(
          "abonent: %d\n"
          "  name: %s\n"
          "  second: %s\n"
          "  tel: %s\n",
          i, abonent_control->abonent_list[i].name,
          abonent_control->abonent_list[i].second_name,
          abonent_control->abonent_list[i].tel);
    }
  }
}

int PerformAction(int option, abonent_control_t *abonent_control) {
  switch (option) {
    case 1:
      AddAbonent(abonent_control);
      break;
    case 2:
      DeleteAbonent(abonent_control);
      break;
    case 3:
      SearchAbonent(abonent_control);
      break;
    case 4:
      PrintAllAbonent(abonent_control);
      break;
    case 5:
      break;
    default:
      printf("Operation not found if you want exit type 5\n");
      break;
  }

  return 0;
}