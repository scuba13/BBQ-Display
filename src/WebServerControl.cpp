#include "WebServerControl.h"
#include "TemperatureControl.h"

WebServerControl::WebServerControl(SystemStatus& systemStatus) 
  : _systemStatus(systemStatus) {}

void WebServerControl::begin() {
  _server.on("/", HTTP_GET, [this](AsyncWebServerRequest *request){
    serveWebPage(request);
  });

  _server.on("/setTemp", HTTP_POST, [this](AsyncWebServerRequest *request){
    if (request->hasParam("temp", true)) { 
      _systemStatus.bbqTemperature = request->getParam("temp", true)->value().toInt();
      Serial.println("Temperatura definida recebida: " + String(_systemStatus.bbqTemperature));
    }
    request->redirect("/");
  });

  _server.on("/getTemp", HTTP_GET, [this](AsyncWebServerRequest *request){
    String jsonResponse = "{ \"currentTemp\": " + String(_systemStatus.calibratedTemp) + ", \"setTemp\": " + String(_systemStatus.bbqTemperature) + " }";
    request->send(200, "application/json", jsonResponse);
  });

  _server.on("/getRelayState", HTTP_GET, [this](AsyncWebServerRequest *request){
    String jsonResponse = "{ \"relayState\": \"" + (_systemStatus.isRelayOn ? String("ON") : String("OFF")) + "\" }";
    request->send(200, "application/json", jsonResponse);
  });

  _server.on("/getAvgTemp", HTTP_GET, [this](AsyncWebServerRequest *request){
    String jsonResponse = "{ \"avgTemp\": " + String(_systemStatus.averageTemp) + " }";
    request->send(200, "application/json", jsonResponse);
  });

_server.on("/getProteinTemp", HTTP_GET, [this](AsyncWebServerRequest *request){
    if (_systemStatus.proteinTemperature > 0) {
        String jsonResponse = "{ \"proteinTemp\": " + String(_systemStatus.calibratedTempP) + " }";
        request->send(200, "application/json", jsonResponse);
    } else {
        request->send(200, "application/json", "{ \"proteinTemp\": 0 }");
    }
});


  _server.on("/resetSystem", HTTP_POST, [this](AsyncWebServerRequest *request){
    resetSystem(_systemStatus);
    Serial.println("Sistema resetado");
    request->redirect("/");
  });

  _server.on("/setProteinTemp", HTTP_POST, [this](AsyncWebServerRequest *request){ // Novo endpoint
        if (request->hasParam("proteinTemp", true)) { 
            _systemStatus.proteinTemperature = request->getParam("proteinTemp", true)->value().toInt();
            Serial.println("Protein temperature set to: " + String(_systemStatus.proteinTemperature));
        }
        request->redirect("/");
    });

  _server.begin();
}

void WebServerControl::serveWebPage(AsyncWebServerRequest *request) {
    String html = 
      "<!DOCTYPE html>"
      "<html>"
      "<head>"
      "<meta name='viewport' content='width=device-width, initial-scale=1'>"
      "<link rel='stylesheet' href='https://stackpath.bootstrapcdn.com/bootstrap/4.3.1/css/bootstrap.min.css'>"
      "<link href='https://fonts.googleapis.com/css2?family=Montserrat:wght@400;500&display=swap' rel='stylesheet'>"
      "<style>"
      "body {"
      "font-family: 'Montserrat', sans-serif;"
      "background-color: #000;"
      "color: #fff;"
      "padding-top: 20px;"
      "}"
      ".container { max-width: 100%; padding: 0 15px; }"
      "input[type='number'] {"
      "font-size: 18px;"
      "width: 100px;"
      "height: 40px;"
      "background-color: #fff;"
      "color: #000;"
      "border: 1px solid #aaa;"
      "display: inline-block;"
      "}"
      ".btn {"
      "margin-left: 15px;"
      "padding: 5px 15px;"
      "color: #000;"
      "display: inline-block;"
      "width: 200px;"
      "height: 40px;"
      "line-height: 30px;"
      "}"
      ".btn-secondary { background-color: #808080; }"
      ".btn-centered { display: block; margin: 20px auto; }"
      "table { width: 100%; }"
      "td { padding: 8px 0; }"
      ".degree-symbol {"
      "margin-right: 15px;"
      "display: inline-block;"
      "}"
      ".form-inline + .form-inline { margin-top: 15px; }"
      "</style>"
      "</head>"
      "<body>"
      "<div class='container'>"
      "<h2 class='text-center mb-4'>Artmill Web BBQ Monitor</h2>"
      "<table>"
      "<tr><td>BBQ Temperature:</td><td id='currentTemp'></td></tr>"
      "<tr><td>Set Temperature:</td><td id='setTemp'></td></tr>"
      "<tr><td>Fire:</td><td id='relayState'></td></tr>"
      "<tr><td>Average Temp 3hs:</td><td id='avgTemp'></td></tr>"
      "<tr><td>Protein Temperature:</td><td id='proteinTemp'></td></tr>"
      "</table>"
      "<form action='/setTemp' method='post' class='form-inline'>"
      "<input class='form-control mb-2' type='number' name='temp' min='30' max='200' placeholder='BBQ Temperature'>"
      "<span class='degree-symbol'>C</span>"
      "<button class='btn btn-secondary' type='submit'>Set BBQ</button>"
      "</form>"
      "<form action='/setProteinTemp' method='post' class='form-inline'>"
      "<input class='form-control mb-2' type='number' name='proteinTemp' min='30' max='200' placeholder='Protein Temperature'>"
      "<span class='degree-symbol'>C</span>"
      "<button class='btn btn-secondary' type='submit'>Set Protein</button>"
      "</form>"
      "<form action='/resetSystem' method='post'>"
      "<button class='btn btn-danger btn-centered' type='submit'>Reset BBQ</button>"
      "</form>"
      "</div>"
      "<script>"
      "function updateTempsAndRelayState() {"
      "fetch('/getTemp')"
      ".then(response => response.json())"
      ".then(data => {"
      "document.getElementById('currentTemp').textContent = data.currentTemp + ' C';"
      "document.getElementById('setTemp').textContent = data.setTemp + ' C';"
      "});"
      "fetch('/getRelayState')"
      ".then(response => response.json())"
      ".then(data => {"
      "document.getElementById('relayState').textContent = data.relayState === 'ON' ? 'ON' : 'OFF';"
      "});"
      "fetch('/getAvgTemp')"
      ".then(response => response.json())"
      ".then(data => {"
      "document.getElementById('avgTemp').textContent = data.avgTemp > 0 ? data.avgTemp + ' C' : '--';"
      "});"
      "fetch('/getProteinTemp')"
      ".then(response => response.json())"
      ".then(data => {"
      "document.getElementById('proteinTemp').textContent = data.proteinTemp > 0 ? data.proteinTemp + ' C' : '--';"
      "});"
      "}"
      "setInterval(updateTempsAndRelayState, 1000);"
      "updateTempsAndRelayState();"
      "</script>"
      "</body>"
      "</html>";

    request->send(200, "text/html", html.c_str());
}
