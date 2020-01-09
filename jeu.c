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
    Horse *stairs[4][6] = {{NULL},
                           {NULL},
                           {NULL},
                           {NULL}};
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

void displayBoard(char nbPlayer, Player *players, Horse *board[], Horse *stairs[4][6]) {
    char dpBoard[17][52] = {
            {32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 65, 66, 32, 68, 66, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 10},
            {32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 0,  32, 32, 0,  32, 32, 0,  32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 10},
            {32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 0,  32, 32, 1,  32, 32, 0,  32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 10},
            {32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 0,  32, 32, 1,  32, 32, 0,  32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 10},
            {32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 0,  32, 32, 1,  32, 32, 0,  32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 10},
            {32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 0,  32, 32, 1,  32, 32, 0,  32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 10},
            {32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 0,  32, 32, 1,  32, 32, 0,  32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 10},
            {68, 74, 32, 32, 0,  32, 32, 0,  32, 32, 0,  32, 32, 0,  32, 32, 0,  32, 32, 0,  32, 32, 0,  32, 32, 1,  32, 32, 0,  32, 32, 0,  32, 32, 0,  32, 32, 0,  32, 32, 0,  32, 32, 0,  32, 32, 0,  32, 32, 32, 10},
            {65, 74, 32, 32, 0,  32, 32, 1,  32, 32, 1,  32, 32, 1,  32, 32, 1,  32, 32, 1,  32, 32, 1,  32, 32, 88, 32, 32, 1,  32, 32, 1,  32, 32, 1,  32, 32, 1,  32, 32, 1,  32, 32, 1,  32, 32, 0,  32, 65, 82, 10},
            {32, 32, 32, 32, 0,  32, 32, 0,  32, 32, 0,  32, 32, 0,  32, 32, 0,  32, 32, 0,  32, 32, 0,  32, 32, 1,  32, 32, 0,  32, 32, 0,  32, 32, 0,  32, 32, 0,  32, 32, 0,  32, 32, 0,  32, 32, 0,  32, 68, 82, 10},
            {32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 0,  32, 32, 1,  32, 32, 0,  32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 10},
            {32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 0,  32, 32, 1,  32, 32, 0,  32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 10},
            {32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 0,  32, 32, 1,  32, 32, 0,  32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 10},
            {32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 0,  32, 32, 1,  32, 32, 0,  32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 10},
            {32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 0,  32, 32, 1,  32, 32, 0,  32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 10},
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
    char *ptrStairs[4][6] = {
            {&dpBoard[2][25],  &dpBoard[3][25],  &dpBoard[4][25],  &dpBoard[5][25],  &dpBoard[6][25],  &dpBoard[7][25]},
            {&dpBoard[8][43],  &dpBoard[8][40],  &dpBoard[8][37],  &dpBoard[8][34],  &dpBoard[8][31],  &dpBoard[8][28]},
            {&dpBoard[14][25], &dpBoard[13][25], &dpBoard[12][25], &dpBoard[11][25], &dpBoard[10][25], &dpBoard[9][25]},
            {&dpBoard[8][7],   &dpBoard[8][10],  &dpBoard[8][13],  &dpBoard[8][16],  &dpBoard[8][19],  &dpBoard[8][22]}
    };

    char *currentLine; // TODO: Need to free currentLine at each scat ?
    char charBuffer[2] = {0};
    char found = 0;
    char color = 0;

    /* Affichage des écuries */
    printf("Écuries :\n");
    currentLine = calloc(1, sizeof(char));
    for (int i = 0; i < nbPlayer; i++) {
        currentLine = scat(currentLine, playerColor[i]);
        currentLine = scat(currentLine, players[i].name);
        if (players[i].isNPC) {
            currentLine = scat(currentLine, " (NPC)");
        }
        currentLine = scat(currentLine, " : ");
        for (char j = 0; j < 4; j++) {
            if (!(players[i].horses[j]).isOut) {
                currentLine = scat(currentLine, "P");
                charBuffer[0] = (players[i].horses[j]).horseID + 1 + '0';
                currentLine = scat(currentLine, charBuffer);
                currentLine = scat(currentLine, " ");
            }
        }
        if (i != nbPlayer - 1) {
            currentLine = scat(currentLine, ANSI_COLOR_RESET);
            currentLine = scat(currentLine, "- ");
        }
    }
    currentLine = scat(currentLine, ANSI_COLOR_RESET);
    free(currentLine);
    printf("%s\n\n", currentLine);
    /* *********** */

    /* Affichage du plateau */
    printf("Plateau :\n");
    for (int i = 0; i < 17; i++) {
        currentLine = calloc(1, sizeof(char));
        for (int j = 0; j < 52; j++) {
            charBuffer[0] = dpBoard[i][j];
            switch (charBuffer[0]) {
                case '\n':
                    printf("%s\n", currentLine);
                    break;
                case 0:
                    for (int k = 0; k < 56; k++) {
                        if (&dpBoard[i][j] == ptrBoard[k] && board[k] != NULL) {
                            currentLine = scat(currentLine, playerColor[board[k]->playerID]);
                            currentLine = scat(currentLine, "P");
                            charBuffer[0] = board[k]->horseID + 1 + '0';
                            currentLine = scat(currentLine, charBuffer);
                            currentLine = scat(currentLine, ANSI_COLOR_RESET);
                            found++;
                            break;
                        }
                    }
                    if (!found) {
                        for (int k = 0; k < 56; k++) {
                            if (&dpBoard[i][j] == ptrBoard[k]) {
                                color = (k + 1) / 14;
                                if (color == 4) {
                                    color = 0;
                                }
                                currentLine = scat(currentLine, playerColor[color]);
                                currentLine = scat(currentLine, "#");
                                currentLine = scat(currentLine, ANSI_COLOR_RESET);
                                break;
                            }
                        }
                    } else {
                        found = 0;
                        j++;
                    }
                    break;
                case 1:
                    for (int k = 0; k < 4; k++) {
                        for (int l = 0; l < 6; l++) {
                            if (&dpBoard[i][j] == ptrStairs[k][l] && stairs[k][l] != NULL) {
                                currentLine = scat(currentLine, playerColor[k]);
                                currentLine = scat(currentLine, "P");
                                charBuffer[0] = stairs[k][l]->horseID + 1 + '0';
                                currentLine = scat(currentLine, charBuffer);
                                currentLine = scat(currentLine, ANSI_COLOR_RESET);
                                found++;
                                break;
                            }
                        }
                        if (found) {
                            break;
                        }
                    }
                    if (!found) {
                        for (int k = 0; k < 4; k++) {
                            for (int l = 0; l < 6; l++) {
                                if (&dpBoard[i][j] == ptrStairs[k][l]) {
                                    currentLine = scat(currentLine, playerColor[k]);
                                    charBuffer[0] = l + 1 + '0';
                                    currentLine = scat(currentLine, charBuffer);
                                    currentLine = scat(currentLine, ANSI_COLOR_RESET);
                                    found++;
                                    break;
                                }
                            }
                            if (found) {
                                found = 0;
                                break;
                            }
                        }
                    } else {
                        found = 0;
                        j++;
                    }
                    break;
                default:
                    currentLine = scat(currentLine, charBuffer);
                    break;
            }
        }
        free(currentLine);
    }
}