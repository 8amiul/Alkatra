#include "display.h"
#include "snake.h"

int score = 0;

void drawSnakeGame() {
    u8g2.clearBuffer();
    DRAW_NAVBAR();
    u8g2.setFontMode(1);
    u8g2.setBitmapMode(1);

    // score-box
    u8g2.drawFrame(83, 10, 44, 26);

    // score_text
    u8g2.setFont(u8g2_font_6x10_tr);
    u8g2.drawStr(86, 20, "Score");

    // score
    char scoreBuff[64];
    u8g2.drawStr(86, 32, itoa(score, scoreBuff, 10));

    // banner
    u8g2.drawXBMP(83, 38, 42, 23, image_banner_bits);

    // snake_border
    u8g2.drawXBMP(0, 10, 82, 54, image_snake_border_bits);

    u8g2.sendBuffer();
}

