#include <FS.h>                   //this needs to be first, or it all crashes and burns...
#include <ESP8266WiFi.h>          //https://github.com/esp8266/Arduino
#include <WiFiClient.h> 

//needed for library
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <ESP8266HTTPUpdateServer.h>
#include <WiFiManager.h>          //https://github.com/tzapu/WiFiManager
#include <ESP8266mDNS.h>
#include <ArduinoJson.h>          //https://github.com/bblanchon/ArduinoJson
#include <PubSubClient.h>
#include <Wire.h>
#include "Arduino.h"
#include "PCF8574.h"  
#include "uptime.h"
#include "i2cdetect.h"

#include "PumpControl.h"

MDNSResponder mdns;
ESP8266WebServer server(80);
ESP8266HTTPUpdateServer httpUpdater;
WiFiClient espClient;
PubSubClient client(espClient);
uptime* UpTime = NULL;
i2cdetect* I2Cdetect = NULL;
  
void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  Serial.println();

  //clean FS, for testing
  //SPIFFS.format();
  //SPIFFS.remove("/PinConfig.json");
  //SPIFFS.remove("/SensorConfig.json");
  //SPIFFS.remove("/VentilConfig.json");
  //SPIFFS.remove("/AutoConfig.json");
  
  CallWiFiManager();
  ReadConfigParam();

  client.setServer(mqtt_server, 1883);
  client.setCallback(MQTT_callback);
  
  if (!MDNS.begin("esp82660"))   {  Serial.println("Error setting up MDNS responder!");  }
  else                          {  Serial.println("mDNS responder started");  }

  server.onNotFound(handleNotFound);
  server.on("/", handleRoot);
  server.on("/PinConfig", handlePinConfig);
  server.on("/SensorConfig", handleSensorConfig);
  server.on("/VentilConfig", handleVentilConfig);
  server.on("/AutoConfig", handleAutoConfig);
  server.on("/Relations", handleRelations);
  
  server.on("/style.css", HTTP_GET, handleCSS);
  server.on("/javascript.js", HTTP_GET, handleJS);
  server.on("/parameter.js", HTTP_GET, handleJSParam);
  
  server.on("/StorePinConfig", HTTP_POST, handleStorePinConfig);
  server.on("/StoreSensorConfig", HTTP_POST, handleStoreSensorConfig);
  server.on("/StoreVentilConfig", HTTP_POST, handleStoreVentilConfig2);
  server.on("/StoreAutoConfig", HTTP_POST, handleStoreAutoConfig);
  server.on("/StoreRelations", HTTP_POST, handleStoreRelations);
  server.on("/reboot", HTTP_GET, handleReboot);

  // start a server
  httpUpdater.setup(&server);
  server.begin();
  Serial.println("Server started");
  
  //Wire.begin(pin_sda, pin_scl);
  I2Cdetect = new i2cdetect(pin_sda, pin_scl);
  //I2Cdetect->scan(); //Scan again if needed??

  if (measureType==HCSR04) {hcsr04_setup();}
  if (measureType==ANALOG) {analog_setup();}
  oled_setup();
  PCF8574_setup();
}

void loop() {
  // put your main code here, to run repeatedly:
  server.handleClient();
  if (!client.connected()) { 
      if (millis() - mqttreconnect_lasttry > 10000) {
        MQTT_reconnect(); 
        mqttreconnect_lasttry = millis();
    }
  }
  client.loop();
  UpTime->loop();
  
  if (millis() - previousMillis > measurecycle*1000) {
    previousMillis = millis();   // aktuelle Zeit abspeichern
    if (measureType==HCSR04) {hcsr04_loop();}
    if (measureType==ANALOG) {analog_loop();}
    oled_loop();
  }

  PCF8574_loop();
}


