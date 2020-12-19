#ifndef Config_H
#define Config_H


#define LOGGING_ENABLED 1
//#define SPIFFS_ENABLE 1


// -- WIFI CONFIG
const char HOSTNAME[] = "PixelTimes";

// -- TIME CONFIG
const char* ntp_server = "0.ca.pool.ntp.org";

// -- WEATHER
int show_weather=true;

int brightness=50;




// ------------------------------- //
// ------------------------------- //
// ---- END OF REGULAR CONFIG ---- //
// ------------------------------- //
// ------------------------------- //
// Do not change, or do I dont care :)

#define RGB 565
#define ADAFRUIT_GFX_EXTRA 10


#if RGB==565
  #define frame_size 1024
#else
  #define frame_size 1536
#endif

#endif


#define BRIGHTNESS_MAX 255
#define BRIGHTNESS_MIN 0