#include "BBQCure.h"
#include "MenuControl.h"
#include "TemperatureControl.h"
#include <Arduino.h>

//Definindo 4 etapas de cura
const CureStage cureStages[4] = {
    {75, 3600000},   // 75°C por 60 minutos
    {90, 2700000},  // 90°C por 45 minutos
    {105, 2700000},  // 105°C por 45 minutos
    {125, 7200000}  // 125°C por 2 horas
};





const int16_t COLOR_BACKGROUND = TFT_BLACK;
const int16_t COLOR_TEXT = TFT_WHITE;

static bool isCureProcessInitialized = false;   // Flag para verificar se o processo foi inicializado
static bool isCompletionScreenActive = false;   // Adicione esta linha para detectar quando a mensagem de conclusão está na tela

void bbqCureProcess(SystemStatus& sysStat, TFT_eSPI& tft, Bounce& encoderButton, RotaryEncoder& encoder) {
    if (!isCureProcessInitialized) {
        // Inicializar a tela do processo de cura
        initializeCureScreen(tft);
        
        // Código de inicialização
        sysStat.cureState.currentStage = 0;
        sysStat.cureState.stageStartTime = millis();
        sysStat.bbqTemperature = cureStages[sysStat.cureState.currentStage].temperature;
        isCureProcessInitialized = true;
    }

    if (millis() - sysStat.cureState.stageStartTime >= cureStages[sysStat.cureState.currentStage].duration) {
        sysStat.cureState.currentStage++;
        sysStat.cureState.stageStartTime = millis();
        
        if (sysStat.cureState.currentStage >= 4) {
            // Finaliza o processo
            sysStat.cureState.completed = true;
            tft.fillScreen(TFT_BLACK);

            int textYPosition = tft.height() / 2 - tft.fontHeight();
            tft.setCursor(10, textYPosition);
            tft.setTextColor(TFT_WHITE);
            tft.print("Cure Process");
            tft.setCursor(10, textYPosition + tft.fontHeight());
            tft.print("Completed");
            resetSystem(sysStat);
            
            isCompletionScreenActive = true;  // Defina esta variável como true quando a mensagem de conclusão estiver na tela
        } else {
            sysStat.bbqTemperature = cureStages[sysStat.cureState.currentStage].temperature;
        }
    }

    if (!isCompletionScreenActive && millis() - sysStat.lastTempUpdateCure >= 500) {  // Adicione a verificação para isCompletionScreenActive aqui
        int remainingSeconds = (cureStages[sysStat.cureState.currentStage].duration - (millis() - sysStat.cureState.stageStartTime)) / 1000;
        updateCureValues(sysStat, tft, cureStages[sysStat.cureState.currentStage].temperature, sysStat.cureState.currentStage, remainingSeconds);
        sysStat.lastTempUpdateCure = millis();
    }

    if (encoderButton.update() && encoderButton.fell()) {
        delay(200);
        sysStat.cureState.currentStage = -1;
        isCureProcessInitialized = false;
        sysStat.cureProcessMode = false;
        isCompletionScreenActive = false;  // Redefina para false ao sair do modo de conclusão
        resetSystem(sysStat);
        tft.print("Return Menu            ");
        delay(500);
        returnMenu(sysStat, encoder, encoderButton, tft);
    }
}



void drawCenteredTextCure(TFT_eSPI& tft, int x, int y, int width, const char* text) {
    int16_t textWidth = tft.textWidth(text);
    int16_t textHeight = tft.fontHeight();
    int16_t xOffset = (width - textWidth) / 2;
    tft.setCursor(x + xOffset, y);
    tft.print(text);
}

void initializeCureScreen(TFT_eSPI& tft) {
    tft.fillScreen(COLOR_BACKGROUND);
    tft.setTextSize(3);
    tft.setTextColor(COLOR_TEXT);

    tft.setCursor(10, 20);
    tft.print("Stage: ");

    tft.setCursor(10, 50);
    tft.print("Stg Temp: ");

    tft.setCursor(10, 80);
    tft.print("BBQ Temp: ");

    tft.setCursor(10, 110);
    tft.print("Rem Time: ");
}

void updateCureValues(SystemStatus& sysStat, TFT_eSPI& tft, int setTemperature, int currentStage, int remainingSeconds) {
    char buffer[30];
    tft.setTextSize(3);
    int rectWidth = tft.width() - 180;
    int rectHeight = tft.fontHeight() * 3; // Considerando o tamanho 3 da fonte

    // Atualizar etapa atual
    snprintf(buffer, sizeof(buffer), "%d", currentStage + 1);
    tft.fillRect(180, 20, rectWidth, rectHeight, COLOR_BACKGROUND);
    tft.setTextColor(COLOR_TEXT);
    drawCenteredTextCure(tft, 180, 20, rectWidth, buffer);

    // Atualizar temperatura setada na etapa
    snprintf(buffer, sizeof(buffer), "%dC", setTemperature);
    tft.fillRect(180, 50, rectWidth, rectHeight, COLOR_BACKGROUND);
    tft.setTextColor(COLOR_TEXT);
    drawCenteredTextCure(tft, 180, 50, rectWidth, buffer);

    // Atualizar temperatura atual
    snprintf(buffer, sizeof(buffer), "%dC", sysStat.calibratedTemp);
    tft.fillRect(180, 80, rectWidth, rectHeight, COLOR_BACKGROUND);
    tft.setTextColor(COLOR_TEXT);
    drawCenteredTextCure(tft, 180, 80, rectWidth, buffer);

    // Atualizar tempo restante
    int minutes = remainingSeconds / 60;
    int seconds = remainingSeconds % 60;
    snprintf(buffer, sizeof(buffer), "%dm %ds", minutes, seconds);
    tft.fillRect(180, 110, rectWidth, rectHeight, COLOR_BACKGROUND);
    tft.setTextColor(COLOR_TEXT);
    drawCenteredTextCure(tft, 180, 110, rectWidth, buffer);
}
