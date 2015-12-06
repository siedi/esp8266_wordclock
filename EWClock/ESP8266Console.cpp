/*
 *  based on https://github.com/esp8266/Arduino/blob/esp8266/hardware/esp8266com/esp8266/libraries/ESP8266WiFi/examples/WiFiTelnetToSerial/WiFiTelnetToSerial.ino
 * 
 */

#include "ESP8266Console.h"

WiFiServer server(TELNET_PORT);
WiFiClient serverClients[TELNET_MAX];

ESP8266Console::ESP8266Console()
{

}

ESP8266Console::~ESP8266Console()
{
  
}

void ESP8266Console::begin()
{
  server.begin();
  server.setNoDelay(true);
}

void ESP8266Console::handle()
{
  uint8_t i;
  
  // check for new clients
  if (server.hasClient()){
    for(i = 0; i < TELNET_MAX; i++){
      //find free/disconnected spot
      if (!serverClients[i] || !serverClients[i].connected()){
        if(serverClients[i]) serverClients[i].stop();
        serverClients[i] = server.available();
        continue;
      }
    }
    //no free/disconnected spot so reject
    WiFiClient serverClient = server.available();
    serverClient.stop();
  }
  
  //check clients for data
  for(i = 0; i < TELNET_MAX; i++){
    if (serverClients[i] && serverClients[i].connected()){
      if(serverClients[i].available()){
        //get data from the telnet client and push it to the UART
        while(serverClients[i].available()) Serial.write(serverClients[i].read());
      }
    }
  }

  //check UART for data
  if(Serial.available()){
    size_t len = Serial.available();
    uint8_t sbuf[len];
    Serial.readBytes(sbuf, len);
    //push UART data to all connected telnet clients
    for(i = 0; i < TELNET_MAX; i++){
      if (serverClients[i] && serverClients[i].connected()){
        serverClients[i].write(sbuf, len);
        delay(1);
      }
    }
  }

}

size_t ESP8266Console::write(uint8_t c)
{
  return write(&c, 1);
}

size_t ESP8266Console::write(const uint8_t *buf, size_t len)
{
  Serial.write(buf, len);
  uint8_t i;
  for(i = 0; i < TELNET_MAX; i++) {
    if (serverClients[i] && serverClients[i].connected()){
      serverClients[i].write(buf, len);
    }
  }
  return len;
}

int ESP8266Console::available(void) {
  // not implemented yet, we handle the input
  return 0;
}

int ESP8266Console::read(void){
  // not implemented yet, we don't receive any input yet
  return -1;
}

int ESP8266Console::peek(void){
  // not implemented yet, we don't receive any input yet
  return -1;
}

void ESP8266Console::flush(void){
  // not implemented yet, we don't receive any input yet
}

ESP8266Console Console = ESP8266Console();
