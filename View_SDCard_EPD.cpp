/*
 * View_SDCard_EPD.cpp

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
#include <Fonts/FreeSerif9pt7b.h>
#include "WiFiUpload.h"

#include "esp_task_wdt.h"

#include "writetocard.h"
#include "SD.h"

#define MAX_FILES 15
#define MAX_FILENAME_SIZE 40
#define TEXT_VIEW_FILE_SPACING 2
#define TEXT_VIEW_FILE_LENGTH 22

static bool displayed_sd = false;

static int filesCount = 0;

char  files [MAX_FILES] [MAX_FILENAME_SIZE] ;
void getFiles(File);

static void EPD_Draw_SDCard()
{

  char info_line [TEXT_VIEW_FILE_LENGTH];
  char id_text   [TEXT_VIEW_FILE_LENGTH];
  uint16_t x = 5;
  uint16_t y = 2;
  
  display->fillScreen(GxEPD_WHITE);
  display->setPartialWindow(0, 0, display->width(), display->height());
  
  display->setFont(&FreeMonoBold12pt7b);
  int16_t  tbx, tby;
  uint16_t tbw, tbh;
  char *initText = "FILE LIST";
  snprintf(info_line, sizeof(info_line), initText);
  display->getTextBounds(info_line, 0, 0, &tbx, &tby, &tbw, &tbh);
  y+=tbh;
  display->setCursor(x+20, y);
  display->print(info_line);
  Serial.println(info_line);
  display->setFont(&FreeSerif9pt7b);
 // display->firstPage();
  y += TEXT_VIEW_FILE_SPACING;
  x=2;
  do {
    
   //   display->fillScreen(GxEPD_WHITE);

      for(int i =0;i<filesCount;i++){
        snprintf(info_line, sizeof(info_line), "%s", files[i]);
        display->getTextBounds(info_line, 0, 0, &tbx, &tby, &tbw, &tbh);
        y += tbh;
        display->setCursor(x, y);
        display->print(info_line);
  
        y += TEXT_VIEW_FILE_SPACING;
      }
    } while (display->nextPage());

    display->hibernate();
  
}

int getFilePosition(char *fileName){
  int filePlace =0;
  for(int i=0; i< filesCount;i++){ 
     
    filePlace = i;
    if(fileName==files[i])
      return -1;

    if(strcmp(fileName,files[i])>0)
      return filePlace;
  }
  return filePlace;
}

void putFileOnPosition(char *fileName, int filePlace){
  if(filePlace<0) return;
  for(int i=MAX_FILES-2; i>=filePlace;i--) strcpy(files[i+1],files[i]);
  strcpy(files[filePlace],fileName);
    
  if(filesCount<MAX_FILES)
    filesCount++;  
}

char*  shortenFileName(char *fileNameOrig){
  char *ch;
  static char fileName[MAX_FILENAME_SIZE];

  ch =fileNameOrig;

  int i=0;
  int minusChar =0;
  while (*ch) // exits on terminating NULL
  {
    if(minusChar<2){
      if(*ch=='-'){
        minusChar++;
        ch++;
        continue;
      }
    }
    if (*ch >= 65 && *ch <= 90) {
      fileName[i] =*ch + 32;
    } else{
      fileName[i]=*ch;
    }
    if(i>5&&fileName[i-3]=='.'&&fileName[i-2]=='i'&&fileName[i-1]=='g'&&fileName[i]=='c')
       fileName[i-3]=NULL;
    i++;
    fileName[i]=NULL;
    ch++;
  }
  return fileName;
}
void addFileToList(char *fileNameOrig){
  char fileName[MAX_FILENAME_SIZE];
  snprintf(fileName, sizeof(fileName), "%s",shortenFileName(fileNameOrig));
  

  if(filesCount ==0){
    strcpy(files[0],fileName);
    filesCount++;
    return;
  }
  int filePlace =getFilePosition(fileName);
  
  putFileOnPosition(fileName,filePlace);
  esp_task_wdt_reset();
//  reorderFiles();

}
void getFiles(File dir) {

  while (true) {

    File entry =  dir.openNextFile();

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
//      Serial.print("add:");
//      Serial.println(charFileName);
      addFileToList(charFileName);
    }

    entry.close();
  
  }
}

void getInitialFiles(){
  
    filesCount =0;
    File root_dir =SD.open(IGC_DIRECTORY);
    if(!root_dir){
       Serial.println("NO FILES->MESSAGE TO DISPLAY");
       EPD_Message("NO FILES","FOUND");
       return;
    }
    getFiles(root_dir);
    root_dir.close();
}

void EPD_sdcard_setup()
{
}

void EPD_sdcard_loop()
{
 // esp_task_wdt_delete();
  if (!EPD_display_frontpage) {

    EPD_Clear_Screen();
    getInitialFiles();
    EPD_display_frontpage = true;
    EPD_Draw_SDCard();
    
    Serial.println("STARTING FILES LIST");
    for(int i=0;i<filesCount;i++){
       char id_text   [MAX_FILENAME_SIZE+10];
      snprintf(id_text, sizeof(id_text), "%2d - %s",i,files[i]);
      Serial.println(id_text);
    }
    Serial.println("END OF LIST");

  }
//  else {
//    if (filesCount > 0) {
//            EPD_Draw_SDCard();
//          } else {
//            EPD_Message("NO FILES","FOUND");
//          }
////     if (isTimeToDisplay()) {
////          if (filesCount > 0) {
////            EPD_Draw_SDCard();
////          } else {
////             EPD_Message("NO FILES","FOUND");
////          }
////      EPDTimeMarker = millis();
////    }
//  }
}

void EPD_sdcard_next()
{
  Serial.println("REFRESH OF FILE LIST ACTIVATED");
  EPD_display_frontpage = false;
}

void EPD_sdcard_prev()
{
  Serial.println("IGC FILES UPLOAD ACTIVATED");
  IGC_Upload();
  EPD_display_frontpage = false;
}
