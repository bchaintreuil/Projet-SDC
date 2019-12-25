//
// Created by Benjamin CHAINTREUIL on 23/12/2019.
//

#ifndef PROJET_SDC_JEU_H
#define PROJET_SDC_JEU_H

#include <stdio.h>

enum {
    NEW, LOAD
};

typedef struct Horse { ;
} Horse;

typedef struct Player {
    char name;
    char isIA;
    Horse *horses[4];
} Player;

void clrscr(void);

void getInput(char *input);

int dice(void);

void game(char mode);

void loadSave(void);

void saveGame(void);

void gc(void *arg1, ...);

char fexists(char *filename);

void delay(int seconds);

void *cmalloc(size_t size);

#endif //PROJET_SDC_JEU_H
