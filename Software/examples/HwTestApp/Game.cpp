//======================== intro =======================================
//      Space Shooter, basically knock-off Space Invaders
//             and also maybe a bit of Galaga
//   Written by Tyler Edwards for the Hackerbox #0020 badge
//  But should work on any ESP32 and Adafruit ILI9341 display
//        I am sorry for the messy code, you'll just
//                  have to live with it
//      Tyler on GitHub: https://github.com/HailTheBDFL/
//          Hackerboxes: http://hackerboxes.com/
//=========================== setup ===================================
// Space Shooter with M5STACK : 2018.01.12 Transplant by macsbug
// Controller   : Buttons A = LEFT, B = RIGHT, C = START or SHOOTING
// Github:https://macsbug.wordpress.com/2018/01/12/esp32-spaceshooter-with-m5stack/
//===================================================================

/*
 * 
 *  ported and debugged to TFT_eSPI 2023 Ralf Wolters
 *  license unclear, for demo use only
 *  original from:
 *  https://github.com/PartsandCircuits/M5Stack-SpaceShooter/blob/master/M5Stack-SpaceShooter.ino
 * 
 *  extended to use TFT_eSPI sprites
 * 
 *  found problem:
 *  in: bool alienShot(int num) in for loop: i was uninitialized and breaks. solution: (int i=0; i < 5; i++) ....
 */

#include "Game.h"


#define SPRITE_ENABLED    // sprite demo if enabled

#ifdef SPRITE_ENABLED     // sprites enabled
#define SCREEN spr
TFT_eSprite spr = TFT_eSprite(&tft);
#define COLOR_BLACK   0
#define COLOR_WHITE   1
#define COLOR_YELLOW  2
#define COLOR_BLUE    3
#define COLOR_RED     4
#define COLOR_GREEN   5
#define COLOR_MAGENTA 6
// 4-bit color map
uint16_t cmap[16] = {TFT_BLACK, TFT_WHITE, TFT_YELLOW, TFT_BLUE, TFT_RED, 0x5E85, TFT_MAGENTA, TFT_WHITE};

#else // normal TFT

#define SCREEN tft
#define COLOR_BLACK   TFT_BLACK
#define COLOR_WHITE   TFT_WHITE
#define COLOR_YELLOW  TFT_YELLOW
#define COLOR_BLUE    TFT_BLUE
#define COLOR_RED     TFT_RED
#define COLOR_GREEN   0x5E85
#define COLOR_MAGENTA TFT_MAGENTA
#endif



//============================= game variables =========================
unsigned long offsetM = 0;
unsigned long offsetT = 0;
unsigned long offsetF = 0;
unsigned long offsetB = 0;
unsigned long offsetA = 0;
unsigned long offsetAF = 0;
unsigned long offsetAB = 0;
unsigned long offsetS = 0;
int threshold = 40;
bool startPrinted = false;
bool beginGame = false;
bool beginGame2 = true;
bool play = false;
int score = 0;
int scoreInc = 10;
int level = 1;

//---------------------Player---------------------------------------

int shipX = 147;
int shipY = 190;
int oldShipX = 0;
int oldShipY = 0;
int changeShipX = 0;
int changeShipY = 0;
int shipSpeed = 50;
bool doSplode = false;
bool fire = false;
int fFireX[5] = {0, 0, 0, 0, 0};
int fFireY[5] = {0, 0, 0, 0, 0};
int fFireAge[5] = {0, 0, 0, 0, 0};

//--------------------------Aliens----------------------------------

bool alienLive[18] = {true};
int alienLiveCount = 18;
int alienX = 7;
int alienY = 25;
int oldAlienX = 7;
int oldAlienY = 25;
int changeAlienX = 6;
int changeAlienY = 0;
int alienSpeed = 200;
int oldAlienSpeed;
int aFireX[5] = {0};
int aFireY[5] = {0};
bool aFireAge[5] = {0};
int chanceOfFire = 2;

//================================ bitmaps ========================

//your starship
const int shipImgW = 14;
const int shipImgH = 16;
char shipImg[] = "ZZZZZZWWZZZZZZZZZZZYWWYZZZZZZZZZZWWWWZZZZZZZZZZ"
 "WWWWZZZZZZZZZWWWWWWZZZZZZZZWWWWWWZZZZZYZZWWWWWWZZYZZYZZWWWWWWZZ"
 "YZWWZZWWBBWWZZWWWWZZWBBBBWZZWWWWZWWBBBBWWZWWWWZWWWWWWWWZWWWWWWW"
 "WWWWWWWWWWRWWWWWWWWWWRWZZWWWWWWWWWWZZZZZWRRWWRRWZZZ";
//flames
const int flamesImgW = 12;
const int flamesImgH = 6;
char flamesImg[] = "RZZZZZZZZZZRRZRYYRRYYRZRRZRYYRRYYRZRZZRYRZZRY"
 "RZZZZZRZZZZRZZZZZZRZZZZRZZZ";
//alien
const int alienImgW = 14;
const int alienImgH = 11;
char alienImg[] = "GGGZZZZZZZZGGGZZZGZZZZZZGZZZZZGGGGGGGGGGZZZGGG"
 "GGGGGGGGGZGGGZGGGGGGZGGGGGGZZGGGGZZGGGGGGGGGGGGGGGGGGGGGGGGGGGG"
 "GGGGZZZGGZZGGZZZGZGGZGGZZGGZGGZZZZZZGZZGZZZZZ";
//ship 'sploded
const int splodedImgW = 14;
const int splodedImgH = 16;
char splodedImg[] = "ZZZZZZWWZZZZZZZZZZRYWWYRZZZYZZZRRWWRRRRZRWYZ"
 "RRRRRYYRRRZWYZRYRYYRYYRRRZWWRYYYRYYYYYRZWWRYYRYRYYYYRRWWRYYYRWR"
 "YBRRZRRRYRRWWWRYRWZZRYYRRBBWRYRWWZZRYYBBBRRYBWWRZZRYYYRRYYZZWZR"
 "RWRYYRBYRZZWZZRYBRYYYYYRRZZRWWYYYWWRRRZZZZWRRWWRRRWZZZ";
//=============================== setup and loop ==================

void gameSetup() 
{
  SCREEN.fillScreen(TFT_BLACK);
  SCREEN.setTextColor(COLOR_GREEN);
  SCREEN.setTextSize(4);

  // re-init all vars to restart
  offsetM = 0;
  offsetT = 0;
  offsetF = 0;
  offsetB = 0;
  offsetA = 0;
  offsetAF = 0;
  offsetAB = 0;
  offsetS = 0;
  threshold = 40;
  startPrinted = false;
  beginGame = false;
  beginGame2 = true;
  play = false;
  score = 0;
  scoreInc = 10;
  level = 1;

  shipX = 147;
  shipY = 190;
  oldShipX = 0;
  oldShipY = 0;
  changeShipX = 0;
  changeShipY = 0;
  shipSpeed = 50;
  doSplode = false;
  fire = false;
  memset(alienLive, true, 18);
  memset(fFireX, 0, 5);
  memset(fFireY, 0, 5);
  memset(fFireAge, 0, 5);
  alienLive[18] = {true};
  alienLiveCount = 18;
  alienX = 7;
  alienY = 25;
  oldAlienX = 7;
  oldAlienY = 25;
  changeAlienX = 6;
  changeAlienY = 0;
  alienSpeed = 200;
  oldAlienSpeed;
  aFireX[5] = {0};
  aFireY[5] = {0};
  aFireAge[5] = {0};
  chanceOfFire = 2;

  sViewSetFields(2, 1); // for touch left right

#ifdef SPRITE_ENABLED
  spr.setColorDepth(4);
  spr.createSprite(tft.width(), tft.height());
  spr.createPalette(cmap);  
  spr.fillSprite(COLOR_BLACK);
  spr.pushSprite(0, 0);
#endif

}

uint8_t keypress;
int16_t field;
/*
 *  loop emulation
 */
void gameLoop() {
  while (gameRun()) {};
  SCREEN.setTextSize(1);
  return;
}

/*
 *  in case of Sprite refresh
 */
void refreshSprite() {
#ifdef SPRITE_ENABLED
  spr.pushSprite(0, 0);
#endif
}

bool gameRun() {
  bool changed = false;
   
  field = sViewGetTouchField();   // get touch emulation for left/right
  if (field == 0) { left();   }
  if (field == 1) { right();  }
 
  keypress = sViewKey();  // get keys
  if(keypress == PHYSICAL_KEY1) { left();   }
  if(keypress == PHYSICAL_KEY2) { right();  }
  if(keypress == PHYSICAL_KEY3) { 
    select();
   qEdgeUiBuzzer(6); }
#ifdef ENABLE_SCREEN_SHOT_FCT 
  if(keypress == PHYSICAL_KEY4) { sViewScreenshot(); }  
#endif 
  //-------------Start Screen--------------
  if (millis() - offsetS >= 900 and !beginGame) {
    if (!startPrinted) {
      SCREEN.setCursor(77, 105);
      SCREEN.print(">START<");
      startPrinted = true;
      offsetS = millis();
      changed = true;
    }
    else {
      SCREEN.fillRect(77, 105, 244, 32, COLOR_BLACK);
      startPrinted = false;
      offsetS = millis();
      changed = true;
    }
  }
  if (beginGame and beginGame2) {
    SCREEN.fillRect(77, 105, 244, 32, COLOR_BLACK);
    beginGame2 = false;
    play = true;
    changed = true;
  }

  //-------------Player-----------------------------------------------
  if (millis() - offsetM >= shipSpeed and play) {
    if (moveShip()) changed = true;
    offsetM = millis();
    
  }
  if (oldShipX != shipX or oldShipY != shipY) {
    SCREEN.fillRect(oldShipX, oldShipY, 28, 44, COLOR_BLACK);
    oldShipX = shipX;
    oldShipY = shipY;
    drawBitmap(shipImg, shipImgW, shipImgH, shipX, shipY, 2);
    changed = true;
  }
  if (fire and play) { 
    fireDaLazer();
    changed = true;
    }
  if (millis() - offsetB >= 50) {
   for (int i = 0; i < 5; i++) {
    if (fFireAge[i] < 20 and fFireAge[i] > 0){
      keepFirinDaLazer(i);
      changed = true;
      }
    if (fFireAge[i] == 20) { 
      stopFirinDaLazer(i);
      changed = true;
      }
   }
   offsetB = millis();
  }  
  if (millis() - offsetT > 50) {
    changeShipX = 0;
    changeShipY = 0;
  }
  
  //---------------Aliens--------------------------------------------
  if (millis() - offsetA >= alienSpeed and play) {
    moveAliens(); offsetA = millis();
    changed = true;
  }
  if (findAlienX(5) >= 294){changeAlienX = -3;changeAlienY = 7;}
  if (alienX <= 6){changeAlienX = 3; changeAlienY = 7;}
  alienLiveCount = 0;
  for (int i = 0; i < 18; i++) {
    if (alienLive[i]) {
      alienLiveCount += 1;
      if (alienShot(i)) {
        SCREEN.fillRect(findOldAlienX(i),findOldAlienY(i),28,22,COLOR_BLACK);
        alienLiveCount -= 1;
        alienLive[i] = false;
        score += scoreInc;
        changed = true;
      }
      if (onPlayer(i) or exceedBoundary(i)) {
        gameOver();
        return(false);
      }
    }
  } 
  if (alienLiveCount == 1) {
    oldAlienSpeed = alienSpeed;
    if (alienSpeed > 50) {
      alienSpeed -= 10;
    } else {
      alienSpeed = 20;
    }
  }  
  if (alienLiveCount == 0) {
    levelUp();
  }
  if (changed) refreshSprite();
  return(true);
}

// functions =======================================================

void gameOver() {
  play = false;
  if (doSplode) {
    drawBitmap(splodedImg,splodedImgW,splodedImgH,shipX,shipY,2);
  }
  SCREEN.fillScreen(COLOR_BLACK);
  drawScore(false);
  refreshSprite();
  delay(1000);
  SCREEN.setCursor(17, 168);
  SCREEN.setTextSize(2);
  qEdgeUiBuzzer(5);
  delay(3000);
  return;
}

//==================================================================

void drawScore(bool win) {
  SCREEN.setCursor(53, 40);
  SCREEN.setTextColor(COLOR_WHITE);
  SCREEN.setTextSize(4);
  if (win) {
    SCREEN.print("LEVEL UP!");
    qEdgeUiBuzzer(5);
  }
  else {
    SCREEN.print("GAME OVER");
  }
  refreshSprite();
  for (;millis() - offsetM <= 1000;)
  SCREEN.setCursor(59, 89);
  SCREEN.setTextSize(3);
  SCREEN.print("Score: "); SCREEN.print(score);
  refreshSprite();
  offsetM = millis();
  for (;millis() - offsetM <= 1000;) {
  }
  SCREEN.setCursor(71, 128);
  SCREEN.print("Level: "); SCREEN.print(level);
  refreshSprite();
}

//==================================================================

void levelUp() {
  play = false;
  memset(alienLive, true, 18);
  memset(aFireX, 0, 5);
  memset(aFireY, 0, 5);
  memset(aFireAge, 0, 5);
  alienX = 7;
  alienY = 25;
  oldAlienX = 7;
  oldAlienY = 25;
  alienSpeed = oldAlienSpeed;
  if (alienSpeed > 100) {
    alienSpeed -= 10; chanceOfFire -= 10;
  }
  else if (alienSpeed > 50) {
    alienSpeed -= 10; chanceOfFire -=5;
  }
  else if (alienSpeed > 25) {
    alienSpeed -= 5; chanceOfFire -=1;
  }
  score += 50;     scoreInc += 5;
  changeShipX = 0; changeShipY = 0; 
  for (unsigned long i = millis(); millis() - i <= 1600;) {
    if (millis() - offsetM >= 20) {
      SCREEN.fillRect(oldShipX, oldShipY, 28, 44, COLOR_BLACK);
      drawBitmap(shipImg,shipImgW,shipImgH,shipX,shipY,2);
      drawBitmap(flamesImg,flamesImgW,flamesImgH,shipX + 1,
                 shipY + 32,2);
      oldShipX = shipX; oldShipY = shipY;
      shipY -= 6;
      offsetM = millis();
    }
  }
  drawScore(true);
  level += 1;
  shipX = 147;
  shipY = 190;
  for (; millis() - offsetM <= 4000;) {
  }
  SCREEN.fillScreen(COLOR_BLACK);
  offsetM = millis();
  play = true;
}

/*
 * 
 */
bool alienShot(int num) 
{
  for (int i=0; i < 5; i++) {
    if (fFireAge[i] < 20 and fFireAge[i] > 0) {
      if (fFireX[i] > (findAlienX(num) - 4) and fFireX[i] < (findAlienX(num) + 28) and fFireY[i] < (findAlienY(num) + 22) and fFireY[i] > (findAlienY(num) + 4)) {
        fFireAge[i] = 20;
        qEdgeUiBuzzer(3);
        return(true);
      }
    }
  }
  return(false);
}

bool onPlayer(int num) {
  if (findAlienX(num) - shipX < 24 and findAlienX(num) - 
      shipX > -28 and findAlienY(num) - shipY < 32 and 
      findAlienY(num) - shipY > -22) {
    doSplode = true;
    return true;
  } else { return false;}
}

bool exceedBoundary(int num) {
  if (findAlienY(num) > 218) { return true;
  } else { return false;
  }
}

void moveAliens() {
  for (int i = 0; i < 18; i++) {
    if (alienLive[i]) {
      SCREEN.fillRect(findOldAlienX(i),findOldAlienY(i),28,22,COLOR_BLACK);
      drawBitmap(alienImg,alienImgW,alienImgH,findAlienX(i),
               findAlienY(i),2);
    }
  }
  oldAlienX = alienX; oldAlienY = alienY;
  alienX += changeAlienX; alienY += changeAlienY;
  if (changeAlienY != 0) { changeAlienY = 0; }
}

int findAlienX   (int num) { return alienX + 42*(num % 6); }

int findAlienY   (int num) { return alienY + 33*(num / 6); }

int findOldAlienX(int num) { return oldAlienX + 42*(num % 6); }


int findOldAlienY(int num) { return oldAlienY + 33*(num / 6); }

//---------------------------Player---------------------------------

void fireDaLazer() {
  int bulletNo = -1;
  for (int i = 0; i < 4; i++) {
    if (fFireAge[i] == 0) { bulletNo = i;}
  }
  if (bulletNo != -1) {
   fFireAge[bulletNo] = 1;
   fFireX[bulletNo] = shipX + 13;
   fFireY[bulletNo] = shipY - 4;
   SCREEN.fillRect(fFireX[bulletNo],fFireY[bulletNo],4,3,COLOR_MAGENTA);
  }
  fire = false;
}

void keepFirinDaLazer(int bulletNo) {
  SCREEN.fillRect(fFireX[bulletNo],fFireY[bulletNo],4,4,COLOR_BLACK);
  fFireY[bulletNo] -= 8;
  SCREEN.fillRect(fFireX[bulletNo],fFireY[bulletNo],4,4,COLOR_MAGENTA);
  fFireAge[bulletNo] += 1;
}

void stopFirinDaLazer(int bulletNo) {
  SCREEN.fillRect(fFireX[bulletNo],fFireY[bulletNo],4,4,COLOR_BLACK);
  fFireAge[bulletNo] = 0;
}

/*
 *  returns true on movement
 */
bool moveShip() {
  if (shipX + changeShipX < 288 and shipX + changeShipX > 
      6 and changeShipX != 0){
    shipX += changeShipX;
  }
  if (shipY + changeShipY > 24 and shipY + changeShipY < 
      192 and changeShipY != 0){
    shipY += changeShipY;
  }
  if (oldShipX != shipX or oldShipY != shipY) {
    SCREEN.fillRect(oldShipX, oldShipY, 28, 44, COLOR_BLACK);
    oldShipX = shipX; oldShipY = shipY;
    drawBitmap(shipImg, shipImgW, shipImgH, shipX, shipY, 2);
    return(true); 
  }
  return(false);
}

void drawBitmap(char img[],int imgW,int imgH,int x,int y,int scale){
  uint16_t cellColor;
  char curPix;
  for (int i = 0; i < imgW*imgH; i++) {
    curPix = img[i];
    if (curPix == 'W') {      cellColor = COLOR_WHITE; }
    else if (curPix == 'Y') { cellColor = COLOR_YELLOW; }
    else if (curPix == 'B') { cellColor = COLOR_BLUE; }
    else if (curPix == 'R') { cellColor = COLOR_RED; }
    else if (curPix == 'G') { cellColor = COLOR_GREEN; }
    if (curPix != 'Z' and scale == 1) {
      SCREEN.drawPixel(x + i % imgW, y + i / imgW, cellColor);
    }
    else if (curPix != 'Z' and scale > 1) {
      SCREEN.fillRect(x + scale*(i%imgW),y + 
         scale*(i/imgW),scale,scale,cellColor);
    }
  }
}
//=========================== button functions =====================
void up() {
  if (millis() - offsetT >= 50 and play) {
    changeShipX = 0; changeShipY = -6; offsetT = millis();
  }
}
//==================================================================
void down() {
  if (millis() - offsetT >= 50 and play) {
    changeShipX = 0; changeShipY = 6; offsetT = millis();
  }
}
//==================================================================
void left() {
  if (millis() - offsetT >= 50 and play) {
    changeShipX = -6; changeShipY = 0; offsetT = millis();
  }
}
//==================================================================
void right() {
  if (millis() - offsetT >= 50 and play) {
    changeShipX = 6; changeShipY = 0; offsetT = millis();
  }
}
//==================================================================
void select() {
  if (millis() - offsetF >= 500 and play) {
    fire = true; offsetF = millis();
  }
  if (!beginGame) { beginGame = true;}
}
//==================================================================
