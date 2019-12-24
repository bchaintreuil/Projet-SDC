//
// Created by Benjamin CHAINTREUIL on 23/12/2019.
//

#ifndef PROJET_SDC_JEU_H
#define PROJET_SDC_JEU_H

extern char input[32];
enum {
    NEW, LOAD
};

typedef struct Horse { ;
} Horse;

typedef struct Player {
    char id;
    char *name;
    char isIA;
    Horse *horses[4];
} Player;

typedef struct State {
    Player *players;
    char *nbPlayers;
} State;

void clrscr(void);

void getInput(char *input);

int dice(void);

void game(char mode);

State *initGame(void);

void loadSave(void);

void saveGame(void);

void gc(void);

char fexists(char *filename);

void delay(int seconds);

#endif //PROJET_SDC_JEU_H
