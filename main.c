#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "jeu.h"

// TODO : Prendre en compte les minuscule aussi dans les menus

int main(int argc, char *argv[]) {
    char *input;
    while (1) {
        clrscr();
        printf("Bienvenue dans le jeu des petits chevaux\n\nQue souhaitez-vous faire ?\n[1] Nouvelle partie\n[2] Charger une partie\n[3] Quitter\n\nChoix : ");
        input = getInput();
        if (!strcmp(input, "1")) {
            game(NEW);
        } else if (!strcmp(input, "2")) {
            game(LOAD);
        } else if (!strcmp(input, "3")) {
            free(input);
            return 0;
        }
    }
}
