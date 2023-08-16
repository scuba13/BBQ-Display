#ifndef BBQ_CURE_H
#define BBQ_CURE_H

#include "SystemStatus.h"  // Assumindo que você tenha essa classe
#include <RotaryEncoder.h>
#include <Bounce2.h>
#include <TFT_eSPI.h>

struct CureStage {
    int temperature;
    unsigned long duration;
};

extern const CureStage cureStages[4];

void bbqCureProcess(SystemStatus& sysStat, TFT_eSPI& tft, Bounce& encoderButton, RotaryEncoder& encoder);
void initializeCureScreen(TFT_eSPI& tft);
void updateCureValues(SystemStatus& sysStat, TFT_eSPI& tft, int setTemperature, int currentStage, int remainingSeconds);


#endif
