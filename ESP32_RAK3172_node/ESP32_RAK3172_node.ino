#define RXD2 16
#define TXD2 17

int count = 0;
int period = 0;

void setup() {
Serial.begin(9600);
Serial2.begin(9600, SERIAL_8N1, RXD2, TXD2);
}
void loop() {
  switch(count)
  {
    case 0:
      Serial2.println("ATE");
      delay(200);
      count++;
      break;
    case 1:
      Serial2.println("AT+NWM=0");
      delay(200);
      count++;
      break;
    case 2:
      Serial2.println("AT+P2P=868000000:7:125:0:10:14");
      delay(200);
      count++;
      period = millis();
      break;
  }
  if(count == 3){
    if(millis()-period>6500){
      period = millis();
      Serial2.println("AT+PSEND=D02911");
    }
  }
  if (Serial.available())
  {
  Serial2.println(Serial.readStringUntil('/n'));
  }
  if (Serial2.available())
  {
  Serial.println(Serial2.readStringUntil('/n'));
  }
}
