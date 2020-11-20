/*
 * View_SDCard_EPD.cpp
 * Copyright (C) 2019-2020 Linar Yusupov
 * Created by George Georgiev
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "EPDHelper.h"

#include <Fonts/FreeMonoBold12pt7b.h>
#include <Fonts/FreeMonoBold18pt7b.h>
#include <Fonts/FreeMono9pt7b.h>

#include "soc/timer_group_struct.h"
#include "soc/timer_group_reg.h"


#include "FS.h"
#include "SD.h"
#include "SPI.h"

#include <TimeLib.h>

//#include "TrafficHelper.h"
//#include "EEPROMHelper.h"
//#include "NMEAHelper.h"
//#include "GDL90Helper.h"

#include "SkyView.h"
#define MAX_FILES 20
#define MAX_FILENAME_SIZE 40
#define IGC_DIRECTORY "/igc"

static bool displayed_sd = false;
//static File current_dir;
static int filesCount = 0;
//static int currentFile=0;
//static int startViewFile =0;
char  files [MAX_FILES] [MAX_FILENAME_SIZE] ;
void getFiles(File);

static void EPD_Draw_SDCard()
{
  return;
  char info_line [TEXT_VIEW_LINE_LENGTH];
  char id_text   [TEXT_VIEW_LINE_LENGTH];
  
  
  display->fillScreen(GxEPD_WHITE);
  display->setPartialWindow(0, 0, display->width(), display->height());
  Serial.println("START DISPLAY");
  display->setFont(&FreeMonoBold12pt7b);
  int16_t  tbx, tby;
  uint16_t tbw, tbh;
  snprintf(info_line, sizeof(info_line), "FILE LIST");
  display->getTextBounds(info_line, 0, 0, &tbx, &tby, &tbw, &tbh);

  display->setCursor(5, 5+tbh);
  display->print(info_line);
  display->setFont(&FreeMono9pt7b);
  
  display->firstPage();
 // return;
//  do
//    {
      uint16_t x = 5;
      uint16_t y = 5+tbh;



      display->fillScreen(GxEPD_WHITE);

     
      for(int i =0;i<filesCount;i++){

        snprintf(info_line, sizeof(info_line), "%s", files[i]);
        display->getTextBounds(info_line, 0, 0, &tbx, &tby, &tbw, &tbh);
        y += tbh;
        display->setCursor(x, y);
        display->print(info_line);
  //      Serial.println(info_line);
  
        y += TEXT_VIEW_LINE_SPACING;
      }
//      if (oclock == 0) {
//        strcpy(info_line, "   ahead");
//      } else {
//        snprintf(info_line, sizeof(info_line), " %2d o'clock", oclock);
//      }
//      display->getTextBounds(info_line, 0, 0, &tbx, &tby, &tbw, &tbh);
//      y += tbh;
//      display->setCursor(x, y);
//      display->print(info_line);
////      Serial.println(info_line);
//
//      y += TEXT_VIEW_LINE_SPACING;
//
//      snprintf(info_line, sizeof(info_line), "%4.1f %s out", disp_dist, u_dist);
//      display->getTextBounds(info_line, 0, 0, &tbx, &tby, &tbw, &tbh);
//      y += tbh;
//      display->setCursor(x, y);
//      display->print(info_line);
////      Serial.println(info_line);
//
//      y += TEXT_VIEW_LINE_SPACING;
//
//      snprintf(info_line, sizeof(info_line), "%4d %s ", disp_alt, u_alt);
//
//      if (traffic[EPD_current - 1].fop->RelativeVertical > 50) {
//        strcat(info_line, "above");
//      } else if (traffic[EPD_current - 1].fop->RelativeVertical < -50) {
//        strcat(info_line, "below");
//      } else {
//        strcpy(info_line, "  same alt."); 
//      }
//
//      display->getTextBounds(info_line, 0, 0, &tbx, &tby, &tbw, &tbh);
//      y += tbh;
//      display->setCursor(x, y);
//      display->print(info_line);
////      Serial.println(info_line);
//
//      y += TEXT_VIEW_LINE_SPACING;
//
//      snprintf(info_line, sizeof(info_line), "CoG %3d deg",
//               traffic[EPD_current - 1].fop->Track);
//      display->getTextBounds(info_line, 0, 0, &tbx, &tby, &tbw, &tbh);
//      y += tbh;
//      display->setCursor(x, y);
//      display->print(info_line);
////      Serial.println(info_line);
//
//      y += TEXT_VIEW_LINE_SPACING;
//
//      snprintf(info_line, sizeof(info_line), "GS  %3d %s", disp_spd, u_spd);
//      display->getTextBounds(info_line, 0, 0, &tbx, &tby, &tbw, &tbh);
//      y += tbh;
//      display->setCursor(x, y);
//      display->print(info_line);
////      Serial.println(info_line);
//
//      y += TEXT_VIEW_LINE_SPACING;
//
//      display->getTextBounds(id_text, 0, 0, &tbx, &tby, &tbw, &tbh);
//      y += tbh;
//      display->setCursor(x, y);
//      display->print(id_text);
////      Serial.println(id_text);
//
//      Serial.println();
//    }
//    while (display->nextPage());

    display->hibernate();
  
}
void addFileToList(char *fileName){
  Serial.println(filesCount);
  if(filesCount ==0){
    strcpy(files[0],fileName);
    filesCount++;
    return;
  }
  int filePlace =0;
  for(int i=0; i< filesCount;i++){ 
     
    filePlace = i;
    if(fileName==files[i])
      return;

    if(strcmp(fileName,files[i])>0)
      break;
  }
  if(filePlace>filesCount)
     return;
  if(filesCount>=MAX_FILES){
    strcpy(files[MAX_FILES-1],fileName);
  } else{
    // adding place at sorting place
    for(int i=MAX_FILES-2; i>=filePlace;i--)
      strcpy(files[i+1],files[i]);

    strcpy(files[filePlace],fileName);
   
    filesCount++;
  }
  
  //reorder files by date
  for(int i=0;i<filesCount;i++){ 
    if(strcmp(files[i],files[i+1])>0){
       char tmp[MAX_FILENAME_SIZE];
       strcpy(tmp, files[i]);
       strcpy(files[i],files[i+1]);
       strcpy(files[i+1], tmp);
    }
  }

}
void getFiles(File dir) {
//  Serial.println(F("GETTING FILES..."));
//   for(int i=0;i<filesCount;i++)
//      Serial.println(i+"->"+files[i]);
  while (true) {

    File entry =  dir.openNextFile();
   // current_dir = entry;
    
//    if(filesCount>MAX_FILES-1){
//       entry.close();
//       return;
//    }
    if (! entry ){
      if( dir.name()==IGC_DIRECTORY&&filesCount==0) {
       Serial.println("NO FILES");
       EPD_Message("NO FILES",NULL);

      }
   //   Serial.print("Exiting from directory\n");
      break;

    }

    if (entry.isDirectory()) {

      getFiles(entry);

    } else {
      
      String tmp = entry.name();
      if(tmp.length()>5) 
        tmp = tmp.substring(5);
      
      if(tmp.indexOf('/')>-1){
        int index = tmp.indexOf('/')+1;
         tmp = tmp.substring(index);
      }
   
      char charFileName[MAX_FILENAME_SIZE];
      tmp.toCharArray(charFileName,MAX_FILENAME_SIZE);
    //  strcpy(charFileName,F(tmp));
      Serial.print("add:");
      Serial.println(charFileName);
      addFileToList(charFileName);
    }

    entry.close();
    yield();

  }
}


void EPD_sdcard_setup()
{
}

void EPD_sdcard_loop()
{

  if (!EPD_display_frontpage) {

    EPD_Clear_Screen();
    EPD_display_frontpage = true;
    EPD_Draw_SDCard();

    filesCount =0;
    File root_dir =SD.open(IGC_DIRECTORY);
    if(!root_dir){
       Serial.println("NO FILES->MESSAGE TO DISPLAY");
       EPD_Message("NO FILES",NULL);
    }
    getFiles(root_dir);
    root_dir.close();
    Serial.println("STARTING LIST");
    for(int i=0;i<filesCount;i++){
       char id_text   [MAX_FILENAME_SIZE+10];
      snprintf(id_text, sizeof(id_text), "%2d - %s",i,files[i]);
      Serial.println(id_text);
    }
    Serial.println("END OF LIST");

  } else {
    
   //  filesCount =0;
    

  }
}

void EPD_sdcard_next()
{
    filesCount =0;
    File root_dir =SD.open(IGC_DIRECTORY);
    if(!root_dir){
       Serial.println("NO FILES->MESSAGE TO DISPLAY");
       EPD_Message("NO FILES",NULL);
    }
    for(int i=0;i< MAX_FILES;i++)
      strcpy(files[i],"");
    getFiles(root_dir);
    root_dir.close();
    Serial.println("STARTING LIST");
    for(int i=0;i<filesCount;i++){
       char id_text   [MAX_FILENAME_SIZE+10];
      snprintf(id_text, sizeof(id_text), "%2d - %s",i,files[i]);
      Serial.println(id_text);
    }
    Serial.println("END OF LIST");
}

void EPD_sdcard_prev()
{

    filesCount =0;
    File root_dir =SD.open(IGC_DIRECTORY);
    if(!root_dir){
       Serial.println("NO FILES->MESSAGE TO DISPLAY");
       EPD_Message("NO FILES",NULL);
    }
    getFiles(root_dir);
    root_dir.close();
    Serial.println("STARTING LIST");
    for(int i=0;i<filesCount;i++){
       char id_text   [MAX_FILENAME_SIZE+10];
      snprintf(id_text, sizeof(id_text), "%2d - %s",i,files[i]);
      Serial.println(id_text);
    }
    Serial.println("END OF LIST");
}
