// This code read RFID and display on lcd along with time

#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <LiquidCrystal_I2C.h>
#include <WiFiUdp.h>
#include <NTPClient.h>
#include <TimeLib.h>
#include<Wire.h>
int count = 0; 
int c=1;
char input[12];
LiquidCrystal_I2C lcd(0x3F,16,2);

ESP8266WebServer server (80);
const char* ssid = "********";
const char* password = "********";
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "asia.pool.ntp.org", 19740, 60000);
char Time[ ] = "TIME:00:00:00";
char Date[ ] = "DATE:00/00/2000";
byte last_second, second_, minute_, hour_, day_, month_;
int year_;
void setup()
{
  Serial.begin(9600);
  WiFi.begin(ssid, password);
  Wire.begin(D2, D1);
  lcd.begin(16, 2);
  lcd.init();
  lcd.backlight();
  welcome();
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
   //lcd.setCursor(0, 0);
    //lcd.print("WiFi connecting...");
  }
  //lcd.setCursor(0, 0);
  Serial.print("WiFi connected");
  //lcd.setCursor(0, 1);
  Serial.print(WiFi.localIP());
  delay(500);
}
 
  void loop(){
   
   if(Serial.available())
   {
      count = 0;
      while(Serial.available() && count < 12)
      {
         input[count] = Serial.read();
         count++;
         delay(5);

      }
      //Serial.print(input);

  if ((strncmp(input, "16002894AB01", 12) == 0)&&(c%2!=0))
      {
        Serial.print("MOULD ID :");
        Serial.print(input);
        Serial.print("Has entered pressing room");
        Serial.println();
      in();
        delay(2000);
        dt();
        delay(2000);
        welcome();
        c++;
      }
       else if ((strncmp(input, "16002894AB01", 12) == 0)&&(c%2==0))
      {
        Serial.print("MOULD ID :");
        Serial.print(input);
        Serial.print("Has entered pressing room");
        Serial.println();
      out();
        delay(2000);
        lcd.clear();
      dt();
        delay(2000);
        welcome();
        c++;
      }
      else
      {
        Serial.print("MOULD ID :");
        Serial.print(input);
        Serial.print("Is been restricted here");
        Serial.println();
      denied();
          delay(2000);
       dt();
        delay(2000);
        welcome();
      }

  }
}
void datetime()
{
  timeClient.update();
  unsigned long unix_epoch = timeClient.getEpochTime();    // Get Unix epoch time from the NTP server
  second_ = second(unix_epoch);
  if (last_second != second_) {
    minute_ = minute(unix_epoch);
    hour_   = hour(unix_epoch);
    day_    = day(unix_epoch);
    month_  = month(unix_epoch);
    year_   = year(unix_epoch);
    Time[12] = second_ % 10 + 48;
    Time[11] = second_ / 10 + 48;
    Time[9]  = minute_ % 10 + 48;
    Time[8]  = minute_ / 10 + 48;
    Time[6]  = hour_   % 10 + 48;
    Time[5]  = hour_   / 10 + 48;
    Date[5]  = day_   / 10 + 48;
    Date[6]  = day_   % 10 + 48;
    Date[8]  = month_  / 10 + 48;
    Date[9]  = month_  % 10 + 48;
    Date[13] = (year_   / 10) % 10 + 48;
    Date[14] = year_   % 10 % 10 + 48;
    Serial.println(Time);
    Serial.println(Date);
    last_second = second_;
  }
  delay(500);
 
}
void welcome()
  {lcd.clear();
    lcd.setCursor(1, 0);
  lcd.print("WELCOME TO PQSI");
  lcd.setCursor(2, 1);
  lcd.print("ROOM 03  ");
  }
  void in()
  {
      lcd.clear();
        lcd.setCursor(0,0);   //Set cursor to character 2 on line 0
        lcd.print("ID:");

        lcd.print(input);
        lcd.setCursor(0,1);   //Move cursor to character 2 on line 1
        lcd.print("Granted");
        lcd.print(" -IN");
        
  }
  void dt()
  {
    lcd.clear();
        datetime();
        lcd.setCursor(0,0);   //Set cursor to character 2 on line 0
        lcd.print(Time);
        lcd.setCursor(0,1);   //Set cursor to character 2 on line 0
        lcd.print("Date:09/12/2021");
  }
  void denied()
  {
      lcd.clear();
        lcd.setCursor(0,0);   //Set cursor to character 2 on line 0
        lcd.print("ID:");
        lcd.print(input);
      
        lcd.setCursor(0,1);   //Move cursor to character 2 on line 1
        lcd.print("Denied");
  }
  void out()
  {
      lcd.clear();
        lcd.setCursor(0,0);   //Set cursor to character 2 on line 0
        lcd.print("ID:");

        lcd.print(input);
        lcd.setCursor(0,1);   //Move cursor to character 2 on line 1
        lcd.print("Granted");
        lcd.print(" -OUT");
        
  }

  
