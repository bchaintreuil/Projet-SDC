//
// Created by Benjamin CHAINTREUIL on 23/12/2019.
//

#ifndef PROJET_SDC_JEU_H
#define PROJET_SDC_JEU_H

#include <stdio.h>

enum {
    NEW, LOAD
};

typedef struct Horse {
    char stair;
    char isOut;
    char pos;
} Horse;

typedef struct Player {
    char *name;
    char isIA;
    char hisTurn;
    Horse horses[4];
} Player;

void clrscr(void);

char *getInput(void);

int dice(void);

void game(char mode);

void loadSave(FILE *save, char *nbPlayers, Player **players, Horse *board[]);

void saveGame(FILE *save, char nbPlayers, Player *players);

void gc(void *arg1, ...);

char fexists(char *filename);

void delay(int seconds);

#endif //PROJET_SDC_JEU_H
