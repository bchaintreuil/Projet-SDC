//
// Created by Benjamin CHAINTREUIL on 23/12/2019.
//

#include "jeu.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#ifdef WIN32
#include <Windows.h>
#else

#include <unistd.h>

#endif

#define SAVE "save.bin"

void clrscr() {
#ifdef WIN32
    system("cls");
#else
    system("clear");
#endif
}

void delay(int seconds) {
#ifdef WIN32
    Sleep(seconds);
#else
    sleep(seconds);
#endif
}

void getInput(char *input) {
    input = fgets(input, sizeof(input) + 1, stdin);
    input[strlen(input) - 1] = '\0';
}

int dice() {
    return ((rand() % 6) + 1);
}

char fexists(char *filename) {
    FILE *file = fopen(filename, "r");
    if (file) {
        fclose(file);
        return 1;
    }
    return 0;
}

void game(char mode) {
    srand(time(NULL));
    FILE *save;
    State *gameState = initGame();
    switch (mode) {
        case NEW:
            if (fexists(SAVE)) {
                while (1) {
                    printf("Une sauvegarde d'une précèdente partie existe déjà ! Souhaitez-vous l'écraser ?\n\n[O] Oui\n[N] Non\n\nChoix : ");
                    getInput(input);
                    if (!strcmp(input, "O")) {
                        break;
                    } else if (!strcmp(input, "N")) {
                        return;
                    } else {
                        clrscr();
                    }
                }
            }
            save = fopen(SAVE, "wb"); // TODO : Penser à close !
            break;
        case LOAD:
            if (!fexists(SAVE)) {
                printf("Aucune sauvegarde existante trouvée !");
                fflush(stdout); // Obliger de flush stdout pour que le printf s'affiche avant le sleep...
                delay(3);
                return;
            }
            save = fopen(SAVE, "rb+"); // TODO : Penser à close !
            loadSave();
            break;
    }
}

State *initGame() {
    State *gameState = malloc(sizeof(State));
    while (1) {
        clrscr();
        printf("Combien de joueurs ? ([1-4]) : ");
        getInput(gameState->nbPlayers);
        printf("%c", gameState->nbPlayers);
        exit(0);
    }
    return &gameState;
}

void loadSave() {
    ;
}

void saveGame() {
    ;
}

void gc() {
    ;
}