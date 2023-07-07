#include <Arduino.h>

#include <SPI.h>
#include <SD.h>

File myFile;

void setup() {

  Serial.begin(115200);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }
  Serial.print("start");
  if (!SD.begin(4)) {
    Serial.println("initialization failed!");
    while (1);
  }
  Serial.println("delete file");
  SD.remove("test.txt");

  // myFile = SD.open("test.txt", FILE_WRITE);

  // if (myFile) {
  //   Serial.print("Writing to test.txt...");
  //   myFile.println("testing 1, 2, 3.");
  //   // close the file:
  //   myFile.close();
  //   Serial.println("done.");
  // } else {
  //   // if the file didn't open, print an error:
  //   Serial.println("error opening test.txt");
  // }

  myFile = SD.open("wifi.txt");
  String finalString = "";

  if (myFile) {
    Serial.println("wifi.txt:");

    
    while (myFile.available())
    {
      finalString += (char)myFile.read();
    }
    Serial.println(finalString);
    
    // close the file:
    myFile.close();
  } else {
    // if the file didn't open, print an error:
    Serial.println("error opening test.txt");
  }
  
  int ind1 = finalString.indexOf(',');
  Serial.println(finalString.substring(0,ind1));
  Serial.println(finalString.substring(ind1+1));
}

void loop()
{
	// nothing happens after setup
}
