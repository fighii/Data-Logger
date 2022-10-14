//--------------Library------------------
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include "RTClib.h"
#include <Adafruit_INA219.h>
#include <SPI.h>
#include <SD.h>
#include <EEPROM.h>



//--------------RTC & SD CARD -------------
RTC_DS1307 RTC;
File logfile;
const int chipSelect = 53;

//--------------LCD-------------
LiquidCrystal_I2C lcd(0x27, 20, 4);

//-----------Speed Sensor------------
const byte speedPin       = 18;
volatile byte tick;
unsigned int rpm;
unsigned int last_rpm;



//------------Flow Sensor-------------
const byte flowPin        = 2;
float calibrationFactor = 4.5;
volatile byte pulseCount = 0;


float flowRate  = 0;
float flow      = 0;
float velocity  = 0;


//-----------Ampere Voltage Sensor----------
Adafruit_INA219 ina219;

float busvoltage  = 0;
float current_mA  = 0;
float power_mW    = 0;

//------------------Variabel------------------------
void(* resetFunc) (void) = 0;

unsigned long last_samplingAV = 0;
unsigned long last_samplingFR = 0;
unsigned long last_samplingSpeed;

unsigned long last_serial     = 0;
unsigned long last_write      = 0;
unsigned long data_count      = 1;
unsigned long last_pressed    = 0;
unsigned long last_notPressed = 0;

bool displayMode = 0;
int write_time;

struct MyObject {
  float river_area;
};

//----------------Addres Variable-----------------
int area_addr = 0;
int writeTime_addr = 1;



String formatData = "No,Date,Time,Flow,Velocity,Voltage,Current,RPM";

void setup() {

  Serial.begin(9600);



  //----------------Setup LCD-------------------------
  lcd.init();
  lcd.backlight();
  lcd.setCursor(4, 1);
  lcd.print("Data Logger");
  delay(3000);
  lcd.clear();
  lcd.setCursor(5, 1);
  lcd.print("Prepare");
  lcd.setCursor(3, 2);
  lcd.print("Data Logger");
  delay(2000);




  //-------------------SD Card-------------------------
  Wire.begin();
  RTC.begin();
  if (! RTC.isrunning()) {
    Serial.println("RTC is NOT running!");
    RTC.adjust(DateTime(F(__DATE__), F(__TIME__)));
  }
  
  if (!SD.begin(chipSelect)) {
    while (1);
  }





  //---------------Setup Flow Sensor---------------------
  pinMode(flowPin, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(flowPin), pulseCounter, FALLING);






  //-------------Setup AmpereVoltage Sensor-------
  if (! ina219.begin()) {
    //    Serial.println("Failed to find INA219 chip");
    while (1) {
      delay(10);
    }
  }


  //-------------Setup Speed Sensor--------------------
  pinMode(speedPin, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(speedPin), speedCount, RISING);





  //------------------Button Pin----------------------

  for (int x = 22; x < 29; x += 2) {
    pinMode(x, INPUT_PULLUP);
  }
  digitalWrite(23, HIGH);
  pinMode(23, OUTPUT);




  //-------------Setup Memory Card-------------------
  char filename[] = "LOGGER00.CSV";
  for (uint8_t i = 0; i < 100; i++) {
    filename[6] = i / 10 + '0';
    filename[7] = i % 10 + '0';
    if (! SD.exists(filename)) {
      // only open a new file if it doesn't exist
      logfile = SD.open(filename, FILE_WRITE);
      break;  // leave the loop!
    }
  }

  if (! logfile) {
    error("couldnt create file");
  }




  //------------Setup Title CSV File-------------------
  logfile.print("No");
  logfile.print(",");
  logfile.print("Date (yyyy/m/d)");
  logfile.print(",");
  logfile.print("Time (HH:MM:SS)");
  logfile.print(",");
  logfile.print("Flow (m3/s)");
  logfile.print(",");
  logfile.print("Velocity (m/s)");
  logfile.print(",");
  logfile.print("Volt (V)");
  logfile.print(",");
  logfile.print("Ampere (A)");
  logfile.print(",");
  logfile.println("RPM (r/m)");
  logfile.flush();



  //----------------If All Okey-------------------------
  write_time = EEPROM.read(writeTime_addr);
  area_addr = sizeof(float);
  MyObject var;
  EEPROM.get( area_addr, var );

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Data Logger Is Ready");
  lcd.setCursor(0, 1);
  lcd.print("Saving To");
  lcd.setCursor(0, 2);
  lcd.print(filename);
  delay(5000);
  lcd.clear();
  lcd.setCursor(2, 0);
  lcd.print("Writing Time:");
  lcd.setCursor(6, 1);
  lcd.print(write_time);
  lcd.print(" s   ");
  lcd.setCursor(2, 2);
  lcd.print("River Area:");
  lcd.setCursor(6, 3);
  lcd.print(var.river_area);
  lcd.print(" m2   ");
  delay(5000);
  lcd.clear();
  Serial.println(formatData);
}


















void loop() {
  //--------------------Get Menu----------------------
  String str = get_button();

  if (str.length() > 1)
  {
    if (str) {
      if (str == "Right") {
        setting_area();
        setting_writeTime();
      }
      if (str == "Down") {
        resetFunc();
      }
      if (str == "Left") {

      }
      if (str == "Up") {
        if (millis() - last_pressed > 500) {
          last_pressed = millis();
          lcd.clear();
          delay(500);
          displayMode = !displayMode;
          if (displayMode == 0) {
            last_serial = millis();
            lcd.setCursor(0, 0);
            lcd.print("F: ");
            lcd.print(flow);
            lcd.print(" m3/s        ");

            lcd.setCursor(0, 1);
            lcd.print("V: ");
            lcd.print(busvoltage);
            lcd.print(" V");

            lcd.setCursor(0, 2);
            lcd.print("I: ");
            lcd.print(abs(current_mA));
            lcd.print(" A");

            lcd.setCursor(0, 3);
            lcd.print("RPM: ");
            lcd.print(rpm);
            lcd.print(" rpm   ");
          }
          if (displayMode == 1) {
            last_serial = millis();
            lcd.setCursor(0, 0);
            lcd.print("Vl: ");
            lcd.print(velocity);
            lcd.print(" m/s      ");

            lcd.setCursor(0, 1);
            lcd.print("V: ");
            lcd.print(busvoltage);
            lcd.print(" V");

            lcd.setCursor(0, 2);
            lcd.print("I: ");
            lcd.print(abs(current_mA));
            lcd.print(" A");

            lcd.setCursor(0, 3);
            lcd.print("RPM: ");
            lcd.print(rpm);
            lcd.print(" rpm   ");
          }
        }

      }
    }
  }
  //-------------------Sampling data------------------
  String all_dataString = "";
  DateTime now = RTC.now();

  readFlow();
  readAmpereVoltage();
  readSpeed();
  lcdPrint();


  if (millis() - last_write > (1000 * write_time)) { //---------------ready for write to sd card-------------

    last_write = millis();

    all_dataString =

      //----------Date-----------
      String(data_count)             +
      String(',') +
      String(now.year(), DEC)   +
      String('/') +
      String(now.month(), DEC)  +
      String('/') +
      String(now.day(), DEC)    +
      String(',') +

      //----------Time-----------
      String(now.hour(), DEC)   +
      String(':') +
      String(now.minute(), DEC) +
      String(':') +
      String(now.second(), DEC) +
      String(',') +

      //----------Data----------
      String(flow)     +
      String(',') +
      String(velocity)     +
      String(',') +
      String(busvoltage)        +
      String(',') +
      String(abs(current_mA))   +
      String(',') +
      String(rpm);

    if (logfile) {
      logfile.println(all_dataString);
      logfile.flush();
      Serial.println(all_dataString);
      data_count++;
    }
    else {
      Serial.println("error opening LOGGER.csv");
      lcd.clear();
      lcd.setCursor(2, 1);
      lcd.print("Error SD Card");
      lcd.setCursor(0, 2);
      lcd.print("Reinstall SD Card");
      while (true) {
        String str = get_button();
        if (str.length() > 1){
          break;
        }
      }
    }
  }
}
