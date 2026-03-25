#include "graph.h"
#include "display.h"
#include "control.h"


float xmin = -100;
float xmax = 100;

float ymin = -100;
float ymax = 100;

// Zoom factor
float zoom = 0.22;

// Graph center
float xCenter = 0;
float yCenter = 0;

float f(float x)
{
    //return exp(-pow(x, cos(pow(x, 2)))) * sin(x);
    return exp(-pow(x, cos(log(x))));
}

/*
void drawEQnGEN() {
    u8g2.clearBuffer();
    int prevY = 0;


    for(int x = 0; x < 128; x++) {

        float realX = xmin + (x / 128.0) * (xmax - xmin);

        float y = f(realX);

        int screenY = 64 - ((y - ymin) / (ymax - ymin)) * 64;

        u8g2.drawPixel(x, screenY);
        u8g2.sendBuffer();
    }

    
    for(int x=0; x<128; x++){

        float realX = xmin + (x/128.0)*(xmax-xmin);

        float y = f(realX);

        int screenY = 64 - ((y-ymin)/(ymax-ymin))*64;

        if(x>0)
            u8g2.drawLine(x-1, prevY, x, screenY);
        u8g2.sendBuffer();
        prevY = screenY;
    }
    
}
*/

float mapf(float x, float in_min, float in_max, float out_min, float out_max) {
    float value = (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
    return round(value * 10.0) / 10.0;
}


void drawEQnGEN() {
    int new_pot_value = analogRead(POTENTIOMETER_PIN);
    
    if (abs(new_pot_value - last_zoom_pot_value) > 40) {
        last_zoom_pot_value = new_pot_value;

        float map_zoom = mapf(new_pot_value, 0, 4095, 0, 5);

        if (zoom != map_zoom) {
            zoom = constrain(map_zoom, 0.22, 5);
        }
    }

    u8g2.clearBuffer();

    int width = 128;
    int height = 64;
    /*
    float xmin = xCenter - 10 * zoom; float xmax = xCenter + 10 * zoom; float ymin = yCenter - 5 * zoom; float ymax = yCenter + 5 * zoom;
    */
    float xmin = xCenter - 20 * zoom;
    float xmax = xCenter + 20 * zoom;

    float ymin = yCenter - 10 * zoom;
    float ymax = yCenter + 10 * zoom;

    int prevY = 0;


    for(int x = 0; x < width; x++)
    {
        float realX = xmin + (x / (float)width) * (xmax - xmin);

        float y = f(realX);

        int screenY = height - ((y - ymin) / (ymax - ymin)) * height;


        //Serial.printf("x: %f, y: %d\n", realX, screenY);

        //if (isnan(y) || isinf(y)) {
        //    prevY = screenY;
        //    continue;
        //}


        if (screenY > 0 && prevY > 0 && screenY < 63 && prevY < 63) {        
            u8g2.drawLine(constrain(x-1, 0, 127), constrain(prevY, 0, 63), constrain(x, 0, 127), constrain(screenY, 0, 63));
            //Serial.printf("x-1: %d, prevY: %d, x: %d, screenY: %d\n");
        }

        prevY = screenY;

        //u8g2.sendBuffer();
    }
        Serial.printf("Zoom: %f, Read: %d\n", zoom, analogRead(POTENTIOMETER_PIN));

    u8g2.sendBuffer();
}

void drawEQnGen_BUTTON_LOGIC(struct Button_struct* Button) {
    if (Button->btn6 == LOW) {
        current_scr = MENU;
    }
    if (Button->btn1 == LOW) {
        xCenter -= 10;
    }
    if (Button->btn2 == LOW) {
        xCenter += 10;
    }
    if (Button->btn5 == LOW) {
        yCenter += 10;
    }
    if (Button->btn4 == LOW) {
        yCenter -= 10;
    }
}