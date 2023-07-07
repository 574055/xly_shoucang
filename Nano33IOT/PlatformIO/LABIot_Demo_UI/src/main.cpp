#include <Arduino.h>

#include <FlashStorage_SAMD.h>
//#define EEPROM_EMULATION_SIZE     (4 * 1024)

// Use 0-2. Larger for more debugging messages
#define FLASH_DEBUG 0

const uint16_t WAITTIME = 15000;

void waitSerial(uint16_t waittime);
void updateData();
void displaySavedData();

const int WRITTEN_SIGNATURE = 0xBEEFDEED;

// Create a structure that is big enough to contain a name
// and a surname. The "valid" variable is set to "true" once
// the structure is filled with actual data for the first time.
typedef struct
{
  char SSID[30];
  char PWD[30];
  char SCALEFACTOR[10];
} EEPROM_data;

// Check signature at address 0
int signature;

// Create a "EEPROM_data" variable and call it "e_data"
uint16_t storedAddress = 0;
EEPROM_data e_data;

void setup()
{
  Serial.begin(115200);
  waitSerial(WAITTIME);

  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);

  EEPROM.get(storedAddress, signature);

  // If the EEPROM is empty then no WRITTEN_SIGNATURE
  if (signature == WRITTEN_SIGNATURE) //if something is in Flash
  {
    EEPROM.get(storedAddress + sizeof(signature), e_data);

    displaySavedData();
    ;

    updateData();
  }
  else
  {

    EEPROM.put(storedAddress, WRITTEN_SIGNATURE);

    String SSID = "LSK 7028";
    String PWD = "wesharetechnology";
    String SCALEFACTOR = "200";

    // Fill the "e_data" structure with the data entered by the user...
    SSID.toCharArray(e_data.SSID, 30);
    PWD.toCharArray(e_data.PWD, 30);
    SCALEFACTOR.toCharArray(e_data.SCALEFACTOR, 10);

    // ...and finally save everything into emulated-EEPROM
    EEPROM.put(storedAddress + sizeof(signature), e_data);

    if (!EEPROM.getCommitASAP())
    {
      Serial.println("CommitASAP not set. Need commit()");
      EEPROM.commit();
    }

    displaySavedData();

    updateData();
  }

  Serial.print("Do nothing...");
}

void loop()
{
  Serial.print(".");
  digitalWrite(LED_BUILTIN, HIGH);
  delay(1000);
  digitalWrite(LED_BUILTIN, LOW);
  delay(1000);
}

//private functions ******************************************************************

void waitSerial(uint16_t waittime)
{
  //waits for Serial Port or the wiattime has elapsed
  long prev_time = millis();
  long tim = 0;
  while (!Serial)
  {
    tim = millis() - prev_time;
    if (tim > waittime)
      break;
  };
}

void updateData()
{
  if (Serial) // only update if serial port is available
  {
    String choice = "";
    do
    {
      //wanna update data?
      Serial.println("Please insert the number of the option you want to do: ");
      Serial.println("1. change wifi login: ");
      Serial.println("2. calibrate the weight scale: ");
      Serial.println("0. skipp ");

      Serial.setTimeout(30000);
      choice = Serial.readStringUntil('\n');
      Serial.println(choice);
      if (choice.substring(0, 1) == "1")
      {

        EEPROM.put(storedAddress, WRITTEN_SIGNATURE);

        Serial.setTimeout(30000);
        Serial.print("Insert your SSID : ");
        String SSID = Serial.readStringUntil('\n');
        Serial.println(SSID);
        Serial.print("Insert your PWD : ");
        String PWD = Serial.readStringUntil('\n');
        Serial.println(PWD);

        // Fill the "e_data" structure with the data entered by the user...
        SSID.toCharArray(e_data.SSID, 30);
        PWD.toCharArray(e_data.PWD, 30);

        // ...and finally save everything into emulated-EEPROM
        EEPROM.put(storedAddress + sizeof(signature), e_data);

        if (!EEPROM.getCommitASAP())
        {
          Serial.println("CommitASAP not set. Need commit()");
          EEPROM.commit();
        }

        displaySavedData();
      }
      else if (choice.substring(0, 1) == "2")
      {

        EEPROM.put(storedAddress, WRITTEN_SIGNATURE);

        Serial.setTimeout(30000);
        Serial.print("Insert your SCALEFACTOR : ");
        String SCALEFACTOR = Serial.readStringUntil('\n');
        Serial.println(SCALEFACTOR);

        // Fill the "e_data" structure with the data entered by the user...
        SCALEFACTOR.toCharArray(e_data.SCALEFACTOR, 10);

        // ...and finally save everything into emulated-EEPROM
        EEPROM.put(storedAddress + sizeof(signature), e_data);

        if (!EEPROM.getCommitASAP())
        {
          Serial.println("CommitASAP not set. Need commit()");
          EEPROM.commit();
        }

        displaySavedData();
      }
    } while (choice.substring(0, 1) != "0");
  }
}

void displaySavedData()
{
  // Print a confirmation of the data inserted.
  Serial.print("<< Your SSID: ");
  Serial.println(e_data.SSID);
  Serial.print(". Your PWD: ");
  Serial.println(e_data.PWD);
  Serial.print(". Your SCALEFACTOR: ");
  Serial.println(e_data.SCALEFACTOR);
  Serial.println(" >> have been saved. Thank you!");
  Serial.println();
}