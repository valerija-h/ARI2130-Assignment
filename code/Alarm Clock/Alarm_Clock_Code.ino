
#include <Wire.h>
#include <LiquidCrystal.h> //Library for the LCD display.
#include "RTClib.h" //Library for the RTC module - had to download from https://www.arduinolibraries.info/libraries/rt-clib.

#define SIG A0

//Digital Pins and States for the buttons.
const int buttonLeft = 8;
const int buttonMid = 9;
const int buttonRight = 10;
int buttonStateLeft = 0;
int buttonStateRight = 0;
int buttonStateMid = 0;

//Digit Pin for the ultrasonic.
const int pingPin = 6;

//Digital Pin for the buzzer.
int buzzerPin = 13;

//Using the Digital Pins to set up the LCD screen.
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

//Variables and functions needed for the RTC.
RTC_DS3231 rtc;
DateTime now; //Stores the current date.
void showDate(void);
void showTime(void);
void setAlarm(void);
void resetAlarm(void);
void showtAlarm(void);
bool settingAlarm = false; //Setting alarm clock mode.
int hourSet = -1; //The set alarm hour.
int minSet = -1; //The set alarm minute.
int currentHour = 0; //Keeps track of the hour when setting the alarm.
int currentMin = 0; //Keeps track of the min when setting the alarm.


void setup ()
{
  Serial.begin(9600); // Starting Serial Terminal.
  // Initalizing the LCD screen.
  lcd.begin(16,2);
  // Initializing the buttons.
  pinMode(buttonLeft, INPUT);
  pinMode(buttonMid, INPUT);
  pinMode(buttonRight, INPUT);
  // Initialzing the buzzer.
  pinMode(buzzerPin, OUTPUT);
  // Initializes the RTC.
  rtc.begin();
}

void loop () 
{
   // Check input of buttons.
   buttonStateLeft = digitalRead(buttonLeft);
   buttonStateMid = digitalRead(buttonMid);
   buttonStateRight = digitalRead(buttonRight);

  //ALARM MODE - Check if the alarm is set and the button hasnt been pressed and the hour and min are valid.
  if(now.hour() == hourSet && (now.minute() == minSet || now.minute() == minSet+1 || now.minute() == minSet+2)){
      lcd.setCursor(0,0);
      lcd.print("Wakey Wakey!");
      lcd.print("   "); //To clear previous line.
      lcd.setCursor(0,1);
      lcd.print("Press Center.");
      lcd.print("   "); //To clear previous line.
    //Stop alarm by pressing the middle button.
    if(buttonStateMid == 1){
      hourSet = -1; //Set alarm and minutes back to 0.
      minSet = -1;
      delay(1000);
      Serial.write("stop\n");
      Serial.flush();
    } else {
      //buzzer(); //Sound of the buzzer.
      String distance = (String) getReading();
      //Running the car code.
      Serial.write("start ");
      Serial.print(distance);
      Serial.write("\n");
      Serial.flush();
      //Sending and raspberry pi.
    }
  } else {
    //If setting Alarm mode isn't on - DEFAULT MODE.
    if(settingAlarm == false)
    {
      now = rtc.now(); //Get current date and time.
      showDate(); //Shows the date and time.
      showTime();
  
      //Left button - SETTING ALARM MODE.
      if(buttonStateLeft == 1)
      {
        settingAlarm = true;
        resetAlarm();
        lcd.clear();
        delay(500);
      //Right button - RESET ALARM.
      } else if(buttonStateRight == 1){
        lcd.clear();
        resetAlarm();
        lcd.setCursor(0,0);
        lcd.print("Alarm has been");
        lcd.setCursor(0,1);
        lcd.print("reset.");
        delay(1000);
      //SHOW ALARM.
      } else if(buttonStateMid == 1){
        showAlarm();
      }
    } else {
      setAlarm();
    }
  }
}

//This function is in charge of setting the time for the alarm.
void setAlarm(){
  //Setting the hour.
    if(hourSet == -1){
      lcd.setCursor(0,0);
      lcd.print("Setting Hour...");
      lcd.setCursor(0,1);
      addExtra(currentHour); //Add Leading Zeros.
      lcd.print(currentHour);
      lcd.print(":00");
      lcd.print("        "); //To clear previous line.
      //If you press the middle button again. Sets the hour.
      if(buttonStateMid == 1)
      {
        hourSet = currentHour;
        currentHour = 0;
        delay(500);
        //Decreasing the hour.
      } else if(buttonStateLeft == 1){
        if(currentHour == 0){
          currentHour = 24;
        } else {
          currentHour--;
        }
        delay(200);
        //Increasing the hour.
      } else if(buttonStateRight == 1){
        if(currentHour == 24){
          currentHour = 0;
        } else {
          currentHour++;
        }
        delay(200);
      }
    } else if (minSet == -1){
      lcd.setCursor(0,0);
      lcd.print("Setting Mins...");
      lcd.setCursor(0,1);
      addExtra(hourSet); //Add Leading Zeros.
      lcd.print(hourSet);
      lcd.print(":");
      addExtra(currentMin); //Add Leading Zeros.
      lcd.print(currentMin);
      lcd.print("        "); //To clear previous line
      //If you press the middle button again. Sets the hour.
      if(buttonStateMid == 1)
      {
        settingAlarm = false; //To exit alarm setting mode.
        minSet = currentMin;
        currentMin = 0;
        delay(500);
        //Decreasing the hour.
      } else if(buttonStateLeft == 1){
        if(currentMin == 0){
          currentMin = 59;
        } else {
          currentMin--;
        }
        delay(200);
        //Increasing the hour.
      } else if(buttonStateRight == 1){
        if(currentMin == 59){
          currentMin = 0;
        } else {
          currentMin++;
        }
        delay(200);
      }
    }
 }


//Resetting the alarm times.
void resetAlarm(){
  //If hourSet and hourmin have values set it to null.
  if(hourSet != -1 || minSet != -1){
      hourSet = minSet = -1;
  }
 }

//Show the alarm the user has set.
void showAlarm(){
  lcd.clear();
  lcd.setCursor(0,0);
  if(hourSet != -1 && minSet != 1){
    lcd.print("Alarm set for:");
    lcd.setCursor(0,1);
    addExtra(hourSet); //Add Leading Zeros.
    lcd.print(hourSet);
    lcd.print(":");
    addExtra(minSet); //Add Leading Zeros.
    lcd.print(minSet);
  } else {
    lcd.print("No alarm has");
    lcd.setCursor(0,1);
    lcd.print("been set.");
  }
  delay(2000);
} 
 
void showDate()
 {
  lcd.setCursor(0,0);
  lcd.print("Date:");
  addExtra(now.day()); //Add Leading Zeros.
  lcd.print(now.day());
  lcd.print('/');
  addExtra(now.month()); //Add Leading Zeros.
  lcd.print(now.month());
  lcd.print('/');
  addExtra(now.year()); //Add Leading Zeros.
  lcd.print(now.year());
  lcd.print("   ");
 }

 void showTime()
 {
  lcd.setCursor(0,1);
  lcd.print("Time:");
  addExtra(now.hour()); //Add Leading Zeros.
  lcd.print(now.hour());
  lcd.print(':');
  addExtra(now.minute()); //Add Leading Zeros.
  lcd.print(now.minute());
  lcd.print(':');
  addExtra(now.second()); //Add Leading Zeros.
  lcd.print(now.second());
  lcd.print("   ");
 } 

//Makes the buzzer sound go on and off.
 void buzzer(){
  tone(buzzerPin, 523, 1000);
 }

 void addExtra(int var){
  if(var < 10){
    lcd.print('0');
  }
 }

int getReading(){
  unsigned long rxTime;
  float distance;
  pinMode(SIG, OUTPUT);

  digitalWrite(SIG, HIGH);
  delayMicroseconds(20);
  digitalWrite(SIG, LOW);
  pinMode(SIG, INPUT);
  rxTime = pulseIn(SIG, HIGH);
  distance = (float) rxTime * 34 /2000.0;
  if(distance < 2){
    distance = 0;
  }
  if(distance > 800){
    distance = 0;
  }
  delay(10);
  return round(distance);
}
