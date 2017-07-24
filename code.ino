//NOUVEL ESSAI AVEC IMPLEMETATION DES CORRECTIONS

#include <SoftwareSerial.h>
#include <TFT_ST7735.h> 
#include <SPI.h>
#include <DHT.h>
#include <TinyGPS.h>
#include "SdFat.h"

const int chipSelect = 4;

#define FILE_BASE_NAME "Data"

SdFat sd;
SdFile file;

TinyGPS gps;

SoftwareSerial ss(3,2);
SoftwareSerial ss2(6,7);

#define DHTPIN A0 
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

TFT_ST7735 tft = TFT_ST7735(); 

byte xpos_txt0 = 0;
byte ypos_txt0 = 0;

byte xpos_txt1 = 0;
byte ypos_txt1 = 15;

byte xpos_txt2 = 0;
byte ypos_txt2 = 30;

byte xpos_txt3 = 0;
byte ypos_txt3 = 45;

byte xpos_txt4 = 0;
byte ypos_txt4 = 60;

byte xpos_txt5 = 0;
byte ypos_txt5 = 75;

byte xpos_txt6 = 0;
byte ypos_txt6 = 90;

byte xpos_txt7 = 0;
byte ypos_txt7 = 105;

byte xpos_txt8 = 0;
byte ypos_txt8 = 120;

byte xpos_txt9 = 0;
byte ypos_txt9= 135;


unsigned int Pm25;
unsigned int Pm10;


void setup()
{    
  ss.begin(9600);
  ss2.begin(19200);
  tft.init();
  tft.setRotation(0);
  tft.fillScreen(ST7735_BLACK);
  tft.setTextColor(0xFFFF, ST7735_BLACK);
  dht.begin();
  
 xpos_txt1 += tft.drawString("Humidity: ",xpos_txt1,ypos_txt1,1);
 xpos_txt2 += tft.drawString("Temperature: ",xpos_txt2,ypos_txt2,1);
 xpos_txt3 += tft.drawString("PM2.5: ",xpos_txt3,ypos_txt3,1);
 xpos_txt4 += tft.drawString("PM10: ",xpos_txt4,ypos_txt4,1); 
 xpos_txt5 += tft.drawString("Latitude: ",xpos_txt5,ypos_txt5,1);
 xpos_txt6 += tft.drawString("Longitude: ",xpos_txt6,ypos_txt6,1);
 xpos_txt7 += tft.drawString("Altitude: ",xpos_txt7,ypos_txt7,1);
 tft.drawString(" ",xpos_txt8,ypos_txt8,1);
 
 //Init important!

 pinMode(10, OUTPUT); 
// digitalWrite(10, HIGH);
 
 // GPS starter
   pinMode(5, OUTPUT);
   digitalWrite(5, HIGH);
 
 
 const uint8_t BASE_NAME_SIZE = sizeof(FILE_BASE_NAME) - 1;
 char fileName[13] = FILE_BASE_NAME "00.csv";
 
 delay(1000);

 // Initialize the SD card at SPI_HALF_SPEED to avoid bus errors with
  // breadboards.  use SPI_FULL_SPEED for better performance.
  if (!sd.begin(chipSelect, SPI_HALF_SPEED)) {
      tft.setTextColor(0xF800, ST7735_BLACK);
    tft.drawString("Card failed",xpos_txt9,ypos_txt9,1);
//    sd.initErrorHalt();
    return;
  }
 
   // Find an unused file name.
  while (sd.exists(fileName)) {
    if (fileName[BASE_NAME_SIZE + 1] != '9') {
      fileName[BASE_NAME_SIZE + 1]++;
    } else if (fileName[BASE_NAME_SIZE] != '9') {
      fileName[BASE_NAME_SIZE + 1] = '0';
      fileName[BASE_NAME_SIZE]++;
    } else {
        tft.setTextColor(0xF800, ST7735_BLACK);
          tft.drawString("Can't create file name",xpos_txt9,ypos_txt9,1);
              return;

    }
  }
  if (!file.open(fileName, O_CREAT | O_WRITE | O_EXCL)) {
      tft.setTextColor(0xF800, ST7735_BLACK);
        tft.drawString("Error file",xpos_txt9,ypos_txt9,1);
            return;
     }
 
  file.print(F("Time;Humidity;Temperature;PM25;PM10;Latitute;Longitude;Altitude"));
  file.println();
  file.sync();  
}

void loop()
{
 
byte x1 = xpos_txt1;
byte x2 = xpos_txt2;
 byte x3 = xpos_txt3;
 byte x4 = xpos_txt4;
  
  
  int h = dht.readHumidity();
  int t = dht.readTemperature();
  tft.setTextColor(0xFFFF, ST7735_BLACK);
  x1 += tft.drawNumber(h,x1,ypos_txt1,1);
  tft.drawString(" %  ",x1,ypos_txt1,1);
  x2 += tft.drawNumber(t,x2,ypos_txt2,1);
  tft.drawString(" C  ",x2,ypos_txt2,1);
  
  bool newData = false;
  unsigned long chars;
  unsigned short sentences, failed;

  String UTC = "";
    
  //Check for new PM data

  uint8_t mData = 0;
  uint8_t i = 0;
  uint8_t mPkt[10] = {0};
  uint8_t mCheck = 0;
  
  ss.listen();
  
 while (ss.available()) 
  {  
     mData = ss.read();  
    delay(2);
    if(mData == 0xAA)
     {
	delay(400);
        mPkt[0] =  mData;
        mData = ss.read();
        if(mData == 0xc0)
        {
          mPkt[1] =  mData;
          mCheck = 0;
          for(i=0;i < 6;i++)
          {
             mPkt[i+2] = ss.read();
             delay(2);
             mCheck += mPkt[i+2];
          }
          mPkt[8] = ss.read();
          delay(1);
	  mPkt[9] = ss.read();

          if(mCheck == mPkt[8])
          {
            ss2.listen();

            Pm25 = (uint16_t)mPkt[2] | (uint16_t)(mPkt[3]<<8);
            Pm10 = (uint16_t)mPkt[4] | (uint16_t)(mPkt[5]<<8);
            
            if(Pm25 > 9999)
             Pm25 = 9999;
            if(Pm10 > 9999)
             Pm10 = 9999;            

            
            
            x3 += tft.drawFloat((float)Pm25/(float)10,1,x3,ypos_txt3,1); 
            tft.drawString(" ug/m3  ",x3,ypos_txt3,1);
            x4 += tft.drawFloat((float)Pm10/(float)10,1,x4,ypos_txt4,1);
            tft.drawString(" ug/m3  ",x4,ypos_txt4,1);

          }
        }      
     }
   } 
  

     //Check for new GPS data

   for (unsigned long start = millis(); millis() - start < 1000;)
  {
    while (ss2.available())
    {
      char c = ss2.read();
      if (gps.encode(c))
        newData = true;
    }
  }
   
   if (newData)
  {
    float lati, longi, alti;
    int year;
    byte month, day, hour, minute, second, hundredths;
    unsigned long age;
    
    gps.crack_datetime(&year, &month, &day, &hour, &minute, &second, &hundredths, &age);
    if (age == TinyGPS::GPS_INVALID_AGE)
        {
          tft.drawString("********************",xpos_txt0,ypos_txt0,1);
          UTC += "********************";
        }
    else
    {
     char sz[32];
      char iso[32];
      sprintf(sz, "%02d/%02d/%02d %02d:%02d:%02d ",
          day, month, year, hour, minute, second);
      sprintf(iso, "%02d-%02d-%02dT%02d:%02d:%02dZ",
          year, month, day, hour, minute, second);   
      tft.drawString(sz,xpos_txt0,ypos_txt0,1);
      UTC += iso;
    }
    gps.f_get_position(&lati, &longi,&age);
    alti = gps.f_altitude();
    tft.drawFloat(lati,6,xpos_txt5,ypos_txt5,1);
    tft.drawFloat(longi,6,xpos_txt6,ypos_txt6,1);
    tft.drawFloat(alti,2,xpos_txt7,ypos_txt7,1);
    tft.drawString(" ",xpos_txt8,ypos_txt8,1);

   
   if (!(h == 0 || t == 0 || h == 255 || t == 255 || Pm25 == 9999 || Pm10 == 9999) && UTC.length()== 20 && !(UTC == "********************" )){
   
    file.print(UTC); 
    file.write(';');
    file.print(h); 
    file.write(';');
    file.print(t); 
    file.write(';');
    file.print((float)Pm25/(float)10,1); 
    file.write(';');
    file.print((float)Pm10/(float)10,1); 
    file.write(';');
    file.print(lati,6); 
    file.write(';');
    file.print(longi,6); 
    file.write(';');
    file.print(alti,2); 
    file.println();
    
   if (!file.sync() || file.getWriteError()) {
        tft.setTextColor(0xF800, ST7735_BLACK);
        tft.drawString("Write error",xpos_txt9,ypos_txt9,1);
        char *saveName;
        file.getName(saveName,13);
        file.close();
        delay(1000);
        tft.drawString("                 ",xpos_txt9,ypos_txt9,1);
        file.open(saveName, O_WRITE);
        delay(1000);
        file.print(UTC); 
        file.write(';');
        file.print(h); 
        file.write(';');
        file.print(t); 
        file.write(';');
        file.print((float)Pm25/(float)10,1); 
        file.write(';');
        file.print((float)Pm10/(float)10,1); 
        file.write(';');
        file.print(lati,6); 
        file.write(';');
        file.print(longi,6); 
        file.write(';');
        file.print(alti,2); 
        file.println();
        if (!file.sync() || file.getWriteError()) {
        tft.setTextColor(0xF800, ST7735_BLACK);
        tft.drawString("Write error x2!",xpos_txt9,ypos_txt9,1);
        delay(1000);
        tft.drawString("                 ",xpos_txt9,ypos_txt9,1);
        }
        
  }
   }
  }
 }

