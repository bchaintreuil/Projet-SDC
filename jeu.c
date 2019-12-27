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
const char startPos[] = {0, 14, 28, 42};
const char endPos[] = {55, 13, 27, 41};

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

// TODO: Penser à free input à la fermeture du jeu (Main - done; & game) ainsi que player.name
char *getInput() { // TODO: Raise except in case of alloc failure
    int i = 0;
    char c, *input;
    input = (char *) malloc(sizeof(char));

    if (input == NULL) {
        printf("Could not allocate memory!");
        exit(1);
    }

    fflush(stdin);
    while ((c = getchar()) != '\n') {
        realloc(input, (sizeof(char)));
        input[i++] = c;
    }
    input[i] = '\0';
    return input;
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
    char *input;
    Horse *board[56] = {NULL};

    /* Variables d'état */
    char nbPlayers;
    Player *players; // TODO: Alloc dyn nb joueurs
    /* */

    switch (mode) {
        case NEW:
            if (fexists(SAVE)) {
                while (1) {
                    printf("Une sauvegarde d'une précèdente partie existe déjà ! Souhaitez-vous l'écraser ?\n\n[O] Oui\n[N] Non\n\nChoix : ");
                    input = getInput();
                    if (!strcmp(input, "O")) {
                        save = fopen(SAVE, "wb"); // TODO : Penser à close et checker l'ouverture !
                        break;
                    } else if (!strcmp(input, "N")) {
                        return;
                    }
                    clrscr();
                }
            }
            while (1) {
                printf("Nombre de Joueurs [2-4] : ");
                input = getInput();
                if (!strcmp(input, "2") || !strcmp(input, "3") || !strcmp(input, "4")) {
                    nbPlayers = input[0] - '0';
                    players = (Player *) calloc(nbPlayers, sizeof(Player));
                    break;
                }
                clrscr();
            }
            while (1) {
                printf("Jouer avec un joueur autonome ? \n\n[O] Oui\n[N] Non\n\nChoix : ");
                input = getInput();
                if (!strcmp(input, "O")) {
                    players[nbPlayers - 1].isIA = 1;
                    break;
                } else if (!strcmp(input, "N")) {
                    break;
                }
                clrscr();
            }
            for (int i = 0; i < nbPlayers; i++) {
                if (players[i].isIA) {
                    players[i].name = "C-3PO"; // FIXME: strcpy();
                    break;
                }
                printf("Nom du joueur #%d : ", i + 1); // TODO: Nom vide
                input = getInput();
                players[i].name = input;
            }
            saveGame(save, nbPlayers, players);
            break;
        case LOAD:
            if (!fexists(SAVE)) {
                printf("Aucune sauvegarde existante trouvée !");
                fflush(stdout); // Obliger de flush stdout pour que le printf s'affiche avant le sleep...
                delay(3);
                return;
            }
            save = fopen(SAVE, "rb+"); // TODO : Penser à close et checker l'ouverture !
            loadSave(save, &nbPlayers, &players, board);
            break;
    }
    /* Début du jeu */
}

void loadSave(FILE *save, char *nbPlayers, Player **players,
              Horse *board[]) { // TODO: return boolean in case of success/fail
    size_t stringSize;
    fread(nbPlayers, sizeof(char), 1, save);
    *players = (Player *) calloc(*nbPlayers, sizeof(Player));
    fread(*players, sizeof(Player), *nbPlayers, save);
    for (int i = 0; i < *nbPlayers; i++) {
        fread(&stringSize, sizeof(size_t), 1, save);
        (*players)[i].name = (char *) calloc(sizeof(char), stringSize);
        fread((*players)[i].name, sizeof(char), stringSize, save);
        for (int j = 0; j < 4; j++) {
            if (((*players)[i].horses[j]).isOut) {
                board[((*players)[i].horses[j]).pos] = &((*players)[i].horses[j]);
            }
        }
    }
    fflush(save);
}

void saveGame(FILE *save, char nbPlayers, Player *players) { // TODO: return boolean in case of success/fail
    size_t stringSize;
    fwrite(&nbPlayers, sizeof(char), 1, save);
    fwrite(players, sizeof(Player), nbPlayers, save);
    for (int i = 0; i < nbPlayers; i++) {
        stringSize = strlen(players[i].name);
        fwrite(&stringSize, sizeof(size_t), 1, save);
        fwrite(players[i].name, sizeof(char), stringSize, save);
    }
    fflush(save);
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