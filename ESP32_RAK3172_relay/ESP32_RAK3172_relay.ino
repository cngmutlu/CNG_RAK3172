#define RXD1 12
#define TXD1 13

#define RXD2 16
#define TXD2 17

const int numNode = 25;
const int Qsize = 10;
const int UpSize = 10;

int ageWeight = 0.5;
int PRIWeight = 0.5;

int CountforAT = 0;
int allATdone = 0;
int readyforGateway = 0;
int idx = 0;
int maxindex = 0;
int i=0;
int j=0;
int timeout = 0;
int maxWeight[UpSize];
int InstAge[numNode];
int UpIndex[UpSize];
int ID = 0;
int PRI = 0;
long int DATA = 0;

unsigned long int LCFSqueue[numNode][Qsize];
int PRIS[numNode][Qsize];
long int AoI[numNode][Qsize];
int PRIScopy[numNode][Qsize];

unsigned long int period = 0;

String UplinkData = "";
String UplinkPacket = "";
String SensorData = "";

void setup() {
Serial.begin(9600);
Serial1.begin(9600, SERIAL_8N1, RXD1, TXD1);
Serial2.begin(9600, SERIAL_8N1, RXD2, TXD2);
}
void loop() {
  if (millis()-timeout > 180000){
    allATdone = 0;
    CountforAT = 0;
  }
  if (allATdone == 0)
  {
    switch (CountforAT)
    {
      case 0:
        Serial2.println("ATE");
        delay(1000);
        Serial2.println("AT");
        delay(200);
        CountforAT++;
        break;
      case 1:
        Serial2.println("AT+NWM=0");
        delay(200);
        CountforAT++;
        break;
      case 2:
        Serial2.println("AT+P2P=868000000:7:125:0:10:14");
        delay(300);
        CountforAT++;
        break;
      case 3:
        Serial2.println("AT+PRECV=65534");
        delay(200);
        CountforAT++;
        break;
      case 4:
        Serial1.println("ATE");
        delay(1000);
        Serial1.println("AT");
        delay(100);
        CountforAT++;
        break;
      case 5:
        Serial1.println("AT+NWM=0");
        delay(100);
        CountforAT++;
        break;
      case 6:
        Serial1.println("AT+P2P=868000000:8:125:0:10:14");
        delay(100);
        CountforAT++;
        break;
    }
    if(CountforAT == 7){
      allATdone = 1;
    }
  }
  if(allATdone == 1 || Serial1.available() || Serial2.available()){
    while (Serial.available())
      {
        String SerialString = Serial.readStringUntil('/n'); // Until CR (Carriage Return)
        Serial2.println(SerialString);
      }
     while (Serial1.available())
      {
        String Serial1String = Serial1.readStringUntil('/n'); // Until CR (Carriage Return)
        Serial.print("Serial1: ");
        Serial.println(Serial1String);
        period = millis();
        if(millis()-period > 60000){
          period = millis();
          for(i=0;i<numNode;i++){
            InstAge[i] = period - AoI[i][0];
          }
          for(i=0;i<numNode;i++){
            for(j=0;j<Qsize;j++){
              PRIScopy[i][j]=PRIS[i][j];
            }
          }
          for(i=0;i<UpSize;i++){
            maxWeight[i] = PRIWeight*PRIScopy[0][0] + ageWeight*InstAge[0];
            maxindex = 0;
            for(j=1;j<numNode;j++){
              if((PRIWeight*PRIS[j][0]+ageWeight*InstAge[j])>maxWeight[i]){
                maxWeight[i]=(PRIWeight*PRIS[j][0]+ageWeight*InstAge[j]);
                maxindex = j;
              }
              UpIndex[i] = maxindex;
              PRIScopy[maxindex][0]=0;
            }
          }
          UplinkData = "";
          for(i=0;i<UpSize;i++){
            maxindex = UpIndex[i];
            UplinkData = UplinkData + String(maxindex) + String(PRIS[maxindex][0]) + String(LCFSqueue[maxindex][0]);
          }
          UplinkPacket = "AT+PSEND=" + UplinkData;
          Serial.print("Uplink packet: ");
          Serial.println(UplinkPacket);
          Serial1.println(UplinkPacket);
          for(i=0;i<UpSize;i++){
            for(j=0;j<(Qsize-1);j++){
              maxindex = UpIndex[i];
              LCFSqueue[maxindex][j]=LCFSqueue[maxindex][j+1];
              LCFSqueue[maxindex][(Qsize-1)]=0;
              PRIS[maxindex][j]=PRIS[maxindex][j+1];
              PRIS[maxindex][(Qsize-1)]=0;
              AoI[maxindex][j]=AoI[maxindex][j+1];
              AoI[maxindex][(Qsize-1)]=0;
            }
          }
        }
      }
      while (Serial2.available())
      {
        timeout = millis();
        String Serial2String = Serial2.readStringUntil('/n'); // Until CR (Carriage Return)
        Serial.print("Serial2: ");
        Serial.println(Serial2String);
        if(Serial2String.indexOf("+EVT:D") > 0 )
        {
          idx = Serial2String.indexOf("+EVT:D");
          SensorData = Serial2String.substring(idx+6);
          SensorData.remove(2);
          ID = SensorData.toInt()-1;
          SensorData = Serial2String.substring(idx+6);
          SensorData.remove(0,2);
          SensorData.remove(1);
          PRI = SensorData.toInt();
          SensorData = Serial2String.substring(idx+6);
          SensorData.remove(0,3);
          DATA = SensorData.toInt();
          for(i=Qsize-1;i>0;i--){
            AoI[ID][i]=AoI[ID][i-1];
          }
          AoI[ID][0]=millis();
          for(i=Qsize-1;i>0;i--){
            LCFSqueue[ID][i]=LCFSqueue[ID][i-1];
          }
          LCFSqueue[ID][0]=DATA;
          for(i=Qsize-1;i>0;i--){
            PRIS[ID][i]=PRIS[ID][i-1];
          }
          PRIS[ID][0]=PRI;
          Serial.println("The LCFS queue");
          for(i=0;i<numNode;i++){
            for(j=0;j<Qsize;j++){
              Serial.print(LCFSqueue[i][j]);
            }
            Serial.println(".");
          }
        }
      }
  }
}
