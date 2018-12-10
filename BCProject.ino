#include <TimeLib.h>

int SER1 = 4;
int SCK1 = 5;
int RCK1 = 14;

time_t t;

time_t tmConvert_t(int YYYY, byte MM, byte DD, byte hh, byte mm, byte ss)
{
  tmElements_t tmSet;
  tmSet.Year = YYYY - 1970;
  tmSet.Month = MM;
  tmSet.Day = DD;
  tmSet.Hour = hh;
  tmSet.Minute = mm;
  tmSet.Second = ss;
  return makeTime(tmSet);         //convert to time_t
}

void setup() {
  
  pinMode(SER1, OUTPUT);
  pinMode(SCK1, OUTPUT);
  pinMode(RCK1, OUTPUT);

  t = tmConvert_t(2000,1,1,23,23,23); 
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

void loop() {
  int i, k, j;
  k = hour(t);
  digitalWrite(RCK1, LOW);
  sendVal(SCK1, SER1, hour(t));  
  sendVal(SCK1, SER1, minute(t));
  sendVal(SCK1, SER1, second(t));
  digitalWrite(RCK1, HIGH);
  delay(1000);
  t = tmConvert_t(2000,1,1,hour(t),minute(t),(second(t)+1));
}
