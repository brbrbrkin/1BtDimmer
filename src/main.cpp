#include "mbed.h"

InterruptIn button(p7);
DigitalOut rLED(p5);
DigitalOut bLED(p6);
PwmOut intensity(p8);

Timeout TO;
Ticker tic;

enum STATE
{
  DOWN, UP,  MIN, MAX, FALL, RISE
};

int stateatu;
bool bstate;
float frequency;

void startDim();
void changeState();
void LEDalt();
void bPress();
void bRelease();



void startDim(){
    bLED = 1.0;
    rLED = 0.0;
    intensity = 1.0;
    stateatu = UP;
}

void readIntensity(float x)
{
    intensity = intensity + x;
    printf("%.2f\n",intensity.read());
}

void changeState()
{
    TO.attach(&LEDalt, 0.1);
    if(bstate)
    {
        if (stateatu == UP) stateatu = RISE;
        else if (stateatu == DOWN) stateatu = FALL;
        else if (stateatu == FALL)
        {
            if(intensity == 0.0) stateatu = MIN;
            else 
            {
                readIntensity(-0.05);
            }
        }
        else if (stateatu == RISE)
        {
            if(intensity == 1.0) stateatu = MAX;
            else
            {
                readIntensity(0.05);
            }
        }
        else
        {
            if (intensity >= 1.0) stateatu = MAX;
            else if (intensity <= 0.0) stateatu = MIN;
        }
    }
}



void LEDalt()
{
    switch(stateatu)
    {
        case FALL:
            frequency = 1;
            rLED = !rLED;
            break;
        case RISE:
            frequency = 1;
            bLED = !bLED;
            break;
        case MAX:
            frequency = 0.1;
            bLED = !bLED;
            break;
        case MIN:
            frequency = 0.1;
            rLED = !rLED;
            break;

        default:
            break;
    }
    TO.attach(&LEDalt, frequency);
}

void bPress()
{
    bstate = true;
    tic.attach(&changeState,1.0);
}

void bRelease()
{
    bstate = false;
    tic.detach();

    if(stateatu == RISE || stateatu == MIN || stateatu == DOWN)
    {
        if(intensity < 1.0)
        {
            stateatu = UP;
            bLED = 1.0;
            rLED = 0.0;
        }
    }
    
    else if(stateatu == FALL || stateatu == MAX || stateatu == UP)
    {
        if(intensity > 0.0)
        {
            stateatu = DOWN;
            bLED = 0.0;
            rLED = 1.0;
        }
    }
}



int main() 
{
    startDim();
    button.rise(&bPress);
    button.fall(&bRelease);
}