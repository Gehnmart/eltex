#include <stdio.h>
#include <string.h>

#define BUFF_MAX 10
#define ABONENT_MAX 100
#define SUCCESS 1
#define FAIL 0

typedef struct {
  char name[BUFF_MAX];
  char second_name[BUFF_MAX];
  char tel[BUFF_MAX];
} abonent;

int get_abonent_len(abonent *abonent_list) {
  int len = 0;
  for (; abonent_list[len].name[0] != 0; len++)
    ;
  return len;
}

void buff_clean(){
  while(getchar() != '\n');
}

int add_abonent(abonent *abonent_list) {
  int len = get_abonent_len(abonent_list);
  if (len > 100) {
    printf("Abonent list overflow");
    return FAIL;
  }
  printf("Enter name:\n");
  buff_clean();
  scanf("%9s", abonent_list[len].name);

  printf("Enter second_name:\n");
  buff_clean();
  scanf("%9s", abonent_list[len].second_name);

  printf("Enter tel:\n");
  buff_clean();
  scanf("%9s", abonent_list[len].tel);

  buff_clean();

  return SUCCESS;
}

int delete_abonent(abonent *abonent_list) {
  char search_name[BUFF_MAX];
  printf("Enter name for delete abonent:\n");
  scanf("%9s", search_name);

  for (int i = 0; i < ABONENT_MAX; i++) {
    if (abonent_list[i].name[0] != 0) {
      for (int k = 0; search_name[k]; k++) {
        if (abonent_list[i].name[k] == search_name[k]) {
          if (search_name[k + 1] == 0) {
            memset(abonent_list[i].name, 0, BUFF_MAX);
            memset(abonent_list[i].second_name, 0, BUFF_MAX);
            memset(abonent_list[i].tel, 0, BUFF_MAX);
          }
        } else
          break;
      }
    }
  }

  return SUCCESS;
}

int search_abonent(abonent *abonent_list) {
  char search_name[BUFF_MAX];
  printf("Enter name for search abonent:\n");
  scanf("%9s", search_name);

  for (int i = 0; i < ABONENT_MAX; i++) {
    if (abonent_list[i].name[0] != 0) {
      for (int k = 0; search_name[k]; k++) {
        if (abonent_list[i].name[k] == search_name[k]) {
          if (search_name[k + 1] == 0) {
            printf(
                "abonent: %d\n"
                "  name: %s\n"
                "  second: %s\n"
                "  tel: %s\n",
                i+1, abonent_list[i].name, abonent_list[i].second_name,
                abonent_list[i].tel);
          }
        } else
          break;
      }
    }
  }

  return SUCCESS;
}

void print_all_abonents(abonent *abonent_list) {
  for (int i = 0; i < ABONENT_MAX; i++) {
    if (abonent_list[i].name[0] != 0) {
      printf(
          "abonent: %d\n"
          "  name: %s\n"
          "  second: %s\n"
          "  tel: %s\n",
          i, abonent_list[i].name, abonent_list[i].second_name,
          abonent_list[i].tel);
    }
  }
}

int menu() {
  int option = 0, err;
  printf(
      "1) Add abonent\n"
      "2) Delete abonent\n"
      "3) Search abonent for name\n"
      "4) Print all abonent\n"
      "5) Exit\n");
  err = scanf("%d", &option);
  if (err != 1) {
    buff_clean();
    option = 0;
  }
  return option;
}

int main() {
  abonent abonent_list[ABONENT_MAX] = {0};

  for (int option = 0; option != 5;) {
    option = menu();
    switch (option) {
      case 1:
        add_abonent(abonent_list);
        break;
      case 2:
        delete_abonent(abonent_list);
        break;
      case 3:
        search_abonent(abonent_list);
        break;
      case 4:
        print_all_abonents(abonent_list);
        break;
      case 5:
        break;
      default:
        printf("Operation not found if you want exit type 5\n");
        break;
    }
  }

  return 0;
}