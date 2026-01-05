/*
Auteur : Alexis Létourneau
Date : 2026-01-05
Brief : Tetris sur une matrice de DEL de 9x16

Veuiller importer les 3 librairie avec platform.io
*/


#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_IS31FL3731.h>

#define nbRow 16 + 3
#define nbCol 9

#define LIGHT_LEVEL 3

#define EMPTY 0
#define SQUARE 1
#define MOVING_BLOCK 2
#define START_LAYER 15

#define BUTTON_LEF_PIN 23
#define BUTTON_MID_PIN 19
#define BUTTON_RIG_PIN 18

#define NB_OF_SIZE_1_BLOCK 1
#define NB_OF_SIZE_2_BLOCK 7
#define NB_OF_SIZE_3_BLOCK 6
#define NB_OF_SIZE_4_BLOCK 0

#define LINE_CLEAR_DELAY 100 //*3*3



// If you're using the full breakout...
Adafruit_IS31FL3731 ledmatrix = Adafruit_IS31FL3731();
// If you're using the FeatherWing version
//Adafruit_IS31FL3731_Wing ledmatrix = Adafruit_IS31FL3731_Wing();


uint8_t board[nbRow][nbCol] = {{}};

uint32_t speed = 250;
uint32_t startTime = 0;

uint8_t score = 0;

uint8_t xBlock = 0;
uint8_t yBlock = 0;

uint8_t sizeOfBlock = 0;

void affichage(){
  uint8_t light = 0;
  // animate over all the pixels, and set the brightness from the sweep table
  for (uint8_t x = 0; x < nbRow - 3; x++)
    for (uint8_t y = 0; y < nbCol; y++)
    {
      if (board[x][y] != 0)
        light = LIGHT_LEVEL;
      else
        light = 0;

      if (x >= 8)
        ledmatrix.drawPixel(-x + 23, -y + 8, light);
      else
        ledmatrix.drawPixel(x, y, light);
    }
}

void checkGameOver(){
  for (uint8_t y = 0; y < nbCol; y++)
    if (board[START_LAYER][y] == 1)
      for (uint8_t x = 0; x < nbRow; x++)
        for (uint8_t y = 0; y < nbCol; y++)
          board[x][y] = 0;
}

void stopBlock(){
    for (uint8_t x = 0; x < nbRow; x++)
      for (uint8_t y = 0; y < nbCol; y++)
        if (board[x][y] == MOVING_BLOCK)
          board[x][y] = SQUARE;
  }

bool checkMovingBlock(){
  int isBlock = false;
  for (uint8_t x = 0; x < nbRow; x++)
    for (uint8_t y = 0; y < nbCol; y++)
      if (board[x][y] == MOVING_BLOCK)
        isBlock = true;

  return isBlock;
}

void createBlock(){
  const uint8_t size_2_Blocks[NB_OF_SIZE_2_BLOCK][3][2] = {
      {   
        { 0, 0 }, //Square
        { 2, 2 },
        { 2, 2 } 
      },
      {   
        { 2, 0 }, //T 
        { 2, 2 },
        { 2, 0 } 
      },
      {   
        { 0, 2 }, //T mirror
        { 2, 2 },
        { 0, 2 } 
      },
      {   
        { 2, 0 }, //L
        { 2, 0 },
        { 2, 2 } 
      },
      {   
        { 0, 2 }, //L mirror
        { 0, 2 },
        { 2, 2 } 
      },
      {   
        { 0, 2 }, //S
        { 2, 2 },
        { 2, 0 } 
      },
      {   
        { 2, 0 }, //Z
        { 2, 2 },
        { 0, 2 } 

      }
    };

  const uint8_t size_3_Blocks[NB_OF_SIZE_3_BLOCK][2][3] = {
      {
        { 0, 2, 0 }, //T
        { 2, 2, 2 } 
      },
      {
        { 2, 2, 2 }, //T mirror
        { 0, 2, 0 } 
      },
      {
        { 0, 2, 2 }, //Z
        { 2, 2, 0 } 
      },
      {
        { 2, 2, 0 }, //S
        { 0, 2, 2 } 
      },
      {
        { 0, 0, 2 }, //L
        { 2, 2, 2 } 
      },
      {
        { 2, 0, 0 }, //L mirroir
        { 2, 2, 2 } 
      }
    };

  uint8_t typeOfBlock = random(1, NB_OF_SIZE_1_BLOCK + NB_OF_SIZE_2_BLOCK + NB_OF_SIZE_3_BLOCK + NB_OF_SIZE_4_BLOCK );
  uint8_t posYSpawn = 0;
  uint8_t randoBlock = 0;

  if (typeOfBlock == 1)
  {
    Serial.println("Spawn line ");
    posYSpawn = random(0, nbCol);
    for (uint8_t x = START_LAYER; x < nbRow; x++)
      board[x][posYSpawn] = MOVING_BLOCK;
    sizeOfBlock = 1;
  }
  else if(typeOfBlock > 1 && typeOfBlock < NB_OF_SIZE_1_BLOCK + NB_OF_SIZE_2_BLOCK)
  {
    sizeOfBlock = 2;
    Serial.println("Spawn size 2 blocks ");
    posYSpawn = random(0, nbCol - 1);
    randoBlock = random(0, NB_OF_SIZE_2_BLOCK);
    for (uint8_t x = START_LAYER; x < START_LAYER + 3; x++)
      for (uint8_t y = posYSpawn; y < sizeOfBlock + posYSpawn; y++)
        board[x][y] = size_2_Blocks[randoBlock][x - START_LAYER][y - posYSpawn];
  }
  else if(typeOfBlock > NB_OF_SIZE_1_BLOCK + NB_OF_SIZE_2_BLOCK && typeOfBlock < NB_OF_SIZE_1_BLOCK + NB_OF_SIZE_2_BLOCK + NB_OF_SIZE_3_BLOCK)
  {
    sizeOfBlock = 3;
    Serial.println("Spawn size 3 blocks ");
    posYSpawn = random(0, nbCol - 2);
    randoBlock = random(0, NB_OF_SIZE_3_BLOCK);
    for (uint8_t x = START_LAYER; x < START_LAYER + 2; x++)
      for (uint8_t y = posYSpawn; y < sizeOfBlock + posYSpawn; y++)
        board[x][y] = size_3_Blocks[randoBlock][x - START_LAYER][y - posYSpawn];

  }

    xBlock = START_LAYER;
    yBlock = posYSpawn;
}

void makeBlockDown(){
  for (uint8_t x = 0; x < nbRow; x++)
  {
    for (uint8_t y = 0; y < nbCol; y++)
    {
      if (board[x][y] == MOVING_BLOCK)
      {
        board[x-1][y] = MOVING_BLOCK; 
        board[x][y] = EMPTY;
      }
    }
  }
  xBlock -= 1;
}

bool checkBlockDown(){
  for (uint8_t x = 0; x < nbRow; x++)
  {
    for (uint8_t y = 0; y < nbCol; y++)
    {
      if (board[x][y] == MOVING_BLOCK)
      {
        if (x > 0)
        {
          if (board[x-1][y] == SQUARE)
            return false;
        }
        else
          return false;
      }
    }
  }
  return true;
}

bool checkBlockLorR(char dir){
  if (dir == 'L')
  {
    if (yBlock - 1 < 0)
    {
      Serial.println("Cant move, on the left edge");
      return false;
    }
    for(uint8_t x = 0; x < nbRow; x++)
      {
        if (board[x][yBlock - 1] == SQUARE && board[x][yBlock] == MOVING_BLOCK)
        {
          Serial.println("Cant move, block in the way");
          return false;
        }
      }
  }
  else if (dir == 'R')
  {
    if (yBlock + sizeOfBlock >= nbCol)
    {
      Serial.println("Cant move, on the right edge");
      return false;
    }
    for(uint8_t x = 0; x < nbRow; x++)
      {
        if (board[x][yBlock + sizeOfBlock] == SQUARE && board[x][yBlock] == MOVING_BLOCK)
        {
          Serial.println("Cant move, block in the way");
          return false;
        }
      }
  }
  return true;
}

void moveBlockLorR(char dir){

  if (dir == 'L')
  {
  for (uint8_t x = 0; x < nbRow; x++)
    for (uint8_t y = 0; y < nbCol; y++)
        if (board[x][y] == MOVING_BLOCK)
        {
          board[x][y-1] = MOVING_BLOCK;
          board[x][y] = EMPTY;          
        }
    yBlock -= 1;
  }
  else if (dir == 'R')
  {
  for (int x = nbRow; x > -1; x--)
    for (int y = nbCol; y > -1; y--)
        if (board[x][y] == MOVING_BLOCK)
        {
          board[x][y+1] = MOVING_BLOCK;
          board[x][y] = EMPTY;  
        }
    yBlock += 1;
  }
}

void deleteLine(){
  uint8_t nbSquare = 0;
  for (int x = nbRow; x > -1 ; x--)
  {
    nbSquare = 0;
    for (uint8_t y = 0; y < nbCol; y++)
      if (board[x][y] == SQUARE)
        nbSquare += 1;

    if (nbSquare == nbCol)
    {
      for (uint8_t i = 0; i < 3; i++)
      {
        for (uint8_t y2 = 0; y2 < nbCol; y2++)
          board[x][y2] = EMPTY;
        affichage();
        delay(LINE_CLEAR_DELAY);
        for (uint8_t y2 = 0; y2 < nbCol; y2++)
          board[x][y2] = SQUARE;
        affichage();
        delay(LINE_CLEAR_DELAY);
        for (uint8_t y2 = 0; y2 < nbCol; y2++)
          board[x][y2] = EMPTY;
        affichage();
        delay(LINE_CLEAR_DELAY);
      }
      score += 1; 
      for (uint8_t x2 = x + 1; x2 < nbRow; x2++)
        for (uint8_t y2 = 0; y2 < nbCol; y2++)
          board[x2 - 1][y2] = board[x2][y2];
    }
  }
}

void setup() 
{
  Serial.begin(9600);

  if (! ledmatrix.begin()) {
    Serial.println("IS31 not found");
    while (1);
  }
  Serial.println("IS31 found!");

  for (uint8_t x = 0; x < nbRow; x++)
    for (uint8_t y = 0; y < nbCol; y++)
      board[x][y] = 0;

  pinMode(BUTTON_LEF_PIN, INPUT);
  pinMode(BUTTON_MID_PIN, INPUT);
  pinMode(BUTTON_RIG_PIN, INPUT);
  
  startTime = millis();

}

void loop() 
{    
  affichage();

  deleteLine();

  if (!checkMovingBlock())
    createBlock();

  if (checkBlockDown())
    makeBlockDown();
  else
    stopBlock();

  checkGameOver();

  while((millis() - startTime) < speed)
  { 
    if (!digitalRead(BUTTON_LEF_PIN) && !digitalRead(BUTTON_MID_PIN) && !digitalRead(BUTTON_RIG_PIN))
    {
      for (uint8_t x = 0; x < nbRow; x++)
        for (uint8_t y = 0; y < nbCol; y++)
          board[x][y] = 0;
      delay(50);
    }
    else if (!digitalRead(BUTTON_LEF_PIN))
    {
      if(checkBlockLorR('L'))
      {      
        moveBlockLorR('L');
        affichage(); 
        delay(50);
      }
    }
    else if (!digitalRead(BUTTON_RIG_PIN))
    {
      if(checkBlockLorR('R'))
      {      
        moveBlockLorR('R');
        affichage(); 
        delay(50);
      }
    }
    else if (!digitalRead(BUTTON_MID_PIN))
    {
      delay(5);
      break;
    }
  }
  //Serial.print("Pos x (rows) : "); Serial.print(xBlock);  Serial.print(" Pos y (col) : "); Serial.println(yBlock);
  startTime = millis();
}


