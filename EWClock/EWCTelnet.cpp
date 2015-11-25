/*
 *  based on https://github.com/esp8266/Arduino/blob/esp8266/hardware/esp8266com/esp8266/libraries/ESP8266WiFi/examples/WiFiTelnetToSerial/WiFiTelnetToSerial.ino
 * 
 */
#include "EWCTelnet.h"
#include "EWCConfig.h"

WiFiServer server(TELNET_PORT);
WiFiClient serverClients[TELNET_MAX];

EWCTelnet::EWCTelnet()
{

}

EWCTelnet::~EWCTelnet()
{
  
}

bool EWCTelnet::begin()
{
  server.begin();
  server.setNoDelay(true);
  // Expects MDNS to be setup already
  // MDNS.addService("arduino", "tcp", TELNET_PORT);
  return true;
}

void EWCTelnet::handle()
{
  uint8_t i;
  if (server.hasClient()){
    for(i = 0; i < TELNET_MAX; i++){
      //find free/disconnected spot
      if (!serverClients[i] || !serverClients[i].connected()){
        if(serverClients[i]) serverClients[i].stop();
        serverClients[i] = server.available();
        Serial.print("New client: "); Serial.print(i);
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

void EWCTelnet::print(const char* str)
{
  uint8_t i;
  for(i = 0; i < TELNET_MAX; i++){
    if (serverClients[i] && serverClients[i].connected()){
      size_t len = sizeof(str)/sizeof(*str);
      serverClients[i].write(str, len);
    }
  }
}

void EWCTelnet::println(const char* str)
{
  uint8_t i;
  for(i = 0; i < TELNET_MAX; i++){
    if (serverClients[i] && serverClients[i].connected()){
      size_t len = sizeof(str)/sizeof(*str);
      serverClients[i].write(str, len);
    }
  }
}

EWCTelnet Telnet = EWCTelnet();
