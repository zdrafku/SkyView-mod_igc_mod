/*
 *  WriteToCard.h
 *
 *  Write flight track to SD card
 *
 *  Author: Mr. Psycho, psychotm@gmail.com
 */

#include "WriteToCard.h"

unsigned long ElapsedTime;
unsigned long StartTime;
char FileName[sizeof(unsigned long)*8+1];

void appendFile(fs::FS &fs, const char *path, String message) {
  //Serial.printf("Appending to file: %s\n", path);

  File file = fs.open(path, FILE_APPEND);
  if (!file) {
    //Serial.println("Failed to open file for appending");
    return;
  }
  if (file.print(message)) {
    //Serial.print("Message appended to file: ");
    //Serial.println(message);
  }
  file.close();
}

void serial_prints() {
  unsigned long CurrentTime = millis();
  ElapsedTime = CurrentTime - StartTime;

  if ( WAIT_TIME < ElapsedTime ) {
    // if there is valid GNSS data
    if ( isValidGNSSFix() ) {
      sprintf(FileName, "/%4u-%02u-%02u-PSY-CHO-", nmea.date.year(), nmea.date.month(), nmea.date.day());
      // <time> <lat> <long> <altitude from pressure sensor> <altitude from GPS>
      // B 110135 52.06343N 000.06198W A 00587 00558

      String T_Row = "B";
      T_Row += nmea.time.value();
      T_Row.remove(7, 2);
      T_Row += " ";
      T_Row += String(nmea.location.lat(), 5);
      T_Row += nmea.location.rawLat().negative ? "S" : "N";

      T_Row += String(nmea.location.lng(), 5);
      T_Row += nmea.location.rawLng().negative ? "W" : "E";
      T_Row += "A";
      T_Row += "altBARO";
      T_Row += " ";
      T_Row += String(nmea.altitude.meters(), 0);
      T_Row.replace(".", "");
      T_Row += "\n";
      appendFile(SD, FileName, T_Row);

      //Serial.print("T_Row: ");
      //Serial.println(T_Row);
    }

    StartTime = millis();
    ElapsedTime = 0;
  }
}
