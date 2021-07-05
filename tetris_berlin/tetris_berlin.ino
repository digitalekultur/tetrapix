#include <NESpad.h>

#include <Adafruit_NeoPixel.h>

//#include <tetratetris.h>

//tetratetris nintendo = tetratetris(2,3,4);
#define NEOPIXELPIN 6
#define DISPLAYWIDTH 7
#define DISPLAYHEIGHT 15

#define SEGMENTWIDTH 8
#define SEGMENTHEIGHT 24

#define NUMBERBRICKS 7
#define BRICKSTART 2
#define PIXCNT DISPLAYHEIGHT*DISPLAYWIDTH
#define BRICKWIDTH 4
#define BRICKHEIGHT 2
#define TTSPEED 1000
#define L ((uint32_t)0 << 16) | ((uint32_t)0 <<  8) | 255
#define J ((uint32_t)0 << 16) | ((uint32_t)255 <<  8) | 0
#define S ((uint32_t)255 << 16) | ((uint32_t)0 <<  8) | 0
#define Z ((uint32_t)255 << 16) | ((uint32_t)255 <<  8) | 0
#define T ((uint32_t)0 << 16) | ((uint32_t)255 <<  8) | 255
#define I ((uint32_t)255 << 16) | ((uint32_t)255 <<  8) | 255
#define O ((uint32_t)255 << 16) | ((uint32_t)0 <<  8) | 255
uint32_t Matrix [DISPLAYHEIGHT][DISPLAYWIDTH] = { 0 };
uint32_t BRICK [NUMBERBRICKS][BRICKWIDTH][BRICKHEIGHT] =  {{{L,0},
                                                            {L,0}, 
                                                            {L,L},
                                                            {0,0}},
                                                            
                                                           {{0,J},
                                                            {0,J}, 
                                                            {J,J},
                                                            {0,0}},
                                                            
                                                           {{S,0},
                                                            {S,S}, 
                                                            {0,S},
                                                            {0,0}},
                                                            
                                                           {{0,Z},
                                                            {Z,Z}, 
                                                            {Z,0},
                                                            {0,0}},
                                                            
                                                           {{0,T},
                                                            {T,T}, 
                                                            {0,T},
                                                            {0,0}},
                                                            
                                                           {{I,0},
                                                            {I,0}, 
                                                            {I,0},
                                                            {I,0}},
                                                            
                                                           {{0,0},
                                                            {O,O}, 
                                                            {O,O},
                                                            {0,0}}
                                                            } ;
enum{check, draw, undraw};                                                            
enum{drawn, freeSpace, occupied};
enum{bottomDir ,rightDir, leftDir, rotateNegDir, rotatePosDir};
enum{brickRemains = 127};

Adafruit_NeoPixel strip = Adafruit_NeoPixel(PIXCNT, NEOPIXELPIN, NEO_GRB + NEO_KHZ800);
// put your own strobe/clock/data pin numbers here -- see the pinout in readme.txt
NESpad nintendo = NESpad(11,12,13);
//wh


void TTMatrixToPixel()
{
  Serial.print("MatrixToPixel\n");
  uint16_t actCol;
 
for(uint16_t  row = 0; row < SEGMENTHEIGHT; row++)
  {
     for(uint16_t  col = 0; col < SEGMENTWIDTH; col++)
     {
       // Odd rows run backwards
       if( row & 0x0001)
         actCol = (SEGMENTWIDTH - 1) - col;
       else
         actCol =  col;
       strip.setPixelColor(row * SEGMENTWIDTH + col, Matrix[SEGMENTHEIGHT - row - 1][SEGMENTWIDTH - actCol - 1]);
      }
    } 

   /* if(DISPLAYHEIGHT >= 2 * SEGMENTHEIGHT)
    {
       for(uint16_t  row = 0; row < SEGMENTWIDTH; row++)
        {
           for(uint16_t  col = 0; col < SEGMENTHEIGHT; col++)
           {
             // Odd rows run backwards
             if( row & 0x0001)
               actCol = (SEGMENTHEIGHT - 1) - col;
             else
               actCol =  col;
             strip.setPixelColor(row * SEGMENTHEIGHT + col + (DISPLAYWIDTH * 1 * SEGMENTHEIGHT), Matrix[row][actCol + 1 * SEGMENTHEIGHT]);
            }
          } 
      */
      //}
  return;
}

void matrixShow()
{
   TTMatrixToPixel();
   strip.show();  
}

void deleteRow(uint16_t rowToDel){
  for(uint16_t col = 0; col < DISPLAYWIDTH; col++){
    Matrix[rowToDel][col] = 0;
    matrixShow();
    delay(100);
    }

  for(uint16_t rowAbove = (rowToDel - 1); rowAbove > 0; rowAbove--){
    for(uint16_t col = 0; col < DISPLAYWIDTH; col++)
    {
      Matrix[rowAbove + 1][col] = Matrix[rowAbove][col];
    }
   // matrixShow();
    //delay(100);
    
  }
}


uint16_t doBrick(  uint16_t ranBrck, uint16_t flowRow, int8_t moveCol, uint16_t rotate,  uint16_t checkDrawUndraw )
//this function is able to check if there is space on the desired place in the DisplayMatrix
//and to draw and undraw a BRick on the desired Place
{
  int8_t matrixCol,matrixRow;
  for(uint16_t brickRow = 0; brickRow < BRICKHEIGHT;brickRow++){
    for(uint16_t brickCol = 0; brickCol < BRICKWIDTH;brickCol++)
    {
      //is there a stone in the Brickarray. also it is neccessary to draw or not
      if(BRICK[ranBrck][brickCol][brickRow] != 0){
           
          switch(rotate){ 
            case 0:
              //001
              //111
              //000
              matrixCol = BRICKSTART               + brickCol + moveCol; 
              matrixRow =              brickRow + flowRow;
              break;
            case 1:
              //010
              //010
              //011
              matrixCol = BRICKSTART + BRICKHEIGHT - brickRow + moveCol; 
              matrixRow =              brickCol + flowRow;
              break;
            case 2:
              //000
              //111
              //100
              matrixCol = BRICKSTART + BRICKHEIGHT - brickCol + moveCol; 
              matrixRow = BRICKHEIGHT - brickRow + flowRow;
              break;
            case 3:
              //110
              //010
              //010
              matrixCol = BRICKSTART               + brickRow + moveCol; 
              matrixRow = BRICKHEIGHT - brickCol + flowRow;
              break;
            default:
              break;
          }//switch rotate
          switch(checkDrawUndraw){
            case check:
              //check right and left edge
              if(matrixCol < 0)
                return occupied;
              if(matrixCol >= DISPLAYWIDTH)
                return occupied;
              //check if lowest row is reached
              if(matrixRow == DISPLAYHEIGHT)
                return occupied;
              //check if space is occupied
              if(Matrix[matrixRow][matrixCol] != 0)
                return occupied;
              break;
            case draw:
              Matrix[matrixRow][matrixCol] = BRICK[ranBrck][brickCol][brickRow];
              break;
            case undraw:
              Matrix[matrixRow][matrixCol] = 0;
              break;
           }//switch checkDrawUndraw
              

        
      }
    }
  }
  if(checkDrawUndraw == check)return freeSpace;
  return drawn;
}

int8_t drawBrick( uint16_t ranBrck, uint16_t flowRow, int8_t moveCol, uint16_t rotate, uint16_t moveDir){
  uint16_t brickState = doBrick(ranBrck, flowRow, moveCol, rotate, check);

  if(brickState == freeSpace){
      doBrick(ranBrck, flowRow, moveCol, rotate, draw);
      matrixShow();
      doBrick(ranBrck, flowRow, moveCol, rotate, undraw);
      return moveCol;
  }else{
    switch(moveDir){
    case rightDir:
      return (moveCol - 1);
    case leftDir:
      return (moveCol + 1);
    case rotatePosDir:
      return (rotate - 1);
    case rotateNegDir:
      return (rotate + 1);
    case bottomDir:
      if(flowRow > 0)
        doBrick(ranBrck, flowRow - 1, moveCol, rotate, draw);
      matrixShow();
      return brickRemains;
     default:
      break;
     //return drawn
    }  
  }
 }




void TTRowToMatrix(uint16_t row, uint16_t rowNumber){
  for(uint16_t i = 0; i < DISPLAYWIDTH; ){
    uint16_t bitmask = ((uint16_t)0x1 << i);
    if(row & bitmask)
      Matrix[i][rowNumber] = I;
    i++;}
  }


byte checkNesButton(byte* lastButton, byte buttonOfInterrest){
  //uint16_t nesButtons[8] = { NES_A, NES_B, NES_SELECT, NES_START, NES_UP, NES_DOWN, NES_LEFT, NES_RIGHT };

  byte currButton = nintendo.buttons();

    if ((*lastButton & buttonOfInterrest) && !(currButton & buttonOfInterrest)){
      *lastButton = 0;
       return 1;
    }

  //*lastButton = currButton;
  return 0;
}
   


void setup()
{
  Serial.begin(57600);  
  strip.begin();
  strip.show(); // Initialize all pixels to 'off'

}

void loop()
{
  /*
  uint32_t magenta = strip.Color(136,0, 10);
  for(uint16_t j=0;j< DISPLAYHEIGHT;j++){
    memset(Matrix, 0, sizeof Matrix);
  for(uint16_t i = 0; i < DISPLAYWIDTH; i++){
    Matrix[i][j] = magenta; 
  }
  matrixShow();

  delay(300);
  }*/
  
  
  //memset(Matrix, 0, sizeof Matrix);
  //TTRowToMatrix(0b0000000000011011,1);
  int8_t brickState = 0;
  uint16_t flow = 0;
  uint16_t lastFlow = 0;
  int8_t moveCol = 0;
  int8_t lastMoveCol = 0;
  uint16_t ranBrck = random(NUMBERBRICKS);
  uint32_t oldtime = 0;
  uint32_t newtime = 0;
    byte button = 0; 
 byte lastButton = 0;
 uint16_t downButtonPressed = 0;


//flow =2;
uint16_t rotate = 0;
/*rotate
 * do{
  memset(Matrix, 0, sizeof Matrix);
      TTCreateBrick(ranBrck, flow,moveCol, rotate, 0);
      //push to LEDs and show for delaytime
      TTMatrixShow();
      delay(1000);
      rotate++;
      if(rotate == 4)rotate = 0;
}while(1);
*/
Serial.print("Start:\n");
//Serial.println(Matrix[matrixCol][matrixRow], DEC);

//draw first Brick
drawBrick(ranBrck, flow, moveCol, rotate, bottomDir); 
//falling sloop
  do{  
      //buttoncycler
      //Check Buttons
      oldtime = millis();
      do{
       //check if user pressed something
       button = nintendo.buttons();
       downButtonPressed = 0;
     
       if(checkNesButton(&lastButton, NES_LEFT)){
        moveCol--;
        moveCol = drawBrick(ranBrck, flow, moveCol, rotate, leftDir); 
       }
        
       if(checkNesButton(&lastButton, NES_RIGHT)){
        moveCol++;
        moveCol = drawBrick(ranBrck, flow, moveCol, rotate, rightDir); 
       }

       if(checkNesButton(&lastButton, NES_A)){
        if(rotate == 3)
          rotate = 0;
        else
          rotate++;
        moveCol = drawBrick(ranBrck, flow, moveCol, rotate, rotatePosDir);
       } 

        if(checkNesButton(&lastButton, NES_B)){
        if(rotate == 0)
          rotate = 3;
        else
          rotate--;
        moveCol = drawBrick(ranBrck, flow, moveCol, rotate, rotateNegDir); 
        }

        if(checkNesButton(&lastButton, NES_DOWN)){
        downButtonPressed = 1;
       }
    
        lastButton = button;
              
       //zum entprellen
       delay(10);
       newtime=millis();
      }while(((newtime - oldtime) < TTSPEED) && !downButtonPressed);
      
      //lastFlow = flow; 
      flow++; 
      brickState = drawBrick(ranBrck, flow, moveCol, rotate, bottomDir); 
  }while(brickState != brickRemains);

  //TODO check full row
  
  for(uint16_t row=0; row < DISPLAYHEIGHT; row++){
    uint16_t col = 0;
    uint16_t stoneExist;
    do{
      if(Matrix[row][col] == 0) {
        stoneExist = 0; 
      } else {
        stoneExist = 1;
        if(col == (DISPLAYWIDTH - 1)){
          deleteRow(row);
          stoneExist = 0;
        }
      }
      col++;
    }while(stoneExist);
  }
  
  //Game Over if this final space is in the first two rows (flowrows)
  if(flow < 2)
     memset(Matrix, 0, sizeof Matrix);
}
