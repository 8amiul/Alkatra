#include "display.h"
#include "control.h"
#include "keyboard.h"

static const unsigned char image___2_bits[] = {0xff,0xff,0x01,0x80,0x01,0x80,0x01,0x80,0x01,0x80,0x81,0x81,0x81,0x81,0x01,0x80,0x01,0x80,0x01,0x80,0x01,0x80,0xff,0xff};
static const unsigned char image_backspace_bits[] = {0xff,0xff,0x7f,0x01,0x00,0x40,0x01,0x00,0x40,0x01,0x44,0x40,0x01,0x36,0x40,0x01,0x1f,0x40,0x01,0x06,0x40,0x01,0x04,0x40,0x01,0x00,0x40,0x01,0x00,0x40,0xff,0xff,0x7f};
static const unsigned char image_caps_bits[] = {0xff,0xff,0x0f,0x01,0x00,0x08,0x01,0x08,0x08,0x01,0x1c,0x08,0x01,0x3e,0x08,0x01,0x08,0x08,0x01,0x0c,0x08,0x01,0x04,0x08,0x01,0x02,0x08,0x01,0x00,0x08,0xff,0xff,0x0f};
static const unsigned char image_comma_bits[] = {0xff,0xff,0x0f,0x01,0x00,0x08,0x01,0x00,0x08,0x01,0x00,0x08,0x01,0x0c,0x08,0x01,0x0c,0x08,0x01,0x08,0x08,0x01,0x04,0x08,0x01,0x00,0x08,0x01,0x00,0x08,0x01,0x00,0x08,0xff,0xff,0x0f};
static const unsigned char image_enter_bits[] = {0xff,0xff,0xff,0x01,0x01,0x00,0x00,0x01,0x01,0x00,0x00,0x01,0x01,0x20,0x00,0x01,0x01,0x40,0x00,0x01,0x01,0xfe,0x00,0x01,0x01,0x40,0x00,0x01,0x01,0x20,0x00,0x01,0x01,0x00,0x00,0x01,0x01,0x00,0x00,0x01,0x01,0x00,0x00,0x01,0xff,0xff,0xff,0x01};
static const unsigned char image_numbers_bits[] = {0xff,0xff,0x0f,0x01,0x00,0x08,0x01,0x00,0x08,0x01,0x00,0x08,0x8d,0xc8,0x09,0xd1,0x14,0x09,0x89,0x90,0x08,0x81,0x08,0x09,0xc9,0xdd,0x08,0x01,0x00,0x08,0x01,0x00,0x08,0xff,0xff,0x0f};
static const unsigned char image_space_bits[] = {0xff,0xff,0xff,0xff,0xff,0x01,0x01,0x00,0x00,0x00,0x00,0x01,0x01,0x00,0x00,0x00,0x00,0x01,0x01,0x00,0x00,0x00,0x00,0x01,0x81,0xf7,0x78,0x8e,0x03,0x01,0x41,0x10,0x45,0x51,0x04,0x01,0x81,0x13,0x45,0xc1,0x07,0x01,0x01,0x14,0x65,0x41,0x00,0x01,0xc1,0xf3,0x58,0x9e,0x07,0x01,0x01,0x10,0x00,0x00,0x00,0x01,0x01,0x10,0x00,0x00,0x00,0x01,0xff,0xff,0xff,0xff,0xff,0x01};

const unsigned char image_PLEFT_bits[] = {0xff,0xff,0x0f,0x01,0x00,0x08,0x01,0x08,0x08,0x01,0x0c,0x08,0x01,0x0e,0x08,0x01,0x0f,0x08,0x01,0x0e,0x08,0x01,0x0c,0x08,0x01,0x08,0x08,0x01,0x00,0x08,0x01,0x00,0x08,0xff,0xff,0x0f};
const unsigned char image_PRIGHT_bits[] = {0xff,0xff,0x01,0x80,0x81,0x80,0x81,0x81,0x81,0x83,0x81,0x87,0x81,0x83,0x81,0x81,0x81,0x80,0x01,0x80,0x01,0x80,0xff,0xff};




typedef struct {
    char *keys;
    int len;

} KeyRow;

char row1[] = {'q','w','e','r','t','y','u','i','o','p'};
char row2[] = {'a','s','d','f','g','h','j','k','l'};
char row3[] = {SHIFT,'z','x','c','v','b','n','m',BACKSPACE};
char row4[] = {NUM_KEY, PLEFT, ' ', PRIGHT, ENTER};


char row1_NUM1[] = {'1', '2', '3', '4', '5', '6', '7', '8', '9', '0'};
char row2_NUM1[] = {'@', '#', '$', '~', '&', '-', '+', '(', ')'};
char row3_NUM1[] = {SHIFT, '*', '"', '\'', ':', ';', '!', '?', BACKSPACE};
char row4_NUM1[] = {NUM_KEY, PLEFT, ' ', PRIGHT, ENTER};



char row1_NUM2[] = {'_', '`', '|', '^', '=', '{', '}', '\\', '/', '%'};
char row2_NUM2[] = {'[', ']', '<', '>', ',', '.'};



KeyRow keyboard[4] = {
    {row1, 10},
    {row2, 9},
    {row3, 9},
    {row4, 5}
};

int row = 0, col = 0;
char keyPointer = keyboard[row].keys[col];

int selectX[4] = {4, 10, 10, 6};
int selectY[4] = {18, 29, 40, 51};
int dx[4] = {12, 12, 12, 21};
int select_x = 4;
int select_y = 18;

char keyboardBuffer[256];
char charBuff[2];

char* p = keyboardBuffer;
char* start_p = keyboardBuffer;
char* end_p = keyboardBuffer + 256;
int keyboardMode = ALPHA;

int pointer_x = 0;

void drawKeyboard(void) {
    u8g2.clearBuffer();

    // Pointer
    u8g2.setDrawColor(1);
    u8g2.setFont(u8g2_font_NokiaSmallPlain_tf);
    u8g2.drawBox(u8g2.getStrWidth(keyboardBuffer+pointer_x)+5, 5, 3, 6);

    if (strlen(keyboardBuffer))
        u8g2.drawStr(4, 11, keyboardBuffer);

    // select_hvr
    drawSelectHoverKeyboard();


    //if (strlen(keyboardBuffer))
    //    u8g2.drawBox(pointer_x, 5, 3, 6);
    //else
    //    u8g2.drawBox(5, 5, 3, 6);




    //u8g2.setFontMode(1);
    //u8g2.setBitmapMode(1);

    u8g2.setFont(u8g2_font_profont12_tr);
    switch (keyboardMode) {
        case ALPHA:
            // r1
            u8g2.drawStr(7, 26, "q w e r t y u i o p");

            // r2
            u8g2.drawStr(13, 37, "a s d f g h j k l");

            // r3
            u8g2.drawStr(24, 48, "z x c v b n m");
            break;
        case NUM1:
            // r1_p
            u8g2.drawStr(7, 26, "1 2 3 4 5 6 7 8 9 0");

            // r2_p
            u8g2.drawStr(13, 37, "@ # $ ~ & - + ( ) ");

            // r3_p
            u8g2.drawStr(24, 49, "* \" ' : ; ! ?");
            break;

        case NUM2:
            // r1_p2
            u8g2.drawStr(7, 26, "_ ` | ^ = { } \\ / %");

            // r2_p2
            u8g2.drawStr(13, 37, "[ ] < > , .");
            break;
    }

    // text_field
    u8g2.drawFrame(1, 0, 126, 16);

    // space
    u8g2.drawXBM(44, 52, 41, 12, image_space_bits);

    // caps
    u8g2.drawXBM(1, 40, 20, 11, image_caps_bits);

    // backspace
    u8g2.drawXBM(105, 40, 23, 11, image_backspace_bits);

    // PLEFT
    u8g2.drawXBM(22, 52, 20, 12, image_PLEFT_bits);

    // PRIGHT
    u8g2.drawXBM(86, 52, 16, 12, image_PRIGHT_bits);

    // enter
    u8g2.drawXBM(103, 52, 25, 12, image_enter_bits);

    // numbers
    u8g2.drawXBM(1, 52, 20, 12, image_numbers_bits);


    u8g2.sendBuffer();
}

void drawSelectHoverKeyboard(void) {
    u8g2.setDrawColor(2);
    if (row == 0)
        u8g2.drawBox(selectX[row]+(col * dx[row]), 18, 10, 11);
    if (row == 1)
        u8g2.drawBox(selectX[row]+(col * dx[row]), 29, 10, 11);
    if (row == 2) {
        switch (keyboard[row].keys[col]) {
            case SHIFT:
                u8g2.drawBox(2, 41, 18, 9);
                break;
            case BACKSPACE:
                u8g2.drawBox(106, 41, 21, 9);
                break;
            default:
                u8g2.drawBox(selectX[row]+(col * dx[row]), 40, 10, 11);
                break;
        }
    }
    if (row == 3) {
        switch (keyboard[row].keys[col]) {
            case NUM_KEY:
                u8g2.drawBox(2, 53, 18, 10);
                break;
            case PLEFT:
                u8g2.drawBox(23, 53, 18, 10);
                break;
            case ' ':
                u8g2.drawBox(45, 53, 39, 10);
                break;
            case PRIGHT:
                u8g2.drawBox(87, 53, 14, 10);
                break;
            case ENTER:
                u8g2.drawBox(104, 53, 23, 10);
                break;
        }
    }    
}

void Keyboard_BUTTON_LOGIC(struct Button_struct* Button) {
    if (Button->btn6 == LOW) {
        current_scr = prev_scr;
        p = &keyboardBuffer[0];
        memset(keyboardBuffer, 0, sizeof(keyboardBuffer));

    }
    if (Button->btn1 == LOW) {
        if (col > 0) {
            select_x -= dx[row];
            col--;
        }

    }
    if (Button->btn2 == LOW) {
        if (keyboard[row].len-1 > col) {
            select_x += dx[row];
            col++;
        }
    }
    if (Button->btn5 == LOW) {
        if (row > 0) {
            row--;
        }
    }
    if (Button->btn4 == LOW) {
        if (row < 3) {
            row++;
            if (col > keyboard[row].len-1)
                col = keyboard[row].len-1;
        }
    }
    if (Button->btn3 == LOW) {
        

        switch (keyboard[row].keys[col]) {
            case ENTER:
                p++;
                *p = '\0';
                Serial.println(keyboardBuffer);
                break;
            case BACKSPACE:
                if (strlen(keyboardBuffer) > 0) {
                    p--;
                    *p = '\0';
                    Serial.println(keyboardBuffer);
                }
                break;
            case NUM_KEY:
                keyboardMode++;
                if (keyboardMode >= 3)
                    keyboardMode = ALPHA;

                if (keyboardMode == ALPHA) {
                    keyboard[0] = {row1, 10}; keyboard[1] = {row2, 9}; keyboard[2] = {row3, 9}; keyboard[3] = {row4, 5};
                }
                else if (keyboardMode == NUM1) {
                    keyboard[0] = {row1_NUM1, 10}; keyboard[1] = {row2_NUM1, 9}; keyboard[2] = {row3_NUM1, 9}; keyboard[3] = {row4_NUM1, 5};                    
                }
                else if (keyboardMode == NUM2) {
                    keyboard[0] = {row1_NUM2, 10}; keyboard[1] = {row2_NUM2, 6}; keyboard[2] = {row3_NUM1, 9}; keyboard[3] = {row4_NUM1, 5};                      
                }
                break;

            case PLEFT:
                if (p > start_p) {
                    p--;
                    pointer_x++;
                }
                break;
            case PRIGHT:
                if (pointer_x > 0) {
                    p++;
                    pointer_x--;
                }
                break;
            default:
                *p = keyboard[row].keys[col];
                charBuff[0] = keyboard[row].keys[col]; charBuff[1] = '\0';
                p++;
        }
    }
    u8g2.setFont(u8g2_font_NokiaSmallPlain_tf);
    Serial.printf("row: %d | col: %d | char: %c | diff: %d | w: %d\n", row, col, keyboard[row].keys[col], (p-start_p), pointer_x);
}