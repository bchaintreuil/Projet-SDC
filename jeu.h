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
    char steps;
} Horse;

typedef struct Player {
    char *name;
    char isIA;
    char hisTurn;
    Horse *horses;
} Player;

void clrscr(void);

char *getInput(void);

int dice(void);

void game(char mode);

void loadSave(void);

void saveGame(void);

void gc(void *arg1, ...);

char fexists(char *filename);

void delay(int seconds);

#endif //PROJET_SDC_JEU_H
