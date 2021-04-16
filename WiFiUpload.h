/*
 *  WiFiUpload.h
 *
 *  Export to server files in ICG directory via WiFi AP
 *
 *  Author: Mr. Psycho, psychotm@gmail.com
 */

#ifndef WIFIUPLOAD_H
#define WIFIUPLOAD_H

#include "WriteToCard.h"
#include "WiFiHelper.h"
#include "WiFi.h"


//#define SSID_NAME     "WIFI SSID"
//#define SSID_PASSWORD "WIFI PASS"
//#define IGC_DIRECTORY "/igc/"
#define IGC_ARCHIVE   "/archive"

void uWiFi_setup();
void IGC_Upload();

#endif /* WIFIUPLOAD_H */
