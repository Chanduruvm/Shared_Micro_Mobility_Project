/* -----------------------------------------------------------------------------
  - Project: Development of Shared E- Micro Mobility with IoT 
  - Author:  Chanduru V M
  - Date:  6/11/2021
   -----------------------------------------------------------------------------
  This code was created by HyloMobility for 
  the RFID communicate with  database using NodeMCU.
   ---------------------------------------------------------------------------*/
//*******************************libraries********************************
//-----RFID-----------------------------
#include <SPI.h>
#include <MFRC522.h>
//***#include <MFRC522_I2C.h>  --for nfc
#include <LiquidCrystal_I2C.h>
#include <Wire.h>
//---NodeMCU--------------------------
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
//*****************************************************

/*
************ nfc rfid files****************
#include <PN532_I2C.h>
#include <PN532.h>
#include <NfcAdapter.h>
PN532_I2C pn532_i2c(Wire);
NfcAdapter nfc = NfcAdapter(pn532_i2c);
String CardID = "";
byte nuidPICC[4];
*/
//************************************************************************
#define SS_PIN D8  //D8  pin
#define RST_PIN D3  //D3  pin
//************************************************************************
MFRC522 mfrc522(SS_PIN, RST_PIN); // Create MFRC522 instance.  normal 
//MFRC522 mfrc522(0x3C, RST); // nfc i2c 
LiquidCrystal_I2C lcd(0x27, 16, 2);

const int pin = D4;
//************************************************************************
/* Set these to your desired wifi credentials. */
//const char *ssid = "hylomob";
//const char *password = "hylo";
const char *ssid = "chanduru";
const char *password = "123456789";
//const char *ssid = "OnePlus 5";
//const char *password = "Babydoll";
//const char *ssid = "VIVEK VAZIRANI";
//const char *password = "anandms05";
const char* device_token  = "c0add30bc816dfec";
//************************************************************************
String URL = "http://thangamgroups.com/getdata.php"; //computer IP or the server domain 162.241.123.127
String getData, Link;
String OldCardID = "";
String savecardid = "";
unsigned long previousMillis = 0; 


//************************************************************************
void setup() {
  delay(1000);
  Serial.begin(115200);
  pinMode(pin, OUTPUT);
  digitalWrite(pin, HIGH);
  SPI.begin();  // Init SPI bus
   //nfc.begin();
  mfrc522.PCD_Init(); // Init MFRC522 card

  lcd.init();
    
  lcd.noBacklight();
  
 
  
  //---------------------------------------------
  connectToWiFi();
  /*lcd.setCursor(0,0);
   lcd.print("Hylomo Welcome u");
   lcd.setCursor(1,1);
   lcd.print("Tap the card");
   */
}
//************************************************************************
void loop() {
  //check if there's a connection to Wi-Fi or not
  if(!WiFi.isConnected()){
    connectToWiFi();    //Retry to connect to Wi-Fi
  }
  //---------------------------------------------
  if (millis() - previousMillis >= 15000) {
    previousMillis = millis();
    OldCardID="";
   
  }
  delay(50);


   //readNFC();
 
  //---------------------------------------------
  //look for new card
  if ( ! mfrc522.PICC_IsNewCardPresent()) {
    return;//got to start of loop if there is no card present
  }
  // Select one of the cards
  if ( ! mfrc522.PICC_ReadCardSerial()) {
    return;//if read card serial(0) returns 1, the uid struct contians the ID of the read card.
  }
  String CardID ="";
  for (byte i = 0; i < mfrc522.uid.size; i++) {
    CardID += mfrc522.uid.uidByte[i];
  }


  
  //---------------------------------------------
  if( CardID == OldCardID ){
    return;
  }
  else{
    OldCardID = CardID;
  }
  //---------------------------------------------


if( savecardid == "" ){
   Serial.println("savecard id in start");
  Serial.println(savecardid);
      savecardid=CardID;
       Serial.println("after add savecard to cardid");
     SendCardID(CardID);
     //Serial.println(savecardid);
  delay(1000);
  return;
}
  else if(CardID == savecardid){
    SendCardID(CardID);
     Serial.println("same card coming");
      delay(1000);
      savecardid = "";
Serial.println("same card coming after save card value");
//Serial.println(savecardid);
      return;
  }
  else{
    Serial.println("already one card is used");
    lcd.backlight();
        lcd.clear();
    lcd.setCursor(1,0);
   lcd.print("Sorry already");
   lcd.setCursor(2,1);
   lcd.print("I'm in use!");
   delay(3000);
   lcd.clear();
   lcd.setCursor(1,0);
   lcd.print("HYLOMOBILITY");
   
   return;
   
  }



  
// Serial.println(CardID);
// Serial.println("OldCardID=");
  //Serial.println(OldCardID);
 // SendCardID(CardID);
 // delay(1000);







  
}
//************send the Card UID to the website*************
void SendCardID( String Card_uid ){
  Serial.println("Sending the Card ID");
  if(WiFi.isConnected()){
    HTTPClient http;    //Declare object of class HTTPClient
    //GET Data
    getData = "?card_uid=" + String(Card_uid) + "&device_token=" + String(device_token); // Add the Card ID to the GET array in order to send it
    //GET methode
    Link = URL + getData;
    http.begin(Link); //initiate HTTP request   //Specify content-type header
    
    int httpCode = http.GET();   //Send the request
    String payload = http.getString();    //Get the response payload

  Serial.println(Link);   //Print HTTP return code
    Serial.println(httpCode);   //Print HTTP return code
    Serial.println(Card_uid);     //Print Card ID
    Serial.println(payload);    //Print request response payload

   
       
    if (httpCode == 200) {
      if (payload.substring(0, 5) == "login") {
        String user_name =  "welcome " + payload.substring(5);
        Serial.println(user_name);
        digitalWrite(pin, LOW);
        String uname = payload.substring(5);
             lcd.clear(); 
          lcd.backlight();   
         lcd.setCursor(0,0);
   lcd.print("HYLOMOBILITY");
   lcd.setCursor(2,1);
   lcd.print("Welcomes You!!");
   delay(2000);
        lcd.clear(); 
         lcd.setCursor(0,0);
         lcd.print("Happy Journey!!");
   lcd.setCursor(0,1);
   lcd.print(payload.substring(5));
   

      }
      else if (payload.substring(0, 6) == "logout") {
        String user_name = " Thanks for Ride!" + payload.substring(6);
        Serial.println(user_name);
        String uname = payload.substring(6);
        digitalWrite(pin, HIGH);
        savecardid = "";
        lcd.clear(); 
         lcd.backlight();
         lcd.setCursor(0,0);
   lcd.print("Thanks for Ride!");
   lcd.setCursor(2,1);
   lcd.print(uname);
    delay(4000);
    lcd.clear();
     lcd.noBacklight();
    /*lcd.setCursor(0,0);
   lcd.print("Hylomo Welcome u");
   lcd.setCursor(1,1);
   lcd.print("Tap the card");
   */
  

        
      }
      else if (payload == "succesful") {

      }
      else if (payload == "available") {

      }
      else if (payload == "Not found!") {
        Serial.println(payload); 
        lcd.clear(); 
         lcd.backlight();
         lcd.setCursor(1,0);
   lcd.print("Card Invalid!!");
   savecardid = "";
    delay(4000);
    lcd.clear();
      lcd.noBacklight();

      }
      delay(100);
      http.end();  //Close connection
    }
  }
}

/*void readNFC() 
{
 if (nfc.tagPresent())
 {
   NfcTag tag = nfc.read();
   tag.print();
   tagId = tag.getUidString();
 }
 delay(5000);
}*/
//********************connect to the WiFi******************
void connectToWiFi(){
    WiFi.mode(WIFI_OFF);        //Prevents reconnection issue (taking too long to connect)
    delay(1000);
    WiFi.mode(WIFI_STA);
    Serial.print("Connecting to ");
    Serial.println(ssid);
    WiFi.begin(ssid, password);
    
    while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
    }
    Serial.println("");
    Serial.println("Connected");

    lcd.clear(); 
         lcd.backlight();
         lcd.setCursor(1,0);
   lcd.print("connected");
    delay(4000);
    lcd.clear();
      lcd.noBacklight();
  
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());  //IP address assigned to your ESP
    
    delay(1000);
}
//=======================================================================
