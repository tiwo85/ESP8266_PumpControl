String getJSParam() {
  char buffer[100] = {0};
  memset(buffer, 0, sizeof(buffer));
  
  // http://jsfiddle.net/mwPb5/
  // http://jsfiddle.net/mwPb5/1058/
  
  html_str = "";
  // bereits belegte Ports, können nicht ausgewählt werden (zb.vi2c-ports)
  // const gpio_disabled = Array(0,4);
  sprintf(buffer, "const gpio_disabled = [%d,%d,%d,%d];\n", pin_sda, pin_scl, pin_hcsr04_trigger, pin_hcsr04_echo);
  html_str += buffer;

  // anhand gefundener pcf Devices die verfügbaren Ports (jeweils Port0)
  //const pcf_found = [65];
  html_str += "const pcf_found = [";
  uint8_t count=0;
  for (uint8_t i=0; i<8; i++) {
    if (i2c_adresses[i] > 0 && i2c_adresses[i] >= 56 && i2c_adresses[i] <= 63) {
      sprintf(buffer, "%s%d", (count>0?",":"") ,((i2c_adresses[i]-55)*8)+57);
      html_str += buffer;
      count++;    
    }
  }
  html_str += "];\n";

  //konfigurierte Ports / Namen
  //const configuredPorts = [ {port:65, name:"Ventil1"}, {port:67, name:"Ventil2"}]
  html_str += "const configuredPorts = [";
  for(int i=0; i < pcf8574devCount; i++) {
    sprintf(buffer, "{port:%d, name:'%s'}%s", pcf8574dev[i].port ,pcf8574dev[i].subtopic, (i<pcf8574devCount-1?",":""));
    html_str += buffer;
  }
  html_str += "];\n";
  
  return html_str.c_str();
}
  
String getPageHeader(int pageactive) {
  char buffer[100] = {0};
  memset(buffer, 0, sizeof(buffer));
  
  html_str  = "<!DOCTYPE html><html><head><meta name='viewport' content='width=device-width, initial-scale=1.0'/>\n";
  html_str += "<meta charset='utf-8'>\n";
  html_str += "<link rel='stylesheet' type='text/css' href='/style.css'>\n";
  html_str += "<script language='javascript' type='text/javascript' src='/parameter.js'></script>\n";
  html_str += "<script language='javascript' type='text/javascript' src='/javascript.js'></script>\n";
  html_str += "<title>Bewässerungssteuerung</title></head>\n";
  html_str += "<body>\n";
  html_str += "<table>\n";
  html_str += "  <tr>\n";
  html_str += "   <td colspan='13'>\n";
  html_str += "     <h2>Konfiguration</h2>\n";
  html_str += "   </td>\n";
  html_str += " </tr>\n";
  html_str += " <tr>\n";
  html_str += "   <td class='navi' style='width: 50px'></td>\n";
  sprintf(buffer, "   <td class='navi %s' style='width: 100px'><a href='/'>Status</a></td>\n", (pageactive==1)?"navi_active":"");
  html_str += buffer;
  html_str += "   <td class='navi' style='width: 50px'></td>\n";
  sprintf(buffer, "   <td class='navi %s' style='width: 100px'><a href='/PinConfig'>Basis Config</a></td>\n", (pageactive==2)?"navi_active":"");
  html_str += buffer;
  html_str += "   <td class='navi' style='width: 50px'></td>\n";
  sprintf(buffer, "   <td class='navi %s' style='width: 100px'><a href='/SensorConfig'>Sensor Config</a></td>\n", (pageactive==3)?"navi_active":"");
  html_str += buffer;
  html_str += "   <td class='navi' style='width: 50px'></td>\n";
  sprintf(buffer, "   <td class='navi %s' style='width: 100px'><a href='/VentilConfig'>Ventil Config</a></td>\n", (pageactive==4)?"navi_active":"");
  html_str += buffer;
  html_str += "   <td class='navi' style='width: 50px'></td>\n";
  sprintf(buffer, "   <td class='navi %s' style='width: 100px'><a href='/AutoConfig'>Automatik</a></td>\n", (pageactive==5)?"navi_active":"");
  html_str += buffer;
  html_str += "   <td class='navi' style='width: 50px'></td>\n";
  html_str += "   <td class='navi' style='width: 100px'><a href='https://github.com/tobiasfaust/ESP8266_PumpControl/wiki' target='_blank'>Wiki</a></td>\n";
  html_str += "   <td class='navi' style='width: 50px'></td>\n";
  html_str += " </tr>\n";
  html_str += "  <tr>\n";
  html_str += "   <td colspan='13'>\n";
  html_str += "<p />\n";

  return html_str.c_str();  
}

void setPage_Footer() {
  html_str += "   <td>\n";
  html_str += "  <tr>\n";
  html_str += "</table>\n";
  html_str += "</body>\n";
  html_str += "</html>\n";
}

String getPage_Status() {
  char buffer[100] = {0};
  memset(buffer, 0, sizeof(buffer));
  bool count = false;

  html_str = getPageHeader(1);
  html_str += "<table class='editorDemoTable'>\n";
  html_str += "<thead>\n";
  html_str += "<tr>\n";
  html_str += "<td style='width: 250px;'>Name</td>\n";
  html_str += "<td style='width: 200px;'>Wert</td>\n";
  html_str += "</tr>\n";
  html_str += "</thead>\n";
  html_str += "<tbody>\n";
  
  html_str += "<tr>\n";
  html_str += "<td>IP-Adresse:</td>\n";
  sprintf(buffer, "<td>%s</td>\n", WiFi.localIP().toString().c_str());
  html_str += buffer;
  html_str += "</tr>\n";

  html_str += "<tr>\n";
  html_str += "<td>WiFi Name:</td>\n";
  sprintf(buffer, "<td>%s</td>\n", WiFi.SSID().c_str());
  html_str += buffer;
  html_str += "</tr>\n";

  html_str += "<tr>\n";
  html_str += "<td>i2c Bus:</td>\n";
  html_str += "<td>";
  count=false;
  for (uint8_t i=0; i<8; i++) {
    if (i2c_adresses[i] > 0) {
      sprintf(buffer, " %02x", i2c_adresses[i]);
      html_str += buffer;
      html_str += ", ";
      count = true;
    } 
  }
  if (!count) {
    html_str += "keine i2c Devices gefunden";
  }
  html_str += "</td>\n";
  html_str += "</tr>\n";
  
  html_str += "<tr>\n";
  html_str += "<td>MAC:</td>\n";
  sprintf(buffer, "<td>%s</td>\n", WiFi.macAddress().c_str());
  html_str += buffer;
  html_str += "</tr>\n";

  html_str += "<tr>\n";
  html_str += "<td>WiFi RSSI:</td>\n";
  sprintf(buffer, "<td>%d</td>\n", WiFi.RSSI());
  html_str += buffer;
  html_str += "</tr>\n";

  html_str += "<tr>\n";
  html_str += "<td>Uptime:</td>\n";
  sprintf(buffer, "<td>%d</td>\n", WiFi.RSSI());
  html_str += buffer;
  html_str += "</tr>\n";

  html_str += "<tr>\n";
  html_str += "<td>Free Memory:</td>\n";
  sprintf(buffer, "<td>%d</td>\n", ESP.getFreeHeap());
  html_str += buffer;
  html_str += "</tr>\n";

  html_str += "<tr>\n";
  html_str += "<td>aktuell geöffnete Ventile</td>\n";
  html_str += "<td>\n";
  count=false;
  for(int i=0; i < pcf8574devCount; i++) {
    if (pcf8574dev[i].active) {
      sprintf(buffer, "%s noch %d/%d sek<br>\n", pcf8574dev[i].subtopic, (pcf8574dev[i].startmillis+pcf8574dev[i].lengthmillis-millis())/1000, pcf8574dev[i].lengthmillis / 1000);
      html_str += buffer; 
      count=true; 
    }
  }
  if (!count) { html_str += "alle Ventile geschlossen\n"; }
  html_str += "</td></tr>\n";

  html_str += "<tr>\n";
  html_str += "<td>Füllstand:</td>\n";
  sprintf(buffer, "<td>%d %%</td>\n", hcsr04_level);
  html_str += buffer;
  html_str += "</tr>\n";

  html_str += "<tr>\n";
  html_str += "<td>Firmware Update</td>\n";
  html_str += "<td><form action='update'><input class='button' type='submit' value='Update' /></form></td>\n";
  html_str += "</tr>\n";

  html_str += "<tr>\n";
  html_str += "<td>Device Reboot</td>\n";
  html_str += "<td><form action='reboot'><input class='button' type='submit' value='Reboot' /></form></td>\n";
  html_str += "</tr>\n";

  html_str += "</tbody>\n";
  html_str += "</table>\n";

  setPage_Footer();
  return html_str.c_str();  
}



String getPage_PinConfig() {
  char buffer[100] = {0};
  memset(buffer, 0, sizeof(buffer));
  html_str = getPageHeader(2);
  
  html_str += "<form id='F1' action='StorePinConfig' method='POST'>\n";
  html_str += "<table class='editorDemoTable'>\n";
  html_str += "<thead>\n";
  html_str += "<tr>\n";
  html_str += "<td style='width: 250px;'>Name</td>\n";
  html_str += "<td style='width: 200px;'>Wert</td>\n";
  html_str += "</tr>\n";
  html_str += "</thead>\n";
  html_str += "<tbody>\n";

  html_str += "<tr>\n";
  html_str += "<td>Device Name</td>\n";
  sprintf(buffer, "<td><input maxlength='40' name='mqttroot' type='text' value='%s'/></td>\n", mqtt_root);
  html_str += buffer;
  html_str += "</tr>\n";
  
  html_str += "<tr>\n";
  html_str += "<td>MQTT Server IP</td>\n";
  sprintf(buffer, "<td><input maxlength='15' name='mqttserver' type='text' value='%s'/></td>\n", mqtt_server);
  html_str += buffer;
  html_str += "</tr>\n";
  
  html_str += "<tr>\n";
  html_str += "<td>MQTT Server Port</td>\n";
  sprintf(buffer, "<td><input maxlength='5' name='mqttport' type='text' value='%d'/></td>\n", mqtt_port);
  html_str += buffer;
  html_str += "</tr>\n";

  html_str += "<tr>\n";
  html_str += "<td>Pin HC-SR04 Trigger</td>\n";
  sprintf(buffer, "<td><input min='0' max='15' id='GpioPin_0' name='pinhcsr04trigger' type='number' value='%d'/></td>\n", pin_hcsr04_trigger); 
  html_str += buffer;
  html_str += "</tr>\n";

  html_str += "<tr>\n";
  html_str += "<td>Pin HC-SR04 Echo</td>\n";
  sprintf(buffer, "<td><input min='0' max='15' id='GpioPin_1' name='pinhcsr04echo' type='number' value='%d'/></td>\n", pin_hcsr04_echo);
  html_str += buffer;
  html_str += "</tr>\n";

  html_str += "<tr>\n";
  html_str += "<td>Pin i2c SDA</td>\n";
  sprintf(buffer, "<td><input min='0' max='15' id='GpioPin_2' name='pinsda' type='number' value='%d'/></td>\n", pin_sda);
  html_str += buffer;
  html_str += "</tr>\n";

  html_str += "<tr>\n";
  html_str += "<td>Pin i2c SCL</td>\n";
  sprintf(buffer, "<td><input min='0' max='15' id='GpioPin_3' name='pinscl' type='number' value='%d'/></td>\n", pin_scl);
  html_str += buffer;
  html_str += "</tr>\n";

  html_str += "<tr>\n";
  html_str += "<td>i2c Adresse OLED 1306</td>\n";
  sprintf(buffer, "<td><input maxlength='2' name='i2coled' type='text' value='%02x'/></td>\n", i2caddress_oled);
  html_str += buffer;
  html_str += "</tr>\n";
 
  html_str += "</tbody>\n";
  html_str += "</table>\n";
  html_str += "<p><br /><input class='button' type='submit' value='Speichern' /></form>\n";

  setPage_Footer();
  return html_str.c_str();  
}


String getPage_SensorConfig() {
  char buffer[100] = {0};
  memset(buffer, 0, sizeof(buffer));
  html_str = getPageHeader(3);

  html_str += "<form id='F2' action='StoreSensorConfig' method='POST'>\n";
  html_str += "<table class='editorDemoTable'>\n";
  html_str += "<thead>\n";
  html_str += "<tr>\n";
  html_str += "<td style='width: 250px;'>Name</td>\n";
  html_str += "<td style='width: 200px;'>Wert</td>\n";
  html_str += "</tr>\n";
  html_str += "</thead>\n";
  html_str += "<tbody>\n";

  html_str += "<tr>\n";
  html_str += "<td>Messintervall HC-SR04</td>\n";
  sprintf(buffer, "<td><input min='0' max='254' name='hcsr04interval' type='number' value='%d'/></td>\n", hc_sr04_interval);
  html_str += buffer;
  html_str += "</tr>\n";

  html_str += "<tr>\n";
  html_str += "<td>Abstand Sensor min (in cm)</td>\n";
  sprintf(buffer, "<td><input min='0' max='254' name='hcsr04distmin' type='number' value='%d'/></td>\n", hc_sr04_distmin);
  html_str += buffer;
  html_str += "</tr>\n";
  html_str += "<tr>\n";
  html_str += "<td>Abstand Sensor max (in cm)</td>\n";
  sprintf(buffer, "<td><input min='0' max='254' name='hcsr04distmax' type='number' value='%d'/></td>\n", hc_sr04_distmax);
  html_str += buffer;
  html_str += "</tr>\n";
  html_str += "</tbody>\n";
  html_str += "</table>\n";
  html_str += "<p><br /><input class='button' type='submit' value='Speichern' /></form>\n";

  setPage_Footer();
  return html_str.c_str();  
}


String getPage_VentilConfig() {
  char buffer[100] = {0};
  memset(buffer, 0, sizeof(buffer));
  html_str = getPageHeader(4);

  html_str += "<form id='F2' action='StoreVentilConfig' method='POST'>\n";
  html_str += "<table class='editorDemoTable'>\n";
  html_str += "<thead>\n";
  html_str += "<tr>\n";
  html_str += "<td style='width: 25px;'>Nr</td>\n";
  html_str += "<td style='width: 25px;'>Active</td>\n";
  html_str += "<td style='width: 250px;'>MQTT SubTopic</td>\n";
  html_str += "<td style='width: 50 px;'>Port</td>\n";
  html_str += "</tr>\n";
  html_str += "</thead>\n";
  html_str += "<tbody>\n\n";
  
  html_str += "<tr>\n";
  for(int i=0; i < pcf8574devCount; i++) {
    sprintf(buffer, "<tr><td>%d</td>", i);
    html_str += buffer;
    sprintf(buffer, "<td><input name='active_%d' type='checkbox' value='1' %s/></td>", i, (pcf8574dev[i].enabled?"checked":""));
    html_str += buffer;
    sprintf(buffer, "<td><input maxlength='20' name='mqtttopic_%d' type='text' value='%s'/></td>", i, pcf8574dev[i].subtopic);
    html_str += buffer;
    sprintf(buffer, "<td><input id='AllePorts_%d' name='pcfport_%d' type='number' min='0' max='220' value='%d'/></td>", i, i, pcf8574dev[i].port);
    html_str += buffer;
    html_str += "</tr>\n";
  }
  html_str += "</tbody>\n";
  html_str += "</table>\n";
  html_str += "<br /><input class='button' type='submit' value='Speichern' /></form>\n";

  setPage_Footer();
  return html_str.c_str();  
}


String getPage_AutoConfig() {
  char buffer[200] = {0};
  memset(buffer, 0, sizeof(buffer));
  html_str = getPageHeader(5);
  html_str += "<form id='F2' action='StoreAutoConfig' method='POST'>\n";
  html_str += "<table class='editorDemoTable'>\n";
  html_str += "<thead>\n";
  html_str += "<tr>\n";
  html_str += "<td style='width: 25px;'>Active</td>\n";
  html_str += "<td style='width: 400px;'>Name</td>\n";
  html_str += "<td style='width: 200px;'>Wert</td>\n";
  html_str += "</tr>\n";
  html_str += "</thead>\n";
  html_str += "<tbody>\n";
  html_str += "<tr>\n";
  html_str += "<td style='text-align: center;'>&nbsp;</td>\n";
  html_str += "<td >Sensor Treshold Min</td>\n";
  sprintf(buffer, "<td><input min='0' max='254' name='treshold_min' type='number' value='%d'/></td>\n", hc_sr04_treshold_min);
  html_str += buffer;
  html_str += "</tr>\n";
  html_str += "<tr>\n";
  html_str += "<td style='text-align: center;'>&nbsp;</td>\n";
  html_str += "<td>Sensor Treshold Max</td>\n";
  sprintf(buffer, "<td><input min='0' max='254' name='treshold_max' type='number' value='%d'/></td>\n", hc_sr04_treshold_max);
  html_str += buffer;
  html_str += "</tr>\n";
  html_str += "<tr>\n";
  
  sprintf(buffer, "<td style='text-align: center;'><input name='enable_syncswitch' type='checkbox' value='1' %s /></td>\n", (enable_syncswitch?"checked":""));
  html_str += buffer;
  html_str += "<td>Ventil Trinkwasser Bypass</td>\n";
  sprintf(buffer, "<td><input min='0' max='254' id='ConfiguredPorts_0' name='syncswitch_port' type='number' value='%d'/></td>\n", syncswitch_port);
  html_str += buffer;
  html_str += "</tr>\n";

  html_str += "<tr>\n";
  sprintf(buffer, "<td style='text-align: center;'><input name='enable_3wege' type='checkbox' value='1' %s /></td>\n", (enable_3wege?"checked":""));
  html_str += buffer;
  html_str += "<td>3WegeVentil Trinkwasser Bypass</td>\n";
  sprintf(buffer, "<td><input min='0' max='254' id='ConfiguredPorts_1' name='ventil3wege_port' type='number' value='%d'/></td>\n", ventil3wege_port);
  html_str += buffer;
  html_str += "</tr>\n";

  html_str += "<tr>\n";
  html_str += "<td style='text-align: center;'>&nbsp;</td>\n";
  html_str += "<td>Max. parallel</td>\n";
  sprintf(buffer, "<td><input min='0' max='16' name='max_parallel' type='number' value='%d'/></td>\n", max_parallel);
  html_str += buffer;
  html_str += "</tr>\n";
  
  html_str += "</tbody>\n";
  html_str += "</table>\n";
  html_str += "<br /><input class='button' type='submit' value='Speichern' /></form>\n";

  setPage_Footer();
  return html_str.c_str();  
}

