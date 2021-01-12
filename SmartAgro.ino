// Universum | Universum Projects > SmartAgro

// Andrei Florian 14/AUG/2019 - 5/JUN/2019

#define ARDUINO_MKR;

#include <SD.h>
#include <SPI.h>
#include <Wire.h>
#include <OneWire.h>

#include <DallasTemperature.h>
#include <ArduinoLowPower.h>
#include <Universum_Logo.h>

#include "Adafruit_SI1145.h"
#include "SparkFunHTU21D.h"
#include <MKRGSM.h>

#include "gsm.h"
#include "sdmodule.h"

Adafruit_SI1145 uv = Adafruit_SI1145();
HTU21D gy21;
OneWire oneWire(5);
DallasTemperature soilSens(&oneWire);

GSM gsm;
GPRS gprs;
GSMLocation location;
GSMModem modem;

// Editable Variables
String deviceName = "device1";
int sleepTime = 10000;
bool proDebug = true;

float soilTemp;
float atmoTemp;

int soilHumidity;
int atmoHumidity;

float uvLight;
float rawUVLight;
int visibleLight;
int irLight;

int rawTemp;
int rawSoilTemp;
int rawSoilHum;
int rawUV;

float batteryVoltage;

String compiledTemp;
String compiledSoilTemp;
String compiledSoilHum;
String compiledLight;

float latitude;
float longitude;

void collectData()
{
  Serial.println("Gathering Data");
  Serial.println("________________________________________");
  Serial.println("Getting Data from Sensors");
  
  Serial.println("  OK - Contacting all Sensors");
  // collecting data from all sensors
  soilSens.requestTemperatures();
  soilTemp = soilSens.getTempCByIndex(0);

  soilHumidity = analogRead(A1);
  soilHumidity = map(soilHumidity, 1023, 0, 0, 100);

  atmoTemp = gy21.readTemperature();
  atmoHumidity = gy21.readHumidity();

  visibleLight = uv.readVisible();
  irLight = uv.readIR();
  rawUVLight = uv.readUV();
  uvLight = (rawUVLight / 100);
  
  // --> Update 1.1
  Serial.println("  OK - Collecting Battery Voltage");
  int rawVal = analogRead(ADC_BATTERY);
  float batteryVoltage = rawVal * (3.7 / 1023.0);
  // <-- Update 1.1
  
  Serial.println("  OK - Data Collected");
  Serial.println("  OK - Dumping Data");

  Serial.print("[Light]   Visible       "); Serial.println(visibleLight);
  Serial.print("[Light]   Infrared      "); Serial.println(irLight);
  Serial.print("[Light]   Ultraviolet   "); Serial.println(uvLight);
  
  Serial.print("[Atmo]    Temperature   "); Serial.println(atmoTemp);
  Serial.print("[Atmo]    Humidity      "); Serial.println(atmoHumidity);

  Serial.print("[Soil]    Temperature   "); Serial.println(soilTemp);
  Serial.print("[Soil]    Humidity      "); Serial.println(soilHumidity);

  Serial.print("[System]  Battery Volt  "); Serial.println(batteryVoltage);
  
  Serial.println("  Success - Data Dumped");
  Serial.println("________________________________________");
  Serial.println("");
}

String processData(float soil1, int soil2, float atmo1, int atmo2, float light1, int light2, int light3)
{
  Serial.println("Preparing Data for Burn");
  Serial.println("________________________________________");
  
  Serial.println("Merging Data");
  Serial.println("  OK - Merging All Data to .csv Row");

  // compile all the data into a .csv file line
  String output;
  output += soil1; output += ", "; output += soil2; output += ", ";
  output += atmo1; output += ", "; output += atmo2; output += ", ";
  output += light1; output += ", "; output += light2; output += ", "; output += light3;

  Serial.println("  OK - Data is Merged");
  Serial.println("  OK - Dumping Data to Serial");

  Serial.println(legend);
  Serial.println(output);

  Serial.println("  Success - Data is Merged");
  Serial.println("________________________________________");
  Serial.println("");
  return output;
}

String makeLine()
{
  Serial.println("Preparing Data for Send");
  Serial.println("________________________________________");

  Serial.println("Compiling Data");
  Serial.println("  OK - Preparing String");
  while(!getLocation());

  Serial.println("  OK - Building String");
  // create the JSON buffer to send to Soracom
  String dataReturned; dataReturned += "{"; dataReturned += " \n";
  
  dataReturned += "\"Latitude\":"+ String(location.latitude(), 7); dataReturned += ", \n";
  dataReturned += "\"Longitude\":"+ String(location.longitude(), 7); dataReturned += ", \n";

  dataReturned += "\"soilTemp\":"+ String(soilTemp); dataReturned += ", \n";
  dataReturned += "\"soilHumidity\":"+ String(soilHumidity); dataReturned += ", \n";
  
  dataReturned += "\"atmoTemp\":"+ String(atmoTemp); dataReturned += ", \n";
  dataReturned += "\"atmoHumidity\":"+ String(atmoHumidity); dataReturned += ", \n";
  
  dataReturned += "\"uvLight\":"+ String(uvLight); dataReturned += ", \n";
  dataReturned += "\"irLight\":"+ String(irLight); dataReturned += ", \n";
  
  dataReturned += "\"deviceName\":"; dataReturned += String("\""); dataReturned += String(deviceName); dataReturned += String("\""); dataReturned += ", \n";
  dataReturned += "\"batteryVoltage\":"+ String(batteryVoltage); dataReturned += "\n";
  
  dataReturned += "}";
  
  Serial.println("  OK - Data is below");
  Serial.println(""); 
  Serial.println(dataReturned);
  Serial.println(""); 
  
  Serial.println("  Success - String is Ready");
  Serial.println("________________________________________");
  Serial.println("");
  
  return dataReturned; 
}

void prepareSample() // runs when the device wakes up
{
  Serial.println("");
  Serial.println("****************************************");
  Serial.println("");
}

bool getLocation()
{
  if(location.available()) // get the location and append it to global variables
  {
    latitude = location.latitude();
    longitude = location.longitude();

    delay(500);
    return true;
  }
  else
  {
    delay(500);
    return false;
  }
}

bool checkLocation(float latitude, float longitude)
{
  // convert location floats to integers
  int rawLat = latitude;
  int rawLng = longitude;

  if(location.available())
  {
    // if the value of the decimal is equal to the integer, the location is not precise
    if(rawLat == latitude && rawLng == longitude) // location is not precise
    {
      Serial.print(".");
      return false;
    }
    else
    {
      Serial.print(".");
      return true;
    }
  }
  else
  {
    return false;
  }
}

void setup()
{
  Serial.begin(9600);

  if(proDebug)
  {
    while(!Serial) {};
    logoStart("SmartAgro");
  }

  Serial.println("Initialising SI1145 Module");
  if(!uv.begin())
  {
    Serial.println("  Error - Module Failed to Initialise");
    while(1) {};
  }
  Serial.println("  Success - Module Online");

  Serial.println("Initialising GY21 Module");
  gy21.begin();
  Serial.println("  Success - Module Online");

  Serial.println("Initialising Temperature Module");
  soilSens.begin();
  Serial.println("  Success - Temperature Sensor Online");

  Serial.println("Initialising SD Module");
  if(!SD.begin(4)) 
  {
    Serial.println("  Error - SD Module Responded with Error");
    while(1) {};
  }
  Serial.println("  Success - Module Online");
  Serial.println("");

  Serial.println("Initialising SIM Card");
  bool connected = false;
  
  while(!connected)
  {
    if((gsm.begin() == GSM_READY))
    {
      connected = true;
    }
    else
    {
      Serial.println("  Error - SIM Card Initialisation Failed");
      Serial.println("  OK - Trying again in 1 Second");
      delay(1000);
    }
  }

  Serial.println("Attaching GPRS Module");
  while(gprs.attachGPRS(apn, login, password) == GPRS_READY) 
  {
    Serial.println("  Error - GPRS Module Responded with Error");
    delay(500);
  }
  Serial.println("  Success - Module Attached");
  
  Serial.println("Attaching Lcation Services");
  location.begin();
  Serial.println("  OK - Services Online");

  Serial.println("  OK - Getting Location Fix");
  Serial.println("  Warning - This may take a couple of minutes");

  Serial.print("  ");
  while(!checkLocation(location.latitude(), location.longitude())) // wait until a lock is achieved
  {
    delay(500);
  }

  Serial.println("");
  Serial.println("  Success - Location is Fixed");

  Serial.println("Attaching Interrupt");
  LowPower.attachInterruptWakeup(RTC_ALARM_WAKEUP, prepareSample, CHANGE);
  Serial.println("  Success - Interrupt Attached");
  
  Serial.println("");
  Serial.println("");
}

void loop()
{  
  collectData(); // append the values from sensors to global variables
  String dataToBurn = processData(soilTemp, soilHumidity, atmoTemp, atmoHumidity, uvLight, visibleLight, irLight); // compile the data into .csv line
  
  while(!burnData(dataToBurn)) {}; // burn the data to the SD card
  String dataToSend = makeLine(); // compile the data into a JSON buffer

  parseData(dataToSend); // send the buffer to Soracom
  delay(500);
  LowPower.sleep(sleepTime); // sleep between reads
}
