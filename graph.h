#pragma once
#include "tinyexpr.h"

extern double t;
extern te_variable vars[];
extern te_expr* expr;
extern int tinyexpr_error;

void drawEQnGEN();
void drawEQnGen_BUTTON_LOGIC(struct Button_struct* Button);