#pragma once

#define SHIFT 16
#define ENTER 17
#define BACKSPACE 18
#define NUM_KEY 19
#define PLEFT 20
#define PRIGHT 21

enum KEYBOARD_TAB {ALPHA, NUM1, NUM2};

void Keyboard_BUTTON_LOGIC(struct Button_struct* Button);
void drawKeyboard(void);
void drawSelectHoverKeyboard(void);