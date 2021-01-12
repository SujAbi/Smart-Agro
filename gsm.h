// Universum | Universum Projects > SmartAgro | gprs.h

// Andrei Florian 14/AUG/2019 - 5/JUN/2019

GSMClient client;

char apn[20] = "soracom.io";
char login[8] = "sora";
char password[8] = "sora";

char url[] = "harvest.soracom.io";
char path[] = "/";

void parseData(String dataToSend)
{
  Serial.println("Sending Data");
  Serial.println("________________________________________");

  Serial.println("Sending Data to Soracom");
  Serial.println("  OK - Setting Up Connection");
  if(client.connect(url, 80)) // prepare connection and format send
  {
    Serial.println("  OK - Connection Established, Parsing Data");

    client.println("POST / HTTP/1.1");
    client.println("Host: harvest.soracom.io");
    client.println("User-Agent: Arduino/1.0");
    client.println("Connection: close");
    client.print("Content-Length: ");
    client.println(dataToSend.length());
    client.println("");
    client.println(dataToSend);

    Serial.println("  OK - Data Parsed");
  }

  Serial.println("  OK - Getting Responce");
  Serial.println("");

  // read back from server
  while(1)
  {
    if(client.available()) 
    {
      char c = client.read();
      Serial.print(c);
    }

    if(!client.connected()) 
    {
      break;
    }
  }

  Serial.println("  Success - Data is Parsed");
  Serial.println("________________________________________");
  Serial.println("");
}

// Universum | Universum Projects > SmartAgro | sdmodule.h

// Andrei Florian 14/AUG/2019 - 5/JUN/2019

String legend = "soilTemp, soilHumidity, atmoTemp, atmoHumidity, lightUV, lightVisible, lightIR";

bool createFile()
{
  // create the .csv file in the SD card
  Serial.println("Creating File");
  Serial.println("  OK - Creating and Opening File");
  File dataFile = SD.open("datalog.csv", FILE_WRITE);

  if(dataFile)
  {
    Serial.println("  OK - File Created");
    Serial.println("  OK - Appending Legend to File");
    
    Serial.print("  OK - Appending "); Serial.println(legend);
    dataFile.println(legend);
    dataFile.close();

    Serial.println("  Success - Data Appended");
  }
  else
  {
    Serial.println("  Error - File Not Detected");
    Serial.println("  OK - Trying Again in 5 seconds");

    Serial.println("________________________________________");
    Serial.println("");
    dataFile.close();
    
    delay(5000);
    return false;
  }

  Serial.println("________________________________________");
  Serial.println("");
  return true;
}

void checkFile()
{
  Serial.println("Checking .csv");
  Serial.println("________________________________________");
  
  Serial.println("Setting Up File");
  Serial.println("  OK - Checking for File Presence");

  if(SD.exists("datalog.csv")) // check if the .csv file already exists
  {
    // append to the existing file is it exists
    Serial.println("  OK - File Exists");
    Serial.println("  OK - Will Append to Existing File");
    Serial.println("________________________________________");
    Serial.println("");
  }
  else
  {
    // create a new file to append to
    Serial.println("  OK - File Not Present");
    Serial.println("  OK - Creating File");
    Serial.println("");

    while(!createFile()) {};
  }
}

bool burnData(String data)
{
  Serial.println("Burning Data");
  Serial.println("________________________________________");

  Serial.println("Burning Data to SD Card");
  Serial.println("  OK - Opening File");
  File dataFile = SD.open("datalog.csv", FILE_WRITE);

  if(dataFile)
  {
    Serial.println("  OK - File is Present");
    Serial.print("  OK - Appending "); Serial.println(data);

    Serial.println("  OK - Burning data");
    dataFile.println(data); // burn the data to the SD card
    dataFile.close();
    
    Serial.println("  Success - Data Appended");
    Serial.println("________________________________________");
    Serial.println("");
  }
  else
  {
    Serial.println("  Error - File Not Present");
    Serial.println("  OK - Trying Again in 5 Second");
    Serial.println("________________________________________");
    Serial.println("");
    delay(5000);
  }

  Serial.println("");
}
