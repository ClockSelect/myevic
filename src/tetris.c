#include "myevic.h"
#include "dataflash.h"
#include "display.h"
#include "screens.h"
#include "events.h"
#include "miscs.h"
#include "timers.h"
#include "battery.h"
#include "tetris.h"

//used:
//http://colinfahey.com/tetris/tetris.html
//https://github.com/AJRussell/Tiny-Tetris
//flappy bird game from Ferox myevic

extern const uint8_t Blocks[7][2];

typedef struct {
    uint8_t curBlock[4][4]; //r c
    int Row;
    int Col;
    uint8_t nextBlock[4][4];
}
PieceSpace;
PieceSpace currentPiece = {0};
PieceSpace oldPiece = {0};

int tetrisScreen[14][24] = {
    {1},
    {1}
};

uint8_t ttTetrisLine = 14;
uint8_t ttAnimStep = 0;
//uint8_t ttAnimTimer = 0;
uint16_t ttScore;
int levellineCount = 0;
//uint8_t score = 0;
uint8_t ttTimeoutMask = 0;
uint8_t ttCurrentTimeout = 0;
uint8_t ttUsedTimeouts = 0;
uint8_t gameOver;
uint8_t countdown;
uint8_t level;
uint8_t CurPieceNumber;
uint8_t NextPieceNumber;
uint8_t nSZ = 3; //for rotate

uint8_t ttKeyDown = 0;
uint8_t ttFireDown = 0;
uint8_t ttLongFireDown = 0;
uint16_t ttKeyTick = 0;
//uint16_t ttKeyPressTime = 0;
uint8_t pause = 0;
uint8_t ScrFlip;
int bto;
uint8_t FastMove = 0; //for score
uint8_t noShift = 1;
uint8_t noRotate = 1;

//-------------------------------------------------------------------------

#define TT_NUM_TIMERS 3

typedef void (TT_TIMEOUT_FUNC(void));

typedef struct ttTimeout_s {
    uint8_t delay;
    TT_TIMEOUT_FUNC *function;
}
ttTimeout_t;
ttTimeout_t ttTimeoutsTable[TT_NUM_TIMERS];

void ttInitTimeouts() {
    ttCurrentTimeout = 0;
    ttUsedTimeouts = 0;

    for (int i = 0; i < TT_NUM_TIMERS; ++i) {
        ttTimeoutsTable[i].delay = 0;
        ttTimeoutsTable[i].function = 0;
    }
}

void ttTickTimeouts() {
    for (int i = 0; i < TT_NUM_TIMERS; ++i) {
        if (ttTimeoutsTable[i].delay) {
            --ttTimeoutsTable[i].delay;
        }
    }
}

void ttDeleteTimeout(int to) {
    ttUsedTimeouts &= ~(1 << to);
    ttTimeoutsTable[to].delay = 0;
}

void ttSetTimeoutDelay(int v) {
    ttTimeoutsTable[ttCurrentTimeout].delay = v;
}

void ttCallTimeouts() {
    ttTimeoutMask = 1;

    for (ttCurrentTimeout = 0; ttCurrentTimeout < TT_NUM_TIMERS; ++ttCurrentTimeout) {
        if (ttUsedTimeouts & ttTimeoutMask) {
            if (!ttTimeoutsTable[ttCurrentTimeout].delay) {
                ttTimeoutsTable[ttCurrentTimeout].function();
            }
        }
        ttTimeoutMask <<= 1;
    }
}

int ttCreateTimeout(TT_TIMEOUT_FUNC f) {
    uint8_t mask = 1;

    for (int i = 0; i < TT_NUM_TIMERS; ++i) {
        if (!(mask & ttUsedTimeouts)) {
            ttUsedTimeouts |= mask;
            ttTimeoutsTable[i].function = f;
            return i;
        }
        mask <<= 1;
    }
    return -1;
}

void ttCLSBuf() {
    MemClear2(0, ScreenBuffer, SCREEN_BUFFER_SIZE);
}

void LoadPiece(uint8_t BlockFlag, uint8_t pieceNumber, int col, int row) {

    uint8_t r = 0, c = 0;
    //    uint8_t pieceRow = 0;
    //    uint8_t pieceColoum = 0;

    for (uint8_t i = 0; i < 2; i++) {

        //iterate through bit mask
        for (uint8_t mask = 1; mask; mask <<= 1) {

            if (BlockFlag == 0) { //current

                if (mask & Blocks[pieceNumber][i]) {
                    //0b 0110 0000, 0b 0000 1100 Z
                    // c 3210 3210
                    // r   1   0        3     2
                    //    0123            
                    // 0  0000                
                    // 1  0110
                    // 2  0011                 
                    // 3  0000                     
                    currentPiece.curBlock[r][c] = 1;

                } else {
                    currentPiece.curBlock[r][c] = 0;

                }
            } else { //next

                if (mask & Blocks[pieceNumber][i]) {

                    currentPiece.nextBlock[r][c] = 1;

                } else {
                    currentPiece.nextBlock[r][c] = 0;

                }
            }

            c++; //c
            if (c >= 4) {
                c = 0;
                r++; //r
            }
        }
    }

    if (BlockFlag == 0) { //current
        currentPiece.Row = row;
        currentPiece.Col = col;
        CurPieceNumber = pieceNumber;

        //if (loadScreen == 1) {
        oldPiece = currentPiece;

        /*
                for (int r = row; r < row - 4; r--) {
                    for (int c = col; c < col + 4; c++) {

                        if (currentPiece.curBlock[pieceRow][pieceColoum]) {
                            tetrisScreen[c][r] = 2;
     
                             //   DrawValue(10, 50, c, 0, 1, 3);
                             //   DrawValue(34, 50, r, 0, 1, 3);
                        }            
                        pieceColoum++;
                    }
                    pieceColoum = 0;
                    pieceRow++;
                }
         */
        //}
    } else {
        NextPieceNumber = pieceNumber;
    }
}

void fillTetrisArray() {
    //+2 for border
    // x 0 1 (2 3 4 5 6 7 8 9 A B) C D
    // 
    for (uint8_t r = 0; r < 24; r++) {
        for (uint8_t c = 0; c < 14; c++) {
            tetrisScreen[c][r] = 1;
        }
    }
}

void fillTetrisScreen(uint8_t value) {

    for (uint8_t r = 2; r < 22; r++) {
        for (uint8_t c = 2; c < 12; c++) {
            tetrisScreen[c][r] = value;
        }
    }
}

int oledX(int x) {
    int ox;
    ox = (x - 2) * 6 + 2; //start x = 2
    return ox;
}

int oledY(int y) {
    int oy;
    oy = 122 - (y - 2) * 6; //121
    return oy;
}

void DrawPiece(int X, int Y, uint8_t draw) {
    // 0000
    // 01*0
    // 0011
    // 0000

    X = oledX(X);
    Y = oledY(Y);

    for (int r = 0; r < 4; r++) {
        for (int c = 0; c < 4; c++) {

            int x1 = c * 6 + X;
            int y1 = r * 6 + Y;
            int x2 = x1 + 4;
            int y2 = y1 + 4;

            if (!draw) {
                if (oldPiece.curBlock[r][c]) {
                    DrawFillRect(x1, y1, x2, y2, draw);
                }
            } else if (currentPiece.curBlock[r][c]) {
                DrawFillRect(x1, y1, x2, y2, draw);
            }
        }
    }
    DisplayRefresh();
}

void DrawNext(int X, int Y) {

    for (int r = 0; r < 3; r++) {
        for (int c = 0; c < 4; c++) {
            int x1 = c * 4 + X;
            int y1 = r * 4 + Y;
            int x2 = x1 + 2;
            int y2 = y1 + 2;
            DrawFillRect(x1, y1, x2, y2, currentPiece.nextBlock[r][c]);
        }
    }
    DisplayRefresh();
}

void DrawDot(int X, int Y, uint8_t dot) {
    X = oledX(X);
    Y = oledY(Y);
    DrawFillRect(X, Y, X + 4, Y + 4, dot);
}

void DrawBoard() {
    //21
    // .    0..3
    // .    .***
    // 4    ...*
    // 3    3...
    // 2 3 4 ... 11
    for (int r = 2; r < 22; r++) {
        for (int c = 2; c < 12; c++) {
            if (tetrisScreen[c][r] == 3) tetrisScreen[c][r] = 0;
            if (tetrisScreen[c][r] == 1) {
                //if (tetrisScreen[c][r]) {
                DrawDot(c, r, 1);
            } else
                DrawDot(c, r, 0);
        }
    }
}

void DrawTTCup() {
    //DrawHLine( const int x1, const int y, const int x2, const int color )
    DrawHLine(0, 127, 62, 1);
    //DrawVLine( const int x, const int y1, const int y2, const int color )
    DrawVLine(0, 7, 127, 1);
    DrawVLine(62, 7, 127, 1);
}

int CheckCollision() {

    uint8_t pieceRow = 0;
    uint8_t pieceCol = 0;
    int Col = currentPiece.Col;
    int Row = currentPiece.Row;

    for (int r = Row; r > Row - 4; r--) {
        for (int c = Col; c < Col + 4; c++) {

            if (currentPiece.curBlock[pieceRow][pieceCol]) {
                if (tetrisScreen[c][r] == 1) {
                    return 1;
                }
            }
            pieceCol++;
        }
        pieceCol = 0;
        pieceRow++;
    }
    return 0;
}

void RotatePiece() {

    uint8_t c;
    uint8_t r;
    uint8_t i;
    uint8_t Fig[4][4] = {0};

    if (CurPieceNumber == 0) {
        return; // O
    }

    oldPiece = currentPiece;

    if (CurPieceNumber == 1) {
        // I
        for (r = 0; r < 4; ++r) {
            for (c = 0; c < 4; ++c) {
                Fig[r][c] = currentPiece.curBlock[3 - c][3 - r];
            }
        }
        MemCpy(currentPiece.curBlock, Fig, 16);

    } else if (CurPieceNumber == 2 || CurPieceNumber == 3) {
        // S Z
        nSZ = nSZ == 1 ? 3 : 1;

        for (i = 0; i < nSZ; ++i) {

            for (r = 0; r < 4; ++r) {
                for (c = 1; c < 4; ++c) {
                    Fig[r][c] = currentPiece.curBlock[c - 1][3 - r];
                }
            }
            MemCpy(currentPiece.curBlock, Fig, 16);
        }
    } else { // L J T
        for (r = 0; r < 4; ++r) {
            for (c = 1; c < 4; ++c) {
                Fig[r][c] = currentPiece.curBlock[c - 1][3 - r];
            }
        }
        MemCpy(currentPiece.curBlock, Fig, 16);
    }

    if (CheckCollision()) {
        currentPiece = oldPiece;

    } else {
        DrawPiece(oldPiece.Col, oldPiece.Row, 0);
        DrawPiece(currentPiece.Col, currentPiece.Row, 1);
        //DisplayRefresh();
    }
}

void movePieceLeft() {

    oldPiece = currentPiece;
    currentPiece.Col = currentPiece.Col - 1;

    if (CheckCollision()) {
        currentPiece = oldPiece;
    } else {
        DrawPiece(oldPiece.Col, oldPiece.Row, 0);
        DrawPiece(currentPiece.Col, currentPiece.Row, 1);
        // DisplayRefresh();
    }
}

void movePieceRight() {
    oldPiece = currentPiece;
    currentPiece.Col = currentPiece.Col + 1;

    if (CheckCollision()) {
        currentPiece = oldPiece;
    } else {
        DrawPiece(oldPiece.Col, oldPiece.Row, 0);
        DrawPiece(currentPiece.Col, currentPiece.Row, 1);
        // DisplayRefresh();
    }
}

void setScore(uint16_t score, uint8_t show) {
    //DrawValue(x, y, value, dot_pos, font, num_digits);
    if (show) {
        DrawValue(34, 0, score, 0, 1, 5);
    } else {
        DrawFillRect(34, 0, 63, 6, 0);
    }
}

void setLevel(uint16_t lvl, uint8_t show) {
    //DrawValue(x, y, value, dot_pos, font, num_digits);
    if (show) {
        DrawValue(1, 0, lvl, 0, 1, 2);
    } else {
        DrawFillRect(1, 0, 12, 6, 0);
    }
}

void CompletedLines() {

    //int rowCheck = 0;
    //int coloumCheck = 0;
    uint8_t fullLine = 0;
    uint8_t noLine = 1;
    uint8_t linesProcessed = 0;
    uint8_t clearedLines = 0;
    uint8_t topRow = 0;
    int bottomRow = 0;
    //int currentRow = 0;
    uint16_t AmountScored = 0;

    if (currentPiece.Row > 5) bottomRow = currentPiece.Row - 4;
    else bottomRow = 1;

    if (currentPiece.Row == 22) topRow = 21;
    else topRow = currentPiece.Row;

    for (int rowCheck = topRow; rowCheck > bottomRow; rowCheck--) {

        fullLine = 1;
        for (int colCheck = 2; colCheck < 12; colCheck++) {
            if (tetrisScreen[colCheck][rowCheck] == 0) {
                fullLine = 0;
                break;
            }
        }

        if (fullLine) {
            //make line values 3's and render
            for (int c = 2; c < 12; c++) {
                tetrisScreen[c][rowCheck] = 3; //to del
            }
            //  bottomRow = rowCheck - 1; ?
            linesProcessed++;
            // animation 
        }
        DrawBoard();
    }

    if (linesProcessed) {

        clearedLines = linesProcessed;

        while (clearedLines) {
            for (int currentRow = 2; currentRow < 22; currentRow++) {
                noLine = 1;
                for (int c = 2; c < 12; c++) {
                    if (tetrisScreen[c][currentRow]) noLine = 0;
                }
                if (noLine) {
                    //move all lines down
                    for (int r = currentRow + 1; r < 22; r++) {
                        for (int c = 2; c < 12; c++) {
                            if (tetrisScreen[c][r]) tetrisScreen[c][r - 1] = 2;
                            else tetrisScreen[c][r - 1] = 3;
                        }
                    }
                }
            }

            //make the 2's 1's
            for (int r = 2; r < 22; r++) {
                for (int c = 2; c < 12; c++) {
                    if (tetrisScreen[c][r] == 2) tetrisScreen[c][r] = 1;
                }
            }
            clearedLines--;
            DrawBoard();
        }


        //process score
        switch (linesProcessed) {
            case 1:
                AmountScored = 10;
                break;
            case 2:
                AmountScored = 30;
                break;
            case 3:
                AmountScored = 70;
                break;
            case 4:
                AmountScored = 150;
                //do 4 line affect
                break;
        }

        fullLine = 1;
        for (int colCheck = 2; colCheck < 12; colCheck++) {
            if (tetrisScreen[colCheck][2]) {
                fullLine = 0;
                break;
            }
        }
        if (fullLine) AmountScored += 150; //empty board

        FastMove /= 10;
        AmountScored += FastMove; // fast drop count + noTouch + noRotate
        FastMove = 0;
        AmountScored *= level;

        ttScore += AmountScored;
        setScore(ttScore, 1);

        //update level line count & level
        levellineCount += linesProcessed;
        if (levellineCount > 10) {
            level++;
            levellineCount = 0;
            //do level up affect
        }
        if (dfTTSpeed != 2) {
            if (level > 10) gameOver = 1;
        }
        setLevel(level, 1);

    }

    //make the 2's 1's ?
    /*
          for (int r = bottomRow; r <= topRow; r++) {
            for (int c = 2; c < 12; c++) {
              if (tetrisScreen[c][r]) {
                tetrisScreen[c][r] = 1;
              }
            }
          }
     */
}

void DrawLandedPiece() {
    // Landed pieces are 1
    uint8_t pieceRow = 0;
    uint8_t pieceColoum = 0;

    int Col = currentPiece.Col;
    int Row = currentPiece.Row;

    for (int r = Row; r > Row - 4; r--) {
        for (int c = Col; c < Col + 4; c++) {

            if (currentPiece.curBlock[pieceRow][pieceColoum]) tetrisScreen[c][r] = 1;
            pieceColoum++;
        }
        pieceColoum = 0;
        pieceRow++;
    }

    FastMove += noRotate * 10;
    FastMove += noShift * 10;
    noRotate = 1;
    noShift = 1;

    CompletedLines();
}

void movePieceDown() {
    uint8_t pieceLanded = 0;

    oldPiece = currentPiece;
    currentPiece.Row = currentPiece.Row - 1;

    if (CheckCollision()) {
        currentPiece = oldPiece;

        DrawLandedPiece();
        pieceLanded = 1;

    } else {
        DrawPiece(oldPiece.Col, oldPiece.Row, 0);
        DrawPiece(currentPiece.Col, currentPiece.Row, 1);
        //DisplayRefresh();
    }

    if (pieceLanded) {
        MemCpy(currentPiece.curBlock, currentPiece.nextBlock, 16);
        currentPiece.Row = 22;
        currentPiece.Col = 5;
        CurPieceNumber = NextPieceNumber;
        DrawPiece(currentPiece.Col, currentPiece.Row, 1);
        //DisplayRefresh();

        if (CheckCollision()) {
            gameOver = 1;
        } else {
            //next piece
            LoadPiece(1, (uint8_t) Random() % 7, 0, 0);
            DrawNext(16, -4);
            // DisplayRefresh();
        }
    }
}

void CheckButtons() {


    if (gameOver) {
        ttDeleteTimeout(ttCurrentTimeout);
        return;
    }

    if (!ttKeyDown) {

        if (!PE0 && (!PD2 || !PD3)) {
            ttKeyDown = 0;
            return;
        }

        if (!PD2 && !PD3) {
            pause = pause == 0 ? 1 : 0;
            ttKeyDown = 0;
            ttSetTimeoutDelay(100);
            return;
        }

        if (pause) return;

        if (!PD2) {
            if (ScrFlip) movePieceLeft();
            else movePieceRight();
            noShift = 0;
            ttKeyDown = 1;
            ttFireDown = 0;
            ttSetTimeoutDelay(bto);
            return;

        } else if (!PD3) {
            if (ScrFlip) movePieceRight();
            else movePieceLeft();
            noShift = 0;
            ttKeyDown = 1;
            ttFireDown = 0;
            ttSetTimeoutDelay(bto);
            return;

        } else if (!PE0) {
            if ((!ttFireDown) && (!ttLongFireDown)) {
                ttFireDown = 1;
                ttSetTimeoutDelay(bto);
            }
            if (ttKeyTick > 5) {
                ttLongFireDown = 1;
                //long press
                movePieceDown();
                FastMove += 1;
                ttSetTimeoutDelay(bto);
            }
            ++ttKeyTick;
            ttKeyDown = 0;
            ttSetTimeoutDelay(bto);
            return;
        }
    }

    if (ttFireDown) {
        //fire up
        if (ttLongFireDown) {
            ttLongFireDown = 0;
        } else {
            // click
            RotatePiece();
            noRotate = 0;
            ttSetTimeoutDelay(bto);
        }
    }

    if (PD2 && PD3 && PE0) {
        ttKeyDown = 0;
        ttFireDown = 0;
        ttKeyTick = 0;
        ttLongFireDown = 0;
        ttSetTimeoutDelay(bto); //for no l-r shift after k_up
    }
}

void ttStartScreen() {

    const uint8_t strVaping[] = {0x7D, 0x68, 0x77, 0x70, 0x75, 0x6E, 0x00};
    const uint8_t strTetris[] = {0xAF, 0xA0, 0xAF, 0xAD, 0xA4, 0xAE, 0x00};
    const uint8_t strBestScore[] = {0x69, 0x6C, 0x7A, 0x7B, 0x00};
    const uint8_t strLastScore[] = {0x8D, 0x82, 0x94, 0x95, 0x00};

    ttSetTimeoutDelay(10);
    if (!PE0 && (PD2 && PD3)) {

        gameOver = 0;
        ttScore = 0;
        level = 1;
        pause = 0;
        FastMove = 0;
        noRotate = 1;
        noShift = 1;
        SetRandSeed(TMR1Counter);
        ttCLSBuf();

        fillTetrisArray(); //+2 border
        fillTetrisScreen(0); //2-11 x 2-21

        DrawTTCup();
        setLevel(level, 1);
        setScore(0, 1);

        //rnd 0-6
        nSZ = 3; //for rotate
        LoadPiece(0, (uint8_t) (Random() % 7), 5, 22);
        DrawPiece(currentPiece.Col, currentPiece.Row, 1);
        //CheckCollision();
        LoadPiece(1, (uint8_t) Random() % 7, 0, 0); //get next piece
        DrawNext(16, -4);
        //DisplayRefresh();

        //KeyUpTimer = 0;
        //ttKeyTick = 0;
        //ttFireDown = 0;
        //ttLongFireDown = 0;
        ttKeyDown = 1;

        ttDeleteTimeout(ttCurrentTimeout);

        ttSetTimeoutDelay(100);
        ttCreateTimeout(ttGame);
        ttCreateTimeout(CheckButtons);

    } else {

        DrawFillRect(0, 18, 63, 84, 0); //erase

        if (ttAnimStep) {
            if (++ttTetrisLine == 16)
                ttAnimStep = 0;
        } else {
            --ttTetrisLine;
            if (ttTetrisLine == 0)
                ttAnimStep = 1;
        }

        DrawString(strVaping, 12, ttTetrisLine + 20);
        DrawString(strTetris, 12, ttTetrisLine + 30);

        DrawString(strBestScore, 4, 60);
        DrawString(strLastScore, 4, 70);
        DrawValue(29, 61, dfTTBest, 0, 1, 5);
        DrawValue(29, 71, ttScore, 0, 1, 5);

        DrawTTCup();
        DisplayRefresh();
        ttSetTimeoutDelay(10);
        //ttCLSBuf();
    }
}

void ttGame() {

    if (pause) return;

    if (gameOver) {
        //ttCLSBuf();

        if (!PE0) return;

        if (dfTTBest < ttScore) {
            dfTTBest = ttScore;
            ttSetTimeoutDelay(4);
            UpdateDFTimer = 50;
        }

        ClearScreenBuffer();
        DisplayRefresh();
        ttDeleteTimeout(ttCurrentTimeout);
        ttCreateTimeout(ttStartScreen + 1);

    }

    if (dfTTSpeed != 2) countdown = 5 * (11 - level);
    ttSetTimeoutDelay(countdown);

    movePieceDown();
}

void ttStartGame() {

    UpdateDataFlash();
    if (dfTTSpeed <= 2) {

        if (dfTTBest < ttScore) {
            dfTTBest = ttScore;
            ttSetTimeoutDelay(4);
            UpdateDFTimer = 50; //survive
        }

        if (dfTTSpeed == 2) {
            bto = 5;
            countdown = 50;
        } else if (dfTTSpeed == 1) {
            bto = 5;
        } else {
            bto = 3;
        }

        ScrFlip = dfStatus.flipped;
        gFlags.playing_tt = 1;
        Screen = 0;
        gFlags.user_idle = 1; //0
        gFlags.refresh_display = 1;

        NoEventTimer = 0;
        SleepTimer = 3000;

        ttInitTimeouts();
        ClearScreenBuffer();
        DisplayRefresh();

        ttSetTimeoutDelay(20);
        ttCreateTimeout(ttStartScreen + 1);
    }
}