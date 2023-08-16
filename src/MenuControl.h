#pragma once

#include "SystemStatus.h"
#include <RotaryEncoder.h>
#include <Bounce2.h>
#include <DisplayControl.h>

void setNumericValue(int& value, int minValue, int maxValue, const char* message, SystemStatus& sysStat, RotaryEncoder& encoder, Bounce& encoderButton, TFT_eSPI& tft);
void displayMenu(SystemStatus& sysStat, RotaryEncoder& encoder, Bounce& encoderButton, TFT_eSPI& tft);
void returnMenu(SystemStatus& sysStat, RotaryEncoder& encoder, Bounce& encoderButton, TFT_eSPI& tft);
