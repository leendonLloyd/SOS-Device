//GSM, 2 = Rx Pin, 3 = Tx Pin,
//For Changing Recipient Number, Send SET RECIP,639********
//authorizedNumber variable is for Admin use, open for change

#include <SoftwareSerial.h>
#include <TinyGPS++.h>
#include <SIM900.h>
#include <sms.h>
SMSGSM sms;

#define GSM_Pin ""
SoftwareSerial gpsSerial(8, 9);
TinyGPSPlus gps;

const int button = 10;
const int buzzer = 8;
const byte numChars = 30;
char smsRecieved[numChars];
char senderNumber[15], recipientNumber[15] = "639453440491", newRecipient[15], commandkey[30] = {0};
char SmsText[30];
boolean validRecip, GSMconnected = false;
int buttonState = 0;

void setup()
{
  Serial.begin(9600);
  Serial.println("Micro_GSM_Project");
  Serial.print("Initializing Serial:");
  gpsSerial.begin(9600);
  gpsSerial.listen();
  pinMode(button, INPUT);
  pinMode(buzzer, OUTPUT);
  while (!Serial)
  {
    Serial.println("Serial not connected"); // wait for serial port to connect.
  }
  delay(500);
  Serial.println(" Done");
  delay(1000);
  Serial.print("Initializing GSM Module:");
  if (gsm.begin(2400)) {
    Serial.println("\nWaiting for messages");
    GSMconnected = true;
    delay(1000);
  }
  else Serial.print("GSM Connecting");
}

void loop() {
  tone(buzzer, 1000);
  delay(100);
  noTone(buzzer);
  delay(50);
  tone(buzzer, 500);
  delay(100);
  buttonState = digitalRead(button);
  if (buttonState == HIGH)
  {
    activate();
  }
  if (GSMconnected)
    {
    buttonState = digitalRead(button);
    if (buttonState == HIGH)
    {
      activate();
    }
    if (gsm.readSMS(SmsText, 30, senderNumber, 16))
    {
      Serial.print("\nMessage received from: +");
      Serial.println(senderNumber);
      Serial.println(SmsText);
      parseData();

     if (strcmp(commandkey, "CURRENT RECIP") == 0) 
        {
         // currentRecip();
        }
     else if (strcmp(commandkey, "SET RECIP") == 0) 
        {
          changeRecip();
          confirmation();
        }
     else
     Serial.println("Invalid Command");
     sms.SendSMS(senderNumber, "The command you sent is invalid");
    }
  }
}

String getCoordinates() {
  Serial.println(F("Obtaining GPS data..."));
  display.println(F("Tracking Location..."));
  bool coordinatesValid = false;
  String coordinates = "";
  gpsSerial.listen();
  while (!coordinatesValid) {
    if (gpsSerial.available()) {
      if (gps.encode(gpsSerial.read())) {
        if (gps.location.isValid()) {
          coordinates += String(gps.location.lat(), 6);
          coordinates += ",";
          coordinates += String(gps.location.lng(), 6);
          coordinatesValid = true;
        }
        delay(1000);
      }
    }
  }
  return coordinates;
}

void activate() {
  Serial.println(buttonState);
  delay(800);
  Serial.println("I'm in dire need of assistance, please send help");
  sms.SendSMS(recipientNumber,"I'm in dire need of assistance, please send help. http://www.google.com/maps/place/" + coordinates);
  Serial.println("\nRecipient_Number");
  Serial.println(recipientNumber);
  Serial.println("Sender_Number");
  Serial.println(senderNumber);
  Serial.println("\nMessage Sent");
}

void parseData() {
  char * strtokIndx;
  strtokIndx = strtok(SmsText, ",");  //Read first part of Command either "SET RECIP" or "CURRENT RECIP"
  strcpy(commandkey, strtokIndx);         //store to commandkey variable

  strtokIndx = strtok(NULL, ",");         //Read where the last call left off, separated by a comma
  strcpy(newRecipient, strtokIndx);       //store to newRecipient variable

  Serial.print("Command Selected:");
  Serial.println(commandkey);
}

void changeRecip() 
    {
  validate();
    if (validRecip == true) 
    {
    strcpy(recipientNumber, newRecipient);    //copy newRecipient String to recipientNumber String
    sms.SendSMS(senderNumber, "The Recipient number you have entered is correct");
    Serial.println("\nRecipient_Number");
    Serial.print(recipientNumber);
    Serial.println("a");
    Serial.println("Sender_Number");
    Serial.println(senderNumber);
    Serial.println("\n");
    }
    if (validRecip == false) 
    {
      Serial.println("Invalid Recipient");
      sms.SendSMS(senderNumber, "The Recipient number you have entered is invalid");     //uncomment for SMS, need load
    }
    }

void validate() 
  {
  if (strlen(newRecipient) == 13)   // compare string lengths
  {       
    Serial.println("Recipient is Valid\n");
    validRecip = true;
  }
  else
  {
    validRecip = false;
  }
}

void confirmation() 
{
    if (validRecip == true)
    {
    Serial.println("Recipient Number successfully changed");    //Serial Monitor confirmation (for troubleshooting)
    Serial.print("New Recipient: +");
    Serial.println(recipientNumber);
    Serial.println("Informing Admin...");
    }
    if (sms.SendSMS(senderNumber, "Recipient Number successfully changed"))
    {  
    Serial.println("Message Sent\n\n");
    }
    else 
    {
    Serial.println("Invalid Recipient");
    sms.SendSMS(senderNumber, "The Recipient number you have entered is invalid");     //uncomment for SMS confirmation, need load
    }
}
