#include "Database.h"

File fileDB;

String readDatabase() {
  // re-open the file for reading:
fileDB = SD.open("test1.txt");
  String fileContent = "empty";
  if (fileDB) {
    // Serial.println("test.txt:");
    // read from the file until there's nothing else in it:
    while (fileDB.available()) {
      // Serial.write(myFile.read());
      fileContent = fileDB.readString();
    }
    // close the file:
    fileDB.close();
  } else {
    // if the file didn't open, print an error:
    Serial.println("error opening test1.txt");
  }
  return fileContent;
}