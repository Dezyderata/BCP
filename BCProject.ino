#include <TimeLib.h>
#include <WiFiUdp.h>
#include <ESP8266WiFi.h>

#ifndef STASSID
#define STASSID ""
#define STAPSK ""
#endif

const char* ssid = STASSID;
const char* password = STAPSK;
const char* ntpServerName = "ntp.task.gda.pl";

IPAddress timeServerIP;

unsigned int localPort = 2390;
const int NTP_PACKET_SIZE = 48;

byte packetBuffer[NTP_PACKET_SIZE];

const unsigned long seventyYears =2208988800UL;

WiFiUDP udp;

int SER1 = 4;
int SCK1 = 6;
int RCK1 = 8;
int i = 100;
time_t t;

time_t getTimeFromNTPServer();
unsigned long sendNTPpacket(IPAddress&);
void sendVal(int, int, int);
time_t tmConvert_t(int, byte, byte, byte, byte, byte);

void setup() {
  pinMode(SER1, OUTPUT);
  pinMode(SCK1, OUTPUT);
  pinMode(RCK1, OUTPUT);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  while(WiFi.status()!=WL_CONNECTED){
    delay(500);
  }
  udp.begin(localPort); 
  //t = tmConvert_t(2000,1,1,15,56,00); 
}

void loop() {
  if(i >=100){
    time_t nowy = getTimeFromNTPServer();
    if(nowy!=0){
      t = nowy;
      i = 0;
      }else{
        t = t+2;
      }
  }
  digitalWrite(RCK1, LOW);
  sendVal(SCK1, SER1, hour(t));  
  sendVal(SCK1, SER1, minute(t));
  sendVal(SCK1, SER1, second(t));
  digitalWrite(RCK1, HIGH);
  delay(1000);
  ++t;
  ++i;
}
void sendVal(int SCK1, int SER1, int k){
  int j;
  for (int i=0; i<8; i++){
    digitalWrite(SCK1, LOW);
    j = k%2;
    k = k>>1;
    if(j == 1){ 
      digitalWrite(SER1, HIGH);
    }else{
      digitalWrite(SER1, LOW);  
    }
    digitalWrite(SCK1, HIGH);
  }  
}
unsigned long sendNTPpacket(IPAddress& address){
    memset(packetBuffer, 0, NTP_PACKET_SIZE);
    packetBuffer[0] = 0b11100011;
    packetBuffer[1] = 0;
    packetBuffer[2] = 6;
    packetBuffer[3] = 0xEC;
    packetBuffer[12] = 49;
    packetBuffer[13] = 0x4E;
    packetBuffer[14] = 49;
    packetBuffer[15] = 52;
    udp.beginPacket(address, 123);
    udp.write(packetBuffer, NTP_PACKET_SIZE);
    udp.endPacket();
  }
time_t getTimeFromNTPServer(){
  WiFi.hostByName(ntpServerName, timeServerIP);
  sendNTPpacket(timeServerIP);
  delay(1000);
  if(udp.parsePacket()){
      udp.read(packetBuffer, NTP_PACKET_SIZE);  
      unsigned long highWord = word(packetBuffer[40], packetBuffer[41]);
      unsigned long lowWord =word(packetBuffer[42], packetBuffer[43]);
      unsigned long secsSince1900 = highWord << 16 | lowWord;
      unsigned long epoch = secsSince1900 - seventyYears;      
      return epoch;
    }else{
      return 0;
    }
}
time_t tmConvert_t(int YYYY, byte MM, byte DD, byte hh, byte mm, byte ss){
  tmElements_t tmSet;
  tmSet.Year = YYYY - 1970;
  tmSet.Month = MM;
  tmSet.Day = DD;
  tmSet.Hour = hh;
  tmSet.Minute = mm;
  tmSet.Second = ss;
  return makeTime(tmSet);         
}
