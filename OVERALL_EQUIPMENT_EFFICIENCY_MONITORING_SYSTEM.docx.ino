#include <WiFi.h>
#include <HTTPClient.h>

#include "FS.h"
#include "SD.h"
#include <SPI.h>

#define SD_CS 5
String dataMessage;

#include <RTClib.h> // for the RTC

const char* ssid = "R&D LABS";
const char* password =  "";
String dieclo = "";
String injectproces = "";
String ejectproces = "";
String por = "";

String dataString = "";
String dataString1 = "";
String dataString2 = "";
String dataString3 = "";
//variable setting
bool p_state = false;
bool c_state = false;
bool i_state = false;
bool e_state = false;
bool p_statef = false;
bool c_statef = false;
bool i_statef = false;
bool e_statef = false;
bool p = false;
bool c = false;
bool i = false;
bool e = false;
bool last_p = false;
bool last_c = false;
bool last_i = false;
bool last_e = false;
bool intavail = false;
File myFile;

RTC_DS3231 rtc;
//reset declaration
void(* resetFunc) (void) = 0;
void m_state_update(String a, bool b,String c, bool d,String e, bool f, String g, bool h);


void setup() {
 
  pinMode(34, INPUT);
  pinMode(35, INPUT);
  pinMode(36, INPUT);
  pinMode(39, INPUT);
 
  Serial.begin(115200);
  digitalWrite(19, 1);
  digitalWrite(27, 1);
 
   SD.begin(SD_CS);
   
 //*************SD Card***************
  if(!SD.begin(SD_CS)) {
    Serial.println("Card Mount Failed");
    return;
  }
  uint8_t cardType = SD.cardType();
  if(cardType == CARD_NONE) {
    Serial.println("No SD card attached");
    return;
  }
  Serial.println("Initializing SD card...");
  if (!SD.begin(SD_CS)) {
    Serial.println("ERROR - SD card initialization failed!");
    return;    // init failed
  }
  if(SD.mkdir("/Log")){
      Serial.println("Dir created");
  }
  else {
      Serial.println("mkdir failed");
  }
  //*********************************************

  if (! rtc.begin()) {
    Serial.println("Couldn't find RTC");
    while (1);
  }
  Serial.println("Netwwork established");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    machine_status();
   // DateTime now = rtc.now();
    //******************Storage*****************p_state,c_state,i_state,e_state,dataString
//    dateString = String(now.day());
//    montheString = String(now.month());
//    yeareString = String(now.year());
//    File file = SD.open(dateString+"_"+montheString+"_"+yeareString+".txt");
//    if(!file) {
//    Serial.println("File doens't exist");
//    Serial.println("Creating file...");
//    //writeFile(SD, dateString+"_"+montheString+"_"+yeareString+".txt", "ESP32 and SD Card \r\n");
//     //writeFile(SD, file+".txt", "ESP32 and SD Card \r\n");
//    
//    }
//    else {
//    Serial.println("File already exists");  
//    }
//    file.close();

    //******************************************
    delay(500);
    Serial.print(". ");
    digitalWrite(19, 0);
    delay(500);
    intavail = true;

   }
     Serial.print("IP address: ");
     Serial.println(WiFi.localIP());
     digitalWrite(19, 1);
     delay(500);
  }
 
void loop() {
  machine_status();
  delay(200);
}

void machine_status() {
 
  DateTime now = rtc.now();

  while(p_statef!=true){
  p = digitalRead(34);
  c = digitalRead(35);
  i = digitalRead(36);
  e = digitalRead(39);
 
  if (p != last_p) {
     p_statef =true;
     e_statef =false;
    if (p == HIGH) {
      p_state = true;
    } else {
      p_state = false;  
    }
    dataString = String(now.unixtime());
   
    Serial.print("Poring:");
    Serial.println(p_state);
    delay(10);
     
   }
   last_p = p;
 
   
  }

  while(c_statef!=true){
  p = digitalRead(34);
  c = digitalRead(35);
  i = digitalRead(36);
  e = digitalRead(39);
  //*************************
 
  if (c != last_c) {
    c_statef =true;
    if (c == HIGH) {
      c_state = true;
    } else {
      c_state = false;
    }
    dataString1 = String(now.unixtime());
   
    Serial.print("Die Closing:");
    Serial.println(c_state);
    delay(10);  
  }
  last_c = c;
  }
   
  while(i_statef!=true){
  p = digitalRead(34);
  c = digitalRead(35);
  i = digitalRead(36);
  e = digitalRead(39);
  //************************
 
  if (i != last_i) {
    i_statef =true;
    if (i == HIGH) {
      i_state = true;
    } else {
      i_state = false;
    }
    dataString2 = String(now.unixtime());
   
    Serial.print("Injection:");  
    Serial.println(i_state);  
    delay(10);  
  }
  last_i = i;
 
  }

  while(e_statef!=true){
  p = digitalRead(34);
  c = digitalRead(35);
  i = digitalRead(36);
  e = digitalRead(39);
  //****************************
 
  if (e != last_e) {
    e_statef =true;
     p_statef =false;
      i_statef =false;
       c_statef =false;
    if (e == HIGH) {
      e_state = true;
    } else {
      e_state = false;
    }
    dataString3 = String(now.unixtime());
   
    Serial.print("Ejection:");  
    Serial.println(e_state);
    delay(10);
  }
  last_e = e;
 }
// m_state_update(dataString,p_state);
// delay(40);
// m_state_update(dataString1,c_state);
//  delay(40);
// m_state_update(dataString2,i_state);
//  delay(40);
// m_state_update(dataString3,e_state);
//  delay(40);

m_state_update(dataString,p_state,dataString1,c_state,dataString2,i_state,dataString3,e_state);
delay(100);

}

// Write the sensor readings on the SD card
void logSDCard() {
  dataMessage =  "Variable Resistor = " + String() + "\n";
  Serial.print("Save data: ");
  Serial.println(dataMessage);
  appendFile(SD, "/data.txt", dataMessage.c_str());
}
// Write to the SD card (DON'T MODIFY THIS FUNCTION)
void writeFile(fs::FS &fs, const char * path, const char * message) {
  Serial.printf("Writing file: %s\n", path);
  File file = fs.open(path, FILE_WRITE);
  if(!file) {
    Serial.println("Failed to open file for writing");
    return;
  }
  if(file.print(message)) {
    Serial.println("File written");
  } else {
    Serial.println("Write failed");
  }
  file.close();
}
// Append data to the SD card (DON'T MODIFY THIS FUNCTION)
void appendFile(fs::FS &fs, const char * path, const char * message) {
  Serial.printf("Appending to file: %s\n", path);
  File file = fs.open(path, FILE_APPEND);
  if(!file) {
    Serial.println("Failed to open file for appending");
    return;
  }
  if(file.print(message)) {
    Serial.println("Message appended");
  } else {
    Serial.println("Append failed");
  }
  file.close();
}

void m_state_update(String a, bool b,String c, bool d,String e, bool f, String g, bool h) {
  if ((WiFi.status() == WL_CONNECTED)) { //Check the current connection status
    HTTPClient http;
    http.begin("http://121.200.55.62/projects/epdc/ucal/api/log.php?mid=101&machine=" + String(a)+String(b)+ String(c)+String(d)+ String(e)+String(f)+ String(g)+String(h)); //Specify the URL
    int httpCode = http.GET();                                        //Make the request

    if (httpCode > 0) { //Check for the returning code
      String payload = http.getString();
      Serial.println(httpCode);
      Serial.println(payload);
    }

    else {
      Serial.println("Error on HTTP request");
    }
    http.end(); //Free the resources
  }
  delay(50);
}

void ack(int a) {
 
}
