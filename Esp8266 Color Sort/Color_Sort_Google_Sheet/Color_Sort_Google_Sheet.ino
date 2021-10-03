#include <LiquidCrystal_I2C.h>
#include <ESP8266WiFi.h>
#include "HTTPSRedirect.h"
#include <Wire.h>
int lcdColumns = 16;
int lcdRows = 2;
LiquidCrystal_I2C lcd(0x27, lcdColumns, lcdRows); // change your I2C ADDRESS HERE EXAMPLE 0x3F
const int s0 = D3;  
const int s1 = D4;  
const int s2 = D5;  
const int s3 = D6;  
const int out = D7;   
int red = 0;  
int green = 0;  
int blue = 0; 
int redcolor= 0;
int greencolor= 0;
int bluecolor= 0;
int yellowcolor= 0;
int color= 0;
// Fill ssid and password with your network credentials
const char* ssid = "hidden";
const char* password = "qwerty12";

const char* host = "script.google.com";
const int httpsPort = 443;
const char *GScriptId = "AKfycbwEanSU2IgzjK3oESUIAzrmwMfyZ1ZH5PjjkqmhhAOUkOl6kmsDF8pImn9hzH69Dtw";

// Write to Google Spreadsheet
String url = String("/macros/s/") + GScriptId + "/exec?tag=adc_A0&value=";

String payload = "";

HTTPSRedirect* client = nullptr;
// used to store the values of free stack and heap
// before the HTTPSRedirect object is instantiated
// so that they can be written to Google sheets
// upon instantiation
void setup() {
  Serial.begin(115200);
  Serial.flush();
    
  Serial.println();
  Serial.print("Connecting to wifi: ");
  Serial.println(ssid);
  Wire.begin(D1,D2);
  // flush() is needed to print the above (connecting...) message reliably, 
  // in case the wireless connection doesn't go through
  Serial.flush();

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  // Use HTTPSRedirect class to create a new TLS connection
  client = new HTTPSRedirect(httpsPort);
  client->setPrintResponseBody(true);
  client->setContentTypeHeader("application/json");
  
  Serial.print("Connecting to ");
  Serial.println(host);

  // Try to connect for a maximum of 5 times
  bool flag = false;
  for (int i=0; i<5; i++){
    int retval = client->connect(host, httpsPort);
    if (retval == 1) {
       flag = true;
       break;
    }
    else
      Serial.println("Connection failed. Retrying...");
  }

  if (!flag){
    Serial.print("Could not connect to server: ");
    Serial.println(host);
    Serial.println("Exiting...");
    return;
  }
  
  payload = "tag=aaaa&value=122";
  client->POST(url, host, payload, false);
  client->GET(url, host); 

  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("ESP Color Finder");

  pinMode(s0, OUTPUT);  
  pinMode(s1, OUTPUT);  
  pinMode(s2, OUTPUT);  
  pinMode(s3, OUTPUT);  
  pinMode(out, INPUT);  
  digitalWrite(s0, HIGH);  
  digitalWrite(s1, HIGH);  
}  
void loop() 

{  
  digitalWrite(s2, LOW);  
  digitalWrite(s3, LOW);  
  //count OUT, pRed, RED  
  red = pulseIn(out, digitalRead(out) == HIGH ? LOW : HIGH);  
  digitalWrite(s3, HIGH);  
  //count OUT, pBLUE, BLUE  
  blue = pulseIn(out, digitalRead(out) == HIGH ? LOW : HIGH);  
  digitalWrite(s2, HIGH);  
  //count OUT, pGreen, GREEN  
  green = pulseIn(out, digitalRead(out) == HIGH ? LOW : HIGH); 
  Serial.print("R Intensity:");  
  Serial.print(red, DEC);  
  Serial.print(" G Intensity: ");  
  Serial.print(green, DEC);  
  Serial.print(" B Intensity : ");  
  Serial.print(blue, DEC);  

  if(red<18 & red>6 & green<32 & green>26 & blue<29 & blue>20){
    delay(250);
    redcolor++;
    String inputVal="Red";
    tranfer(intputVal);
    Serial.print("Red");
    lcd.setCursor(0, 1);
    lcd.print("     Red       ");
    delay(2000);
    lcd.clear();
  }

  if(red<30 & red>26 & green<40 & green>36 & blue<30 &blue>26){
    delay(250);
    bluecolor++;
    String inputVal="Blue"
    tranfer(intputVal)
    Serial.print("Blue");
    lcd.setCursor(0, 1);
    lcd.print("    Blue      ");
    delay(2000);
    lcd.clear();
  }

  if(red<24 & red>20 & green<24 & green>20 & blue<22 & blue>18){
    delay(250);
    greencolor++;
    String inputVal="Green";
    tranfer(intputVal);
    Serial.print("Green");
    lcd.setCursor(0, 1);
    lcd.print("     Green      ");
    delay(2000);
    lcd.clear();
  }

  if(red<16 & red>12 & green<24 & green>20 & blue<26 & blue>22){
    delay(250);
    yellowcolor++;
    String inputVal="Yellow";
    tranfer(intputVal);
    Serial.print("Yellow");
    lcd.setCursor(0, 1);
    lcd.print("     Yellow     ");
    delay(2000);
    lcd.clear();
  } 
  
   Serial.println();  

} 
String tranfer(String intputVal){
  String myString = String(inputVal);  
  String FinalStringToSend;
  FinalStringToSend = url + myString;

  
  if (client != nullptr){
    if (!client->connected()){
      client->connect(host, httpsPort);
      payload= "";
       Serial.println("POST Data to Sheet");
       FinalStringToSend = url + myString;
  Serial.println("POST url :" + FinalStringToSend);
  client->POST(FinalStringToSend, host, payload);
    }
  }
  else{
  Serial.println(" >> Failed to POST data");
  }
  Serial.println("GET url :"+FinalStringToSend);
  client->GET(FinalStringToSend, host);

  delay(500);                          
}
