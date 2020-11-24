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
unsigned long Record;
unsigned long NoSpeed;

String FileName;
String T_Row;
char F_Date[12];
char GPS_Time[7];
char GPS_Alt[6];
char GPS_Lat[8];
char GPS_Lng[9];
bool run_once;

void appendFile(fs::FS &fs, String path, String message) {
  File file = fs.open(path, FILE_APPEND);
  if (!file) {
    //Serial.println("Failed to open file for appending" + path);
    Serial.print("Would appended message to file: ");
    Serial.println(path);
    Serial.println(message);
    file.close();
    return;
  }

  if (file.print(message)) {
    Serial.println("Message appended to file: " + path);
    Serial.println(message);
  }
  file.close();
}

String fCheck(fs::FS &fs, String fDate) {
  String f_Constant = "-PSY-CHO-"; // + String((SoC->getChipId() & 0xFFFFFF), HEX) + "-";
  //String r_Path = "/" + fDate + "/" + fDate + f_Constant;
  String r_Path = IGC_DIRECTORY;
  r_Path += "/" + fDate + f_Constant;
  char count[2];

  //fs.mkdir("/" + fDate);
  fs.mkdir(IGC_DIRECTORY);

  //File root_dir = fs.open("/" + fDate);
  File root_dir = fs.open(IGC_DIRECTORY);
  File list = root_dir.openNextFile();
  int c = 0;
  while (list) {
    String file_name = list.name();
    String f_count = file_name.substring(r_Path.length(), r_Path.length()+2);
    if ( file_name.indexOf(r_Path) == 0 ) {
      if ( f_count.toInt() > 0 && f_count.toInt() > c) {
        c = f_count.toInt();
      }
    }
    esp_task_wdt_reset();
    list = root_dir.openNextFile();
  }
  root_dir.close();
  sprintf(count, "%02i", ++c);
  return r_Path + count + ".igc";
}

void serial_prints() {
  unsigned long CurrentTime = millis();
  ElapsedTime = CurrentTime - StartTime;


  if (WAIT_TIME < ElapsedTime) {
      // if there is valid GNSS data

    if (isValidGNSSFix()) {
      Record = CurrentTime - NoSpeed;

      if ( (int)nmea.speed.kmph() < MIN_SPEED ) {
        if ( run_once && KEEP_ALIVE < Record ) {
          Serial.print("Will start new .IGC file and reset RECORD with speed: ");
          Serial.println((int)nmea.speed.kmph());
          run_once = false;
          Record = 0;
          return;
        } else {
          //run_once = false;
          return;
        }
      }

      Serial.print("Resetting RECORD: ");
      Serial.println(Record/1000);
      Record = 0;

      // %d.%02d", (int)f, (int)(f*100)%100);
      sprintf(GPS_Alt, "%05u", (int)nmea.altitude.meters());
      sprintf(GPS_Lat, "%02u%5u", (int)nmea.location.lat(), (int)(nmea.location.lat()*100000)%100000);
      sprintf(GPS_Lng, "%03u%5u", (int)nmea.location.lng(), (int)(nmea.location.lng()*100000)%100000);

      if ( !run_once ) {
        sprintf(F_Date, "%04u-%02u-%02u", nmea.date.year(), nmea.date.month(), nmea.date.day());
        FileName = fCheck(SD, F_Date);

        char H_Date[4];
        sprintf(H_Date, "%02u%02u", nmea.date.day(), nmea.date.month());
        String str_H_Date = H_Date;
        str_H_Date += nmea.date.year();
        str_H_Date.remove(4, 2);

        // <time> <lat> <long> <altitude from pressure sensor> <altitude from GPS>
        String Header = "APSYCHO SkyView-mod\n";
        Header += "HFDTE" + str_H_Date + "\n";
        Header += "HFFXA050\n";
        // from web settings
        Header += "HFPLTPILOTINCHARGE:";
        Header.concat(settings->glider_pilot);
        Header += "\n";
        Header += "HFCM2CREW2:";
        Header.concat(settings->glider_crew);
        Header += "\n";
        Header += "HFGTYGLIDERTYPE:";
        Header.concat(settings->glider_type);
        Header += "\n";
        Header += "HFGIDGLIDERID:";
        Header.concat(settings->glider_registration);
        Header += "\n";
        // from web settings
        Header += "HFFTYFRTYPE: SoftRF + SkyView-mod by Mr. Psycho\n";
        Header += "HFGPS: SoftRF tracker\n";
        // if altBaro is set
        // Header += "HFPRSPRESSALTSENSOR: \n";
        Header += "HFDTM100GPSDATUM:WGS-1984\n";
        Header += "I023638FXA3940SIU\n";
        run_once = true;

        appendFile(SD, FileName, Header);
      }

      T_Row = "B";
      sprintf(GPS_Time, "%02u%02u%02u", nmea.time.hour(), nmea.time.minute(), nmea.time.second());
      T_Row += GPS_Time;
      T_Row += GPS_Lat;
      T_Row += nmea.location.rawLat().negative ? "S" : "N";
      T_Row += GPS_Lng;
      T_Row += nmea.location.rawLng().negative ? "W" : "E";
      T_Row += "A";
      T_Row += GPS_Alt;
      T_Row += GPS_Alt;
      T_Row += "00000";
      T_Row += "\n";

      appendFile(SD, FileName, T_Row);
      //Serial.println(T_Row);
    }
    StartTime = millis();
    ElapsedTime = 0;
  }
}
