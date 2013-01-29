#include <EtherCard.h>
#include <OneWire.h>
#include <DallasTemperature.h>

static byte mymac[] = {0xDD,0xDD,0xDD,0x00,0x00,0x01};
static byte server_IP[] = {192,168,1,202};
static int server_Port = 1000;
byte Ethernet::buffer[600];

#define ONE_WIRE_BUS 2
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

unsigned long previousMillis = 0;
unsigned long interval = 10000;

void setup () {
 
  Serial.begin(57600);
  Serial.println("Temp demo");
  Serial.println();
 
  if (!ether.begin(sizeof Ethernet::buffer, mymac, 10)) {
    Serial.println( "Failed to access Ethernet controller");
    while(1); 
  } else Serial.println("Ethernet controller initialized");
  Serial.println();
 
  if (!ether.dhcpSetup()) {
    Serial.println("Failed to get configuration from DHCP");
    while(1);
  } else Serial.println("DHCP configuration done:"); 
 
  ether.printIp("IP Address:\t", ether.myip);
  ether.printIp("Netmask:\t", ether.mymask);
  ether.printIp("Gateway:\t", ether.gwip);
  
  ether.copyIp(ether.hisip, server_IP);
  ether.hisport = server_Port;
}
  
void loop() {

  ether.packetLoop(ether.packetReceive());
  
  unsigned long currentMillis = millis();
  
  if(currentMillis - previousMillis > interval) {

    previousMillis = currentMillis;   
    sensors.requestTemperatures();
    float float_temp = sensors.getTempCByIndex(0);
    
    char string_temp[7];
    dtostrf(float_temp, 4, 2, string_temp);
    Serial.print("Temperature -> ");
    Serial.println(string_temp);
    
    Stash stash;
    byte sd = stash.create();
    stash.print(string_temp);
    stash.print('\r');
    stash.save();     
    Stash::prepare(PSTR("$H"), sd);
    ether.tcpSend();
    
    Serial.println("Packet sent!");
    Serial.println();
  }  
}

