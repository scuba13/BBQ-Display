#include "BBQCure.h"
#include "MenuControl.h"
#include <Arduino.h>

// Definindo 4 etapas de cura
const CureStage cureStages[4] = {
    {75, 2700000},  // 75°C por 45 minutos
    {90, 2700000},  // 90°C por 45 minutos
    {105, 2700000},  // 105°C por 45 minutos
    {125, 7200000}  // 125°C por 2 horas
};

void bbqCureProcess(SystemStatus& sysStat, TFT_eSPI& tft, Bounce& encoderButton, RotaryEncoder& encoder) {
    static bool isCureProcessInitialized = false; // Flag para verificar se o processo foi inicializado

    if (!isCureProcessInitialized) {
        // Inicializar a tela do processo de cura
        initializeCureScreen(tft);
        
        // Coloque aqui o código de inicialização, como definir o primeiro estágio e outras coisas que você deseja fazer apenas uma vez no início do processo.
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
            tft.setCursor(10, tft.height() / 2);
            tft.setTextColor(TFT_WHITE);
            tft.print("Cure Process Completed");
            delay(5000);
            sysStat.cureState.currentStage = -1; // Reset para a próxima chamada
            isCureProcessInitialized = false;
            returnMenu(sysStat, encoder, encoderButton, tft);
        } else {
            sysStat.bbqTemperature = cureStages[sysStat.cureState.currentStage].temperature;
        }
    }

    if (millis() - sysStat.lastTempUpdateCure >= 1000) {  
        // Seu código de atualização da tela aqui, assim como você fez com monitor.
        int remainingSeconds = (cureStages[sysStat.cureState.currentStage].duration - (millis() - sysStat.cureState.stageStartTime)) / 1000;
        updateCureValues(sysStat, tft, cureStages[sysStat.cureState.currentStage].temperature, sysStat.cureState.currentStage, remainingSeconds);
        sysStat.lastTempUpdateCure = millis();
    }

    // Adicione aqui qualquer outro código que você quer que seja executado em cada passagem por bbqCureProcess, talvez verificação de botões, etc.
    if (encoderButton.update() && encoderButton.fell()) {
        // Se o botão for pressionado, finalize o processo e retorne ao menu
        isCureProcessInitialized = false;
        tft.setTextSize(2);
        tft.setTextColor(TFT_DARKGREY);
        tft.setCursor(10, 140);
        tft.print("Return Menu            ");
        delay(500);
        returnMenu(sysStat, encoder, encoderButton, tft);
    }
}

void initializeCureScreen(TFT_eSPI& tft) {
    tft.fillScreen(TFT_BLACK);
    tft.setTextSize(2); 
    tft.setTextColor(TFT_WHITE); 

    // Etapa atual
    tft.setCursor(10, 20);
    tft.print("Current Stage: ");

    // Temperatura da etapa
    tft.setCursor(10, 50);
    tft.print("Stage Temp: ");
    tft.print("C");

    // Temperatura atual
    tft.setCursor(10, 80);
    tft.print("Current Temp: ");
    tft.print("C");

    // Nova linha mostrando o valor da variável sysStat.bbqTemperature
    tft.setCursor(10, 110);
    tft.print("Variavel: ");
    tft.print("C");

    // Tempo restante
    tft.setCursor(10, 140);
    tft.print("Time Remaining: ");
}

void updateCureValues(SystemStatus& sysStat, TFT_eSPI& tft, int setTemperature, int currentStage, int remainingSeconds) {
    char buffer[30];
    
    // Atualizar etapa atual
    tft.fillRect(170, 20, 100, 30, TFT_BLACK);  // Limpa o texto anterior
    tft.setCursor(170, 20);
    snprintf(buffer, sizeof(buffer), "%d", currentStage + 1);
    tft.print(buffer);

    // Atualizar temperatura setada na etapa
    tft.fillRect(140, 50, 100, 30, TFT_BLACK);  // Limpa o texto anterior
    tft.setCursor(140, 50);
    snprintf(buffer, sizeof(buffer), "%dC", setTemperature);
    tft.print(buffer);

    // Atualizar temperatura atual
    tft.fillRect(170, 80, 100, 30, TFT_BLACK);  // Limpa o texto anterior
    tft.setCursor(170, 80);
    snprintf(buffer, sizeof(buffer), "%dC", sysStat.calibratedTemp);
    tft.print(buffer);

    // Atualizar valor da variável sysStat.bbqTemperature
    tft.fillRect(140, 110, 100, 30, TFT_BLACK);  // Limpa o texto anterior
    tft.setCursor(140, 110);
    snprintf(buffer, sizeof(buffer), "%dC", sysStat.bbqTemperature);
    tft.print(buffer);

    // Atualizar tempo restante
    tft.fillRect(230, 140, 100, 30, TFT_BLACK);  // Limpa o texto anterior
    tft.setCursor(230, 140);
    int minutes = remainingSeconds / 60;
    int seconds = remainingSeconds % 60;
    snprintf(buffer, sizeof(buffer), "%dmin %ds", minutes, seconds);
    tft.print(buffer);
}
