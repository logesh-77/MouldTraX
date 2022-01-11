//This code read RFID ,if ID satisifies those conditions allowed to pass in and an entry log was written in firestore dashboard

#if defined(ESP32)
#include <WiFi.h>
#elif defined(ESP8266)
#include <ESP8266WiFi.h>
#endif
#include <Firebase_ESP_Client.h>
#include<String.h>
//Provide the token generation process info.
#include <addons/TokenHelper.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <WiFiUdp.h>
#include <NTPClient.h>
#include <TimeLib.h>
//#include<Wire.h>
int count = 0;
int var=0;
char input[12];
String id;
String state;
#define API_KEY "___________________________________________"  //Add your firebase API key
#define FIREBASE_PROJECT_ID "_______________________________"  //Add firebase project ID
#define USER_EMAIL "*****************"  // create firebase client and their email ID here
#define USER_PASSWORD "**************"
//Define Firebase Data object
FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;
unsigned long dataMillis = 0;
ESP8266WebServer server (80);
const char* ssid = "logesh";
const char* password = "logesh77";
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "asia.pool.ntp.org", 19740, 60000);
char Time[ ] = "TIME:00:00:00";
char Date[ ] = "DATE:00/00/2000";
byte last_second, second_, minute_, hour_, day_, month_;
int year_;
void setup()
{
  pinMode(D3, OUTPUT);
  Serial.begin(9600);
  WiFi.begin(ssid, password);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
  }
  Serial.print("WiFi connected");
  Serial.print(WiFi.localIP());
  delay(500);
    config.api_key = API_KEY;
    auth.user.email = USER_EMAIL;
    auth.user.password = USER_PASSWORD;
    config.token_status_callback = tokenStatusCallback; //see addons/TokenHelper.h
    Firebase.begin(&config, &auth);
    Firebase.reconnectWiFi(true);
}
void loop(){
   if(Serial.available())
   {
      datetime();
      count = 0;
      digitalWrite(D3, HIGH);
      delay(50);
      digitalWrite(D3, LOW);
      while(Serial.available() && count < 12)
      {
         input[count] = Serial.read();
         count++;
         delay(5);
      }
      id=input;
      Serial.print(id);
      Serial.println();
      if (strncmp(input, "16002894AB01", 12) == 0)
      {
        state = "Access Granted";
        idpush(id,state);
        dtpush(Time,Date);
      }
      else if (strncmp(input, "16000BD0AA67", 12) == 0)
      {
        state = "Access Granted";
        idpush(id,state);
        dtpush(Time,Date);
      }
      else
      {
        state = "Access Denied";
        idpush(id,state);
        dtpush(Time,Date);
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
void idpush(String id,String state)
  {
     if (Firebase.ready() && (millis() - dataMillis > 50|| dataMillis == 0))
    {
        dataMillis = millis();
        var++;
        Serial.print("Commit a document (append array)... ");
        std::vector<struct fb_esp_firestore_document_write_t> writes;
        struct fb_esp_firestore_document_write_t transform_write;
        transform_write.type = fb_esp_firestore_document_write_type_transform;
        transform_write.document_transform.transform_document_path = "Gateway_01/moulds";
        struct fb_esp_firestore_document_write_field_transforms_t field_transforms;
        field_transforms.fieldPath = "appended_data";
        field_transforms.transform_type = fb_esp_firestore_transform_type_append_missing_elements;
        FirebaseJson content;
        String txt = id + ","  + state;
        content.set("values/[0]/stringValue", txt);
        field_transforms.transform_content = content.raw();
        transform_write.document_transform.field_transforms.push_back(field_transforms);
        writes.push_back(transform_write);
        if (Firebase.Firestore.commitDocument(&fbdo, FIREBASE_PROJECT_ID, "" , writes , "" ))
            Serial.printf("ok\n%s\n\n", fbdo.payload().c_str());
        else
            Serial.println(fbdo.errorReason());
    }
}
void dtpush(String Time,String Date)
  {
     if (Firebase.ready() && (millis() - dataMillis > 50|| dataMillis == 0))
    {
        dataMillis = millis();
        var++;
        Serial.print("Commit a document (append array)... ");
        std::vector<struct fb_esp_firestore_document_write_t> writes;
        struct fb_esp_firestore_document_write_t transform_write;
        transform_write.type = fb_esp_firestore_document_write_type_transform;
        transform_write.document_transform.transform_document_path = "Gateway_01/time";
        struct fb_esp_firestore_document_write_field_transforms_t field_transforms;
        field_transforms.fieldPath = "appended_data";

        field_transforms.transform_type = fb_esp_firestore_transform_type_append_missing_elements;

        FirebaseJson content;
        String txt =Time + "," + Date;
        content.set("values/[0]/stringValue", txt);
        field_transforms.transform_content = content.raw();
        transform_write.document_transform.field_transforms.push_back(field_transforms);
        writes.push_back(transform_write);
        if (Firebase.Firestore.commitDocument(&fbdo, FIREBASE_PROJECT_ID, "" , writes , "" ))
            Serial.printf("ok\n%s\n\n", fbdo.payload().c_str());
        else
            Serial.println(fbdo.errorReason());
    }
}
