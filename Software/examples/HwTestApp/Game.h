#include "SmartView.h"

void gameSetup();
void gameLoop();
bool gameRun();
void gameOver();
void drawScore(bool win);
void levelUp();
bool alienShot(int num);
bool onPlayer(int num);
bool exceedBoundary(int num);
void moveAliens();
int  findAlienX(int num);
int  findAlienY(int num);
int  findOldAlienX(int num);
int  findOldAlienY(int num);
void fireDaLazer();
void keepFirinDaLazer(int bulletNo);
void stopFirinDaLazer(int bulletNo);
bool moveShip();
void drawBitmap(char img[], int imgW, int imgH, int x, int y, int scale);
void up();
void down();
void left();
void right();
void select();
