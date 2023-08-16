#include "MenuControl.h"
#include <Arduino.h>
#include <DisplayControl.h>
#include "BBQCure.h"

// Cores
#define COLOR_BACKGROUND TFT_BLACK
#define COLOR_TITLE TFT_DARKGREY
#define COLOR_HIGHLIGHT TFT_WHITE
#define COLOR_OPTION TFT_DARKGREY

const char* menu[] = { 
    "Monitor BBQ",
    "Set BBQ Temp ",
    "Calibrate BBQ Temp", 
    "Set Protein Temp",
    "BBQ Curing Process",
    "Information" 
};

const int menuLength = sizeof(menu) / sizeof(menu[0]);



void setNumericValue(int& value, int minValue, int maxValue, const char* message, SystemStatus& sysStat, RotaryEncoder& encoder, Bounce& encoderButton, TFT_eSPI& tft) {
    tft.fillScreen(TFT_BLACK);
    tft.setTextSize(3);
    
    // Centralizar o título no topo
    int16_t textWidth = tft.textWidth(message);
    int16_t textHeight = tft.fontHeight();
    int16_t titleX = (tft.width() - textWidth) / 2;
    tft.setCursor(titleX, 10);
    tft.setTextColor(TFT_DARKGREY);
    tft.print(message);

    // Aumentar a fonte para o valor
    tft.setTextSize(4);

    // Centralização do valor
    char tempBuffer[10];
    snprintf(tempBuffer, sizeof(tempBuffer), "%d", value);
    textWidth = tft.textWidth(tempBuffer);
    int16_t valueX = (tft.width() - textWidth) / 2;
    int16_t valueY = tft.height() / 2 - tft.fontHeight();
    tft.setTextColor(TFT_WHITE);
    tft.setCursor(valueX, valueY);
    tft.fillRect(valueX - 10, valueY - 10, textWidth + 20, tft.fontHeight() + 20, TFT_DARKGREY); // Destaque
    tft.print(tempBuffer);

    bool exitLoop = false;
    while (!exitLoop) {
        encoder.tick();
        int newPos = encoder.getPosition();

        if (newPos != sysStat.currentPos) {
            value += newPos - sysStat.currentPos;
            value = constrain(value, minValue, maxValue);

            // Atualizar o valor
            snprintf(tempBuffer, sizeof(tempBuffer), "%d", value);
            textWidth = tft.textWidth(tempBuffer);
            valueX = (tft.width() - textWidth) / 2;
            tft.fillRect(valueX - 10, valueY - 10, textWidth + 20, tft.fontHeight() + 20, TFT_DARKGREY); // Limpar e destacar
            tft.setCursor(valueX, valueY);
            tft.print(tempBuffer);

            sysStat.currentPos = newPos;
        }

        if (encoderButton.update() && encoderButton.fell()) {
            delay(200);
            exitLoop = true;
        }
        delay(10);
    }
    tft.setCursor(10, tft.height() - 30);
    tft.fillRect(10, tft.height() - 40, tft.width() - 20, 30, TFT_BLACK); // Limpar mensagem anterior
    tft.setTextColor(TFT_DARKGREY);
    tft.print("Value Set");
    returnMenu(sysStat, encoder, encoderButton, tft);
}


void displayMenu(SystemStatus& sysStat, RotaryEncoder& encoder, Bounce& encoderButton, TFT_eSPI& tft) {
    //Serial.println("Displaying Menu");
    encoder.tick();
    sysStat.currentPos = encoder.getPosition();

    if (sysStat.currentPos > menuLength) { encoder.setPosition(1); }
    if (sysStat.currentPos < 1) { encoder.setPosition(menuLength); }

    if (sysStat.lastPos != sysStat.currentPos) {
        tft.fillScreen(TFT_BLACK);
        tft.setTextSize(2);  // Ajustar o tamanho da fonte conforme preferência

        int lineHeight = tft.fontHeight() * 1.5;  // Ajustar conforme o tamanho da fonte
        int startY = 10; // Definir um ponto de partida Y (ajustado para 10)

        for (int i = 0; i < menuLength; i++) {
            int y = startY + i * lineHeight;

            // Destacar o item selecionado com a cor azul e os outros em branco
            if (i == sysStat.currentPos - 1) {
                tft.setTextColor(TFT_WHITE); // Altera a cor do item selecionado para azul
            } else {
                tft.setTextColor(TFT_DARKGREY);
            }

            tft.setCursor(30, y + 10); // Adiciona um padding
            tft.print(menu[i]);
        }

        sysStat.lastPos = sysStat.currentPos;
    }

    encoderButton.update();

    if (encoderButton.fell()) {
        tft.fillScreen(TFT_BLACK);
        delay(200);
        switch (sysStat.currentPos) {
            case 1:
                sysStat.monitorMode = true;
                break;
            case 2:
                setNumericValue(sysStat.bbqTemperature, sysStat.minBBQTemp, sysStat.maxBBQTemp, "Set BBQ Temp", sysStat, encoder, encoderButton, tft);
                break;
            case 3:
                setNumericValue(sysStat.tempCalibration, -20, 20, "Calibrate Temp", sysStat, encoder, encoderButton, tft);
                break;
            case 4:
               setNumericValue(sysStat.proteinTemperature, 40, 97, "Set Protein Temp", sysStat, encoder, encoderButton, tft);
                break;      
            case 5:
               sysStat.cureProcessMode = true;
                break;
            case 6: 
                displayInformation(sysStat, encoder, encoderButton, tft);

            break;
        }
    }
}

void returnMenu(SystemStatus& sysStat, RotaryEncoder& encoder,Bounce& encoderButton, TFT_eSPI& tft) {
    delay(500);
    tft.fillScreen(TFT_BLACK);
    sysStat.currentPos = 1;
    encoder.setPosition(sysStat.currentPos);
    sysStat.lastPos = -1;
}
