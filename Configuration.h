#ifndef Config_H
#define Config_H


#define LOGGING_ENABLED
//#define SPIFFS_ENABLE 1


// -- WIFI CONFIG
const char HOSTNAME[] = "PixelTimes";

// -- TIME CONFIG
const char* ntp_server = "0.ca.pool.ntp.org";

// -- WEATHER
int show_weather=true;
const char* weather_api_url = "http://api.openweathermap.org/data/2.5/weather?q=Kitchener,CA&APPID=2c615a3ac804e362d1de5cf62b74a949&units=metric";
const char* forecast_api_url = "http://api.openweathermap.org/data/2.5/forecast?q=Kitchener,CA&APPID=2c615a3ac804e362d1de5cf62b74a949&units=metric";

#define DEFAULT_BRIGHTNESS 50


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