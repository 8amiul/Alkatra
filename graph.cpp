#include "graph.h"
#include "display.h"
#include "control.h"
#include "keyboard.h"
#include "tinyexpr.h"

float xmin = -100;
float xmax = 100;

float ymin = -100;
float ymax = 100;

// Zoom factor
float min_zoom = 0.22;
float max_zoom = 3.00;
float zoom = min_zoom;

// Graph center
float xCenter = 0;
float yCenter = 0;

int tinyexpr_error;
double t = 0;
te_variable vars[5] = {{"x", &t}};
//te_expr* expr = te_compile("sin(x)+cos(x^3)", vars, 1, &tinyexpr_error);
te_expr* expr = te_compile("sqrt(2*abs(x)) + sqrt(7-(x^2)) * sin(15*pi*x)", vars, 1, &tinyexpr_error);
float f(float x)
{
    //return exp(-pow(x, cos(pow(x, 2)))) * sin(x);
    //return exp(-pow(x, cos(log(x))));
    
    t = (double) x;
    double result = te_eval(expr);
    
    return result;
}


float mapf(float x, float in_min, float in_max, float out_min, float out_max) {
    float value = (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
    return round(value * 10.0) / 10.0;
}


static unsigned long lastTime = 0;
static const unsigned long interval = 500;

struct GraphPoints {
    int x1;
    int y1;
    int x2;
    int y2;
    int result;
} graphPoints[128];

int i = 0;
int basefq = 1200;


float phase = 0;
float sampleRate = 8000;
float frequency = 1600;

void audioLoop(int n) {

    phase += frequency / sampleRate;

    if (phase > 2 * PI)
        phase -= 2 * PI;

    float value = graphPoints[n].result;  

    int pwm = 128 + value * 100;

    ledcWrite(0, pwm);
}

void drawEQnGEN() {
    u8g2.setDrawColor(1);
    u8g2.clearBuffer();
    int new_pot_value = analogRead(POTENTIOMETER_PIN);
    
    if (abs(new_pot_value - last_zoom_pot_value) > 70) {
        last_zoom_pot_value = new_pot_value;

        float map_zoom = mapf(new_pot_value, 0, 4095, 0, max_zoom);

        if (zoom != map_zoom) {
            zoom = constrain(map_zoom, min_zoom, max_zoom);
        }
    }



    int width = 128;
    int height = 64;
    /*
    float xmin = xCenter - 10 * zoom; float xmax = xCenter + 10 * zoom; float ymin = yCenter - 5 * zoom; float ymax = yCenter + 5 * zoom;
    */
    float xmin = xCenter - 10 * zoom;
    float xmax = xCenter + 10 * zoom;

    float ymin = yCenter - 5 * zoom;
    float ymax = yCenter + 5 * zoom;

    int prevY = 0;
    
    if (Serial.available() > 0) {
        basefq = Serial.parseInt(); 
    }


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

        graphPoints[x].x1 = x-1;
        graphPoints[x].y1 = prevY;
        graphPoints[x].x2 = x;
        graphPoints[x].y2 = screenY;
        graphPoints[x].result = y;

        
        if (screenY > 0 && prevY > 0 && screenY < 63 && prevY < 63) {        
            u8g2.drawLine(constrain(x-1, 0, 127), constrain(prevY, 0, 63), constrain(x, 0, 127), constrain(screenY, 0, 63));
            //u8g2.sendBuffer();
            //Serial.printf("x-1: %d, prevY: %d, x: %d, screenY: %d\n");
        }
            

        prevY = screenY;

        //u8g2.sendBuffer();
    }
    
    if (i++ >= width) i = 0;

    if (graphPoints[i].y1 > 0 && graphPoints[i].y2> 0 && graphPoints[i].y1 < 63 && graphPoints[i].y2 < 63) {

        u8g2.setDrawColor(2);
        u8g2.drawLine(constrain(graphPoints[i].x1, 0, 127), constrain(graphPoints[i].y1, 0, 63), constrain(graphPoints[i].x2, 0, 127), constrain(graphPoints[i].y2, 0, 63));



        //float freq = basefq + graphPoints[i].result;
        //float val = pow(t, 2) - 1;
        //if (val < 0) val = 0; // Prevent NaN errors
        //float freq = 440 + sqrt(val);

    }


    Serial.printf("Zoom: %f, Read: %d\n", zoom, analogRead(POTENTIOMETER_PIN));

    u8g2.sendBuffer();
}

void drawEQnGen_BUTTON_LOGIC(struct Button_struct* Button) {
    if (Button->btn6 == LOW) {
        current_scr = MENU;
        ledcWriteTone(0, 0);
    }
    if (Button->btn1 == LOW) {
        xCenter -= 10;
    }
    if (Button->btn2 == LOW) {
        xCenter += 10;
    }
    if (Button->btn5 == LOW) {
        ledcWriteTone(0, 0);
        prev_scr = current_scr;
        current_scr = KEYBOARD;
    }
    if (Button->btn4 == LOW) {
        yCenter -= 10;
    }
}