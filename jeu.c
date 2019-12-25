//
// Created by Benjamin CHAINTREUIL on 23/12/2019.
//

#include "jeu.h"
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <stdarg.h>

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

void getInput(char *input) { // FIXME : Input var global conflict with function var;
    fflush(stdin);
    input = fgets(input, sizeof(input) + 1, stdin);
    if (input[strlen(input) - 1] == '\n') {
        input[strlen(input) - 1] = '\0';
    }
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
    char input[32] = "";
    Player *players; // TODO: Alloc dyn nb joueurs
    char nbPlayers;

    switch (mode) {
        case NEW:
            if (fexists(SAVE)) {
                while (1) {
                    printf("Une sauvegarde d'une précèdente partie existe déjà ! Souhaitez-vous l'écraser ?\n\n[O] Oui\n[N] Non\n\nChoix : ");
                    getInput(input);
                    if (!strcmp(input, "O")) {
                        save = fopen(SAVE, "wb"); // TODO : Penser à close et checker l'ouverture !
                        break;
                    } else if (!strcmp(input, "N")) {
                        return;
                    } else {
                        clrscr();
                    }
                }
                while (1) {
                    printf("Nombre de Joueurs [1-4] : ");
                    getInput(input);
                    if (!strcmp(input, "1") || !strcmp(input, "2") || !strcmp(input, "3") || !strcmp(input, "4")) {
                        nbPlayers = input[0] - '0';
                        players = cmalloc(nbPlayers * sizeof(Player));
                        break;
                    }
                    clrscr();
                }
                while (1) {
                    printf("Jouer avec un joueur autonome ? \n\n[O] Oui\n[N] Non\n\nChoix : ");
                    getInput(input);
                    if (!strcmp(input, "O")) {
                        players[nbPlayers - 1].isIA = 1;
                        break;
                    } else if (!strcmp(input, "N")) {
                        break;
                    } else {
                        clrscr();
                    }
                }
                for (int i = 1; i <= nbPlayers; i++) {
                    if (players[i - 1].isIA) {
                        break;
                    }
                    printf("Nom du joueur #%d : ", i);
                    getInput(input);
                    players[i - 1].name = input;
                }
            }
            exit(0);
            break;
        case LOAD:
            if (!fexists(SAVE)) {
                printf("Aucune sauvegarde existante trouvée !");
                fflush(stdout); // Obliger de flush stdout pour que le printf s'affiche avant le sleep...
                delay(3);
                return;
            }
            save = fopen(SAVE, "rb+"); // TODO : Penser à close et checker l'ouverture !
            loadSave();
            break;
    }
}

void loadSave() {
    ;
}

void saveGame() {
    ;
}

void gc(void *arg1, ...) { // Un example de call : gc(arg1, ..., NULL);
    va_list args;
    void *element;
    free(arg1);
    va_start(args, arg1);
    while ((element = va_arg(args, void*)) != NULL) {
        free(element);
    }
    va_end(args);
}

void *cmalloc(size_t size) {
    void *pointer;
    pointer = malloc(size);
    if (pointer) {
        return pointer;
    }
    clrscr(); // TODO: Rise exception (Close stream file, free ram, etc...)
    printf("Erreur d'allocation : Mémoire vive insuffisante !");
    delay(3);
    fflush(stdout);
    exit(-1);
}