#include "display.h"
#include "control.h"
#include "game.h"
#include "snake.h"


int gameIconArray[3] = {-1, 0, 1};

Icon icons[] = {
    {image_Snake_bits, 21, 17},
    {image_BricBrek_bits, 22, 16},
    {image_TETR_bits, 16, 16},
    {image_airStrike_bits, 27, 14}
};


void drawCenteredIcon(int box_x, int box_y, int box_w, int box_h,
                      int icon_w, int icon_h,
                      const unsigned char* icon)
{
    int x = box_x + (box_w - icon_w) / 2;
    int y = box_y + (box_h - icon_h) / 2;

    u8g2.drawXBMP(x, y, icon_w, icon_h, icon);
}

void DRAW_GAME_MENU() {
    u8g2.clearBuffer();
    u8g2.setFontMode(1);
    u8g2.setBitmapMode(1);

    if (gameIconArray[0] >= 0) {
        // border-left
        u8g2.drawFrame(0, 20, 31, 25);

        drawCenteredIcon(1, 21, 29, 23, icons[gameIconArray[0]].w, icons[gameIconArray[0]].h, icons[gameIconArray[0]].bitmap);
        //u8g2.drawXBMP(4, 25, icons[gameIconArray[0]].w, icons[gameIconArray[0]].h, icons[gameIconArray[0]].bitmap);
    }

    // selected_hover
    u8g2.drawXBMP(40, 14, 46, 40, image_selected_hover_bits);
    drawCenteredIcon(44, 15, BOX_W, BOX_H, icons[gameIconArray[1]].w, icons[gameIconArray[1]].h, icons[gameIconArray[1]].bitmap);
    //u8g2.drawXBMP(51, 25, icons[gameIconArray[1]].w, icons[gameIconArray[1]].h, icons[gameIconArray[1]].bitmap);


    if (gameIconArray[2] < 4) {
        // border-right
        u8g2.drawFrame(97, 20, 31, 25);
        drawCenteredIcon(98, 21, 29, 23, icons[gameIconArray[2]].w, icons[gameIconArray[2]].h, icons[gameIconArray[2]].bitmap);
        //u8g2.drawXBMP(105, 24, icons[gameIconArray[2]].w, icons[gameIconArray[2]].h, icons[gameIconArray[2]].bitmap);

    }

    u8g2.sendBuffer();
}

void GAME_MENU_BUTTON_LOGIC(struct Button_struct* Button) {

    if (Button->btn2 == LOW) {
        if (gameIconArray[2] < TOTAL_GAMES) {
            for (int i = 0; i < 3; i++) {
                gameIconArray[i] += 1;
            }
        }
    }

    if (Button->btn1 == LOW) {
        if (gameIconArray[0] > -1) {
            for (int i = 0; i < 3; i++) {
                gameIconArray[i] -= 1;
            }
        }
    }

    if (Button->btn6 == LOW) {
        current_scr = MENU;
    }

    if (Button->btn3 == LOW) {
        current_scr = 30+gameIconArray[1];
    }
}