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
#define NPC_NAME "C-3PO"

/* Colors */
#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_YELLOW  "\x1b[33m"
#define ANSI_COLOR_CYAN    "\x1b[36m"
#define ANSI_COLOR_RESET   "\x1b[0m"
/* ****** */

const char startPos[] = {0, 14, 28, 42};
const char endPos[] = {55, 13, 27, 41};
const char playerColor[4][10] = {ANSI_COLOR_CYAN, ANSI_COLOR_RED, ANSI_COLOR_GREEN, ANSI_COLOR_YELLOW};

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
    char NPCAlreadySet = 0;

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
            for (int i = 0; i < nbPlayers; i++) {
                if (NPCAlreadySet) {
                    while (1) {
                        printf("Nom du joueur #%d : ", i + 1);
                        input = getInput();
                        if (strlen(input) < 2 || strchr(input, ' ') != NULL) {
                            clrscr();
                            continue;
                        }
                        players[i].name = input;
                        break;
                    }
                } else {
                    while (1) {
                        printf("Nom du joueur #%d (Saisir \"NPC\" si joueur autonome) : ", i + 1);
                        input = getInput();
                        if (!strcmp(input, "NPC")) {
                            players[i].name = (char *) calloc(strlen(NPC_NAME), sizeof(char));
                            strcpy(players[i].name, NPC_NAME);
                            players[i].isNPC++;
                            NPCAlreadySet++;
                            break;
                        }
                        if (strlen(input) < 2 || strchr(input, ' ') != NULL) { // Check si ""
                            clrscr();
                            continue;
                        }
                        players[i].name = input;
                        break;
                    }
                }
                for (int j = 0; j < 4; j++) {
                    (players[i].horses[j]).playerID = i;
                    (players[i].horses[j]).horseID = j;
                }
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
    board[0] = &(players[0].horses[0]);
    displayBoard(nbPlayers, players, board);
    sleep(1);
    clrscr();
    for (int i = 1; i < 56; i++) {
        board[i] = board[i - 1];
        board[i - 1] = NULL;
        displayBoard(nbPlayers, players, board);
        sleep(1);
        clrscr();
    }

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

char *scat(const char *s1, const char *s2) {
    char *result = calloc(strlen(s1) + strlen(s2) + 1, sizeof(char));
    // TODO : in real code you would check for errors in malloc here
    strcpy(result, s1);
    strcat(result, s2);
    return result;
}

void displayBoard(char nbPlayer, Player *players, Horse *board[]) {
    char dpBoard[17][52] = {
            {32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 65, 66, 32, 68, 66, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 10},
            {32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 0,  32, 32, 0,  32, 32, 0,  32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 10},
            {32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 0,  32, 32, 49, 32, 32, 0,  32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 10},
            {32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 0,  32, 32, 50, 32, 32, 0,  32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 10},
            {32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 0,  32, 32, 51, 32, 32, 0,  32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 10},
            {32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 0,  32, 32, 52, 32, 32, 0,  32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 10},
            {32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 0,  32, 32, 53, 32, 32, 0,  32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 10},
            {68, 74, 32, 32, 0,  32, 32, 0,  32, 32, 0,  32, 32, 0,  32, 32, 0,  32, 32, 0,  32, 32, 0,  32, 32, 54, 32, 32, 0,  32, 32, 0,  32, 32, 0,  32, 32, 0,  32, 32, 0,  32, 32, 0,  32, 32, 0,  32, 32, 32, 10},
            {65, 74, 32, 32, 0,  32, 32, 49, 32, 32, 50, 32, 32, 51, 32, 32, 52, 32, 32, 53, 32, 32, 54, 32, 32, 88, 32, 32, 54, 32, 32, 53, 32, 32, 52, 32, 32, 51, 32, 32, 50, 32, 32, 49, 32, 32, 0,  32, 65, 82, 10},
            {32, 32, 32, 32, 0,  32, 32, 0,  32, 32, 0,  32, 32, 0,  32, 32, 0,  32, 32, 0,  32, 32, 0,  32, 32, 54, 32, 32, 0,  32, 32, 0,  32, 32, 0,  32, 32, 0,  32, 32, 0,  32, 32, 0,  32, 32, 0,  32, 68, 82, 10},
            {32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 0,  32, 32, 53, 32, 32, 0,  32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 10},
            {32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 0,  32, 32, 52, 32, 32, 0,  32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 10},
            {32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 0,  32, 32, 51, 32, 32, 0,  32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 10},
            {32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 0,  32, 32, 50, 32, 32, 0,  32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 10},
            {32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 0,  32, 32, 49, 32, 32, 0,  32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 10},
            {32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 0,  32, 32, 0,  32, 32, 0,  32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 10},
            {32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 68, 86, 32, 65, 86, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 10}
    };
    char *ptrBoard[] = {&dpBoard[1][28], &dpBoard[2][28], &dpBoard[3][28], &dpBoard[4][28], &dpBoard[5][28],
                        &dpBoard[6][28], &dpBoard[7][28], &dpBoard[7][31], &dpBoard[7][34], &dpBoard[7][37],
                        &dpBoard[7][40], &dpBoard[7][43], &dpBoard[7][46],
                        &dpBoard[8][46], &dpBoard[9][46], &dpBoard[9][43], &dpBoard[9][40], &dpBoard[9][37],
                        &dpBoard[9][34], &dpBoard[9][31], &dpBoard[9][28], &dpBoard[10][28], &dpBoard[11][28],
                        &dpBoard[12][28], &dpBoard[13][28], &dpBoard[14][28],
                        &dpBoard[15][28], &dpBoard[15][25], &dpBoard[15][22], &dpBoard[14][22], &dpBoard[13][22],
                        &dpBoard[12][22], &dpBoard[11][22], &dpBoard[10][22], &dpBoard[9][22], &dpBoard[9][19],
                        &dpBoard[9][16], &dpBoard[9][13], &dpBoard[9][10],
                        &dpBoard[9][7], &dpBoard[9][4], &dpBoard[8][4], &dpBoard[7][4], &dpBoard[7][7], &dpBoard[7][10],
                        &dpBoard[7][13], &dpBoard[7][16], &dpBoard[7][19], &dpBoard[7][22], &dpBoard[6][22],
                        &dpBoard[5][22], &dpBoard[4][22], &dpBoard[3][22],
                        &dpBoard[2][22], &dpBoard[1][22], &dpBoard[1][25]};

    /* Affichage des écuries */
    char *foo; // TODO: Need to free foo at each scat ?
    char bar[2] = {0};
    printf("Écuries :\n");
    foo = calloc(1, sizeof(char));
    for (int i = 0; i < nbPlayer; i++) {
        foo = scat(foo, playerColor[i]);
        foo = scat(foo, players[i].name);
        if (players[i].isNPC) {
            foo = scat(foo, " (NPC)");
        }
        foo = scat(foo, " : ");
        for (char j = 0; j < 4; j++) {
            if (!(players[i].horses[j]).isOut) {
                foo = scat(foo, "P");
                bar[0] = (players[i].horses[j]).horseID + 1 + '0';
                foo = scat(foo, bar);
                foo = scat(foo, " ");
            }
        }
        if (i != nbPlayer - 1) {
            foo = scat(foo, ANSI_COLOR_RESET);
            foo = scat(foo, "- ");
        }
    }
    foo = scat(foo, ANSI_COLOR_RESET);
    free(foo);
    printf("%s\n\n", foo);
    /* *********** */

    /* Affichage du plateau */
    printf("Plateau :\n");
    for (int i = 0; i < 56; i++) {
        if (board[i] != NULL) {
            *(ptrBoard[i] - 1) = 'P';
            *(ptrBoard[i]) = board[i]->horseID + 1 + '0';
        } else {
            *(ptrBoard[i]) = '#';
        }
    }

    for (int i = 0; i < 17; i++) {
        printf("%s", dpBoard[i]);
    }

}