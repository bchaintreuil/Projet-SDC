#include <string.h>
#include <stdio.h>
#include "jeu.h"


char input[32];

int main(int argc, char *argv[]) {
    do {
        clrscr();
        printf("Bienvenue dans le jeu des petits chevaux\n\nQue souhaitez-vous faire ?\n[1] Nouvelle partie\n[2] Charger une partie\n[3] Quitter\n\nChoix : ");
        getInput(input);
        if (!strcmp(input, "1")) {
            game(NEW);
        } else if (!strcmp(input, "2")) {
            game(LOAD);
        } else if (!strcmp(input, "3")) {
            return 0;
        } else {
            continue;
        }
    } while (1);
}
