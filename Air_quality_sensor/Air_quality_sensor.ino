#include <RSCG12864B.h>
#include <Wire.h>
#define DebugSerial Serial
#define PM2_5Serial Serial //用于接收PM2.5数据
#include "DHT.h"
#define DHTPIN 4    
#define DHTTYPE DHT11   // DHT 11
DHT dht(DHTPIN, DHTTYPE);
int buttonread=8,val;
int buttonplus,buttonminus,button;
byte buffer[32] = {};
int count = 0;
bool bufferComplete = false;
/*以下为汉字*/
char hum[]={0XCA,0XAA,0XB6,0XC8,0XA3,0XBA,0X0D,0X0A,0};      //湿度
char tem[]={0XCE,0XC2,0XB6,0XC8,0XA3,0XBA,0};                //温度
 
 
void setup() {
  RSCG12864B.begin();
  RSCG12864B.brightness(0);
  Serial.begin(115200);
  Wire.begin();
 
}
 
void loop() {
    while(bufferComplete == false)  
  {
    getPM();
  }
 
  int PM1 = buffer[10]*256+buffer[11];
  int PM2_5 = buffer[12]*256+buffer[13];
  int PM10 = buffer[14]*256+buffer[15];
 
  DebugSerial.print("PM1.0 = ");
  DebugSerial.print(PM1);
  DebugSerial.println("ug/m3");
 
  DebugSerial.print("PM2.5 = ");
  DebugSerial.print(PM2_5);
  DebugSerial.println("ug/m3");
 
  DebugSerial.print("PM10 = ");
  DebugSerial.print(PM10);
  DebugSerial.println("ug/m3");
  
  bufferComplete = false;
  /*温湿度传感器*/
    double h = dht.readHumidity();
    double t = dht.readTemperature();
  /*显示控制:汉字高度12*  (x,y)*/
    char hp[40];
    char tp[40];
    char PM1_P[40];
    char PM25_P[40];
    char PM10_P[40];
    char yr[40];
    char mon[40];
    char Date[40];
    char hr[40];
    char m[40];
    dtostrf(h,3,1,hp);
    dtostrf(t,3,1,tp); 
    dtostrf(PM1,3,1,PM1_P);
    dtostrf(PM2_5,3,1,PM25_P);  
    dtostrf(PM10,3,1,PM10_P); 
/*以下为按键控制
 *功能：按下按键时打开背光
 */
  val=digitalRead(buttonread);
  if(val==HIGH)
  {
    RSCG12864B.brightness(255);
    delay(5000);
  }
  else
{ 
  RSCG12864B.brightness(10);
  delay(1000);}
 
}
 
void getPM()
{
  int ppm;
  int count_sum = 0;
  long sum = 0;
 
  int flag_end = false;
  int avg_num = 10;
  byte receive_pre = 0x00;
 
 
  int flag_start = false;
  // DebugSerial.println("--------------------------");
  flag_end = false;
  while (flag_end == false)
  {
    // DebugSerial.println("+++++++++++++++++++++++++");
    // Serial.flush();
    if (PM2_5Serial.available() > 0) {
 
      char inChar = (char)PM2_5Serial.read();
      buffer[count] = (byte)inChar;
      if (buffer[count] == 0x4d &&  receive_pre == 0x42)
      {
        buffer[0] = 0x42;
        count = 1;  
        flag_start = true;
        DebugSerial.println("\r\n##################################################");
        DebugSerial.println("start");
      }
      receive_pre = (byte)inChar;
      // DebugSerial.print("##count=");
      // DebugSerial.println(count);
 
      // DebugSerial.print("buffer[count]=");
      // DebugSerial.print(buffer[count],HEX);
      // DebugSerial.print(",");
 
      count++;
 
      if (count >= 32)
      {
        count = 0;
        if (flag_start)
        {
          int checksum = 0;
          for(int i = 0 ; i < 30 ; i++)
          {
            checksum += buffer[i];
          }
          if (buffer[0] == 0x42 && buffer[1] == 0x4d && checksum == buffer[30]*256+buffer[31])
          {
            bufferComplete = true;
            // DebugSerial.println("bufferComplete = true;");
            
            flag_end = true;    //结束了，可以下一帧数据
 
            //把接收到的数据转发出来
            if (bufferComplete == true) {
              for (int i = 0 ; i < 32 ; i++)
              {
                DebugSerial.print(buffer[i], HEX);
                DebugSerial.print(",");
              }
              DebugSerial.println("");              
            }
          }
          else
          {
            DebugSerial.println("\r\n****error data*****");
            for (int i = 0 ; i < 7 ; i++)
            {
              DebugSerial.print(buffer[i], HEX);
              DebugSerial.print(",");
            }
            DebugSerial.println("\r\n****error*****");
          }
        }
      }
    }
 
  }
}
