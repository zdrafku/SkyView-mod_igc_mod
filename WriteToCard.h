/*
 *  WriteToCard.h
 *
 *  Write flight track to SD card
 *
 *  Author: Mr. Psycho, psychotm@gmail.com
 */

#ifndef WRITETOCARD_H
#define WRITETOCARD_H

// EEPROMHelper is needed for the settings->protocol variable:
#include "EEPROMHelper.h"
// NMEAHelper is needed for the GPS/NMEA data
#include "NMEAHelper.h"
#include "SkyView.h"

#include "FS.h"
#include "SD.h"
#include "SPI.h"
#include "TinyGPS++.h"

#define isValidGNSSFix()  ( nmea.location.isValid()               && \
                            nmea.altitude.isValid()               && \
                            nmea.date.isValid()                   && \
                           (nmea.location.age() <= NMEA_EXP_TIME) && \
                           (nmea.altitude.age() <= NMEA_EXP_TIME) && \
                           (nmea.date.age()     <= NMEA_EXP_TIME))

#define WAIT_TIME       4500  /* 4.5 seconds */
#define KEEP_ALIVE      20000 /* 60 seconds */
#define MIN_SPEED       20    /* 20km/h */

void serial_prints();

#endif /* WRITETOCARD_H */
