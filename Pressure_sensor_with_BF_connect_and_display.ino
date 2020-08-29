#include <ESP8266HTTPClient.h>
#include <ESP8266WiFi.h>
//#include <avr/dtostrf.h>
//#include "U8g2lib.h"
//#include <Arduino.h>
#include <U8x8lib.h>


long BF_pause_start = 0;
long WIFI_status = 0;
float PressureValue_raw = 0.0;
float PressureValue_corr = 0.0;
float PressureValue = 0.00;
String PressureValue_string; // Buffer big enough for 7-character float
String PressureValue_LCD;

int WIFI = 0;

char ssid[] = "xxxxxxxx";     //  your network SSID (name)
char pass[] = "xxxxxxxxx";  // your network password

//Pressure sensor calibration coefficients, can be tweaked for each sensor used
const float coeffBar = 0.0037;
const float cal_offset = 0.66;

U8X8_SH1106_128X64_NONAME_HW_I2C u8x8(/* reset=*/ U8X8_PIN_NONE);

#define BF_15min 900000
#define WIFI_retry 300000

void setup() {
  u8x8.begin();                 //OLED init

  u8x8.setFont(u8x8_font_torussansbold8_r);
  u8x8.drawString(0, 0, "                 "); //OLED clear
  u8x8.drawString(0, 1, "                 ");
  u8x8.drawString(0, 2, "                 ");
  u8x8.drawString(0, 3, "                 ");
  u8x8.drawString(0, 4, "                 ");
  u8x8.drawString(0, 5, "                 ");
  u8x8.drawString(0, 6, "                 ");
  u8x8.drawString(0, 7, "                 ");
  u8x8.drawString(13, 7, "bar");

  Serial.begin(115200);                                  //Serial connection
  WiFi.begin(ssid, pass);   //WiFi connection

  while ((WiFi.status() != WL_CONNECTED) && (WIFI < 20)) {  //Wait for the WiFI connection completion, only try 20 times, if no connection it will retry every 5minutes

    delay(500);
    Serial.println("Waiting for connection");
    WIFI ++;
  }

  Serial.print("Connected to IP-adress: ");
  Serial.println(WiFi.localIP());

  //Analog input to trykksensor
  pinMode (A0, INPUT);

//start counters for BF transmit and WIFI connectivity retry
  BF_pause_start = millis();
  WIFI_status = millis();
}

void loop() {

  for (int i = 0; i < 10; i++)  //read pressure value 10 times and average it
  {
    PressureValue_raw += analogRead (A0);
    delay (25);
  }
  PressureValue_raw /= 10;

  Serial.print("PressureValue_raw: ");
  Serial.println(PressureValue_raw);

  

  PressureValue_corr = (PressureValue_raw - 105);

Serial.print("PressureValue_corr: ");
  Serial.println(PressureValue_corr);

  if (int(PressureValue_corr) > 80)   //check that pressure is above zero
  {
    PressureValue = (PressureValue_raw * coeffBar) - cal_offset;
  }

  PressureValue_LCD = String(PressureValue);
  
//Print pressurevalue and 'bar' to display
  u8x8.setFont(u8x8_font_inr46_4x8_n);
  u8x8.setCursor(0, 0);
  u8x8.print(PressureValue_LCD);
  u8x8.setFont(u8x8_font_torussansbold8_r);
  u8x8.drawString(13, 7, "bar");

  if (WiFi.status() != WL_CONNECTED) {  //If no WIFI connection, show a 'x' in the display

    u8x8.setFont(u8x8_font_open_iconic_check_1x1);
 
    u8x8.drawString(5, 3, "\x44");
  }

  PressureValue_string = String(PressureValue);

 
  if ((millis() - WIFI_status) > long(WIFI_retry) && (WiFi.status() != WL_CONNECTED)) {
    WiFi.begin(ssid, pass);
    WIFI_status = millis();
  }

  if ((millis() - BF_pause_start) > long(BF_15min)) {                                                                                                                 //only send every 15min to BF


    if (WiFi.status() == WL_CONNECTED) {                                                                                                                              //Check WiFi connection status

      HTTPClient http;                                                                                                                                                //Declare object of class HTTPClient

      http.begin("http://log.brewfather.net/stream?id=XXXXXXXXXXXXXX");                                                                                               //Specify request destination
      http.addHeader("Content-Type", "application/JSON");                                                                                                             //Specify content-type header

      int httpCode = http.POST("{\r\n  \"name\": \"PressureSensor2000\", \r\n  \"pressure\": " + PressureValue_string + ",\r\n  \"pressure_unit\": \"BAR\"}");

      String payload = http.getString();                                                                                                                              //Get the response payload
      Serial.print("HTTP return code: ");
      Serial.println(httpCode);                                                                                                                                       //Print HTTP return code
      Serial.print("Payload response: ");
      Serial.println(payload);                                                                                                                                        //Print request response payload

      http.end();                                                                                                                                                     //Close connection

    }
    else {

      Serial.println("Error in WiFi connection");

    }
    Serial.println("Waiting 15min until next send");
   
    BF_pause_start = millis();
  }
}
