/*
 *  WriteToCard.h
 *
 *  Write flight track to SD card
 *
 *  Author: Mr. Psycho, psychotm@gmail.com
 */

#ifndef WRITETOCARD_H
#define WRITETOCARD_H

#include "EPDHelper.h"
#include "SkyView.h"

#include "FS.h"
#include "SD.h"
#include "SPI.h"
#include "TinyGPS++.h"

#define WAIT_TIME       1000 /* 5 seconds */
#define SerialInput  Serial1

void serial_prints();
void filename_print();

#endif /* WRITETOCARD_H */
