/*
 *  WriteToCard.h
 *
 *  Write flight track to SD card
 *
 *  Author: Mr. Psycho, psychotm@gmail.com
 */

#include "WriteToCard.h"
TinyGPSPlus gpsdata;

unsigned long ElapsedTime;
unsigned long StartTime;
char l_CurrentTime[sizeof(unsigned long)*8+1];

void filename_print()  {
  long CurrentTime = millis();
  ltoa(CurrentTime, l_CurrentTime, sizeof(CurrentTime)+2);

  const char *FileName = '/' + l_CurrentTime + '.txt';
  Serial.print("DEBUG-file: ");
  Serial.println(FileName);
  Serial.print("DEBUG: ");
  Serial.println(l_CurrentTime);
  Serial.print("DEBUG-2: ");
  Serial.println(CurrentTime);
  // shutdown Arduino!
  esp_deep_sleep_start();
}

bool checkFile(fs::FS &fs, const char *path){
  File file = fs.open(path);
  return file;
}

void appendFile(fs::FS &fs, const char *path, const char *message) {
  //Serial.printf("Appending to file: %s\n", path);

  File file = fs.open(path, FILE_APPEND);
  if (!file) {
    //Serial.println("Failed to open file for appending");
    return;
  }
  if (file.print(message)) {
    //Serial.println("Message appended");
  } else {
    //Serial.println("Append failed");
  }
}

void serial_prints() {
  unsigned long CurrentTime = millis();
  ElapsedTime = CurrentTime - StartTime;
  ultoa(CurrentTime, l_CurrentTime, 20);

  const char *FileName = '/' + l_CurrentTime + '.txt';  
  if ( checkFile(SD, FileName) ) {
    Serial.println("File exists");
  } else {
    Serial.print("File ");
    Serial.print(FileName);
    Serial.println(" doesn't exist!!!");
    //appendFile(SD, FileName, "jjj\n");
  }
  // if have passed WAIT_TIME seconds
  if ( WAIT_TIME < ElapsedTime ) {
    // if there is valid GNSS data
    if ( gpsdata.location.isValid()         && \
        gpsdata.altitude.isValid()            && \
        gpsdata.date.isValid()                && \
        (gpsdata.location.age() <= WAIT_TIME) && \
        (gpsdata.altitude.age() <= WAIT_TIME) && \
        (gpsdata.date.age()     <= WAIT_TIME)    \
    ) {
      Serial.print("Date: ");
      Serial.println(gpsdata.date.value());
      Serial.print("Latitude: ");
      Serial.println(gpsdata.location.rawLat().deg);
      Serial.print("Longitude: ");
      Serial.println(gpsdata.location.rawLng().deg);
      Serial.print("Speed (km/h): ");
      Serial.println(gpsdata.speed.kmph());
      Serial.print("Altitude: ");
      Serial.println(gpsdata.altitude.meters());
      Serial.print("Satellites: ");
      Serial.println(gpsdata.satellites.value());
    } else {
      Serial.println("No GPS Signal!");
      Serial.print("Satellites: ");
      Serial.println(gpsdata.satellites.value());
    }

    StartTime = millis();
    ElapsedTime = 0;
  }
}

/*
#define NMEA_EXP_TIME  4500 // 4.5 seconds
#define isValidGNSSFix()  ( nmea.location.isValid()               && \
                            nmea.altitude.isValid()               && \
                            nmea.date.isValid()                   && \
                           (nmea.location.age() <= NMEA_EXP_TIME) && \
                           (nmea.altitude.age() <= NMEA_EXP_TIME) && \
                           (nmea.date.age()     <= NMEA_EXP_TIME))

if (hasData) {

  bool hasFix = settings->protocol == PROTOCOL_NMEA  ? isValidGNSSFix()   :
                settings->protocol == PROTOCOL_GDL90 ? GDL90_hasOwnShip() :
                false;

  if (hasFix) {
    EPD_Draw_Radar();
  } else {
    EPD_radar_Draw_Message(NO_FIX_TEXT, NULL);
  }
} else {
  EPD_radar_Draw_Message(NO_DATA_TEXT, NULL);
}

while (SerialInput.available() > 0) {
  char c = SerialInput.read();
  Serial.println("Serial read");
  Serial.print(c);
}

bool isValidSentence = gpsdata.encode(c);
if (isValidSentence) {
  //print GPS data to serial
}

Serial.print("Date: ");
Serial.println(gpsdata.date.value());
Serial.print("Latitude: ");
Serial.println(gpsdata.location.rawLat().deg);
Serial.print("Longitude: ");
Serial.println(gpsdata.location.rawLng().deg);
Serial.print("Speed (km/h): ");
Serial.println(gpsdata.speed.kmph());
Serial.print("Altitude: ");
Serial.println(gpsdata.altitude.meters());
Serial.print("Satellites: ");
Serial.println(gpsdata.satellites.value());

Serial.println(gpsdata.date.year()); // Year (2000+) (u16)
Serial.println(gpsdata.date.month()); // Month (1-12) (u8)
Serial.println(gpsdata.date.day()); // Day (1-31) (u8)
Serial.println(gpsdata.time.value()); // Raw time in HHMMSSCC format (u32)
Serial.println(gpsdata.time.hour()); // Hour (0-23) (u8)
Serial.println(gpsdata.time.minute()); // Minute (0-59) (u8)
Serial.println(gpsdata.time.second()); // Second (0-59) (u8)
Serial.println(gpsdata.time.centisecond()); // 100ths of a second (0-99) (u8)
*/
