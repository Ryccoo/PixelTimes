#include "Configuration.h"
#include "anim_data.h"

#include <HTTPClient.h>

#include <Time.h>
#include <TimeLib.h>
#include <WebServer.h>
//#include <ESP8266WebServer.h>

#include <DNSServer.h>
//#include <TimeLib.h>   // https://github.com/PaulStoffregen/Time
#include <Ticker.h>
#include <EEPROM.h>
//#include <time.h>
//#include <ESP8266HTTPClient.h>
#include <NTPClient.h>
//#include <NtpClientLib.h>   // https://github.com/2dom/NtpClient
#include <PxMatrix.h>
#include <Fonts/TomThumb.h>
#include "FS.h"
#include <SPIFFS.h>


#include "DrawingBuffer.h"
#include "Display.h"
#include "Weather.h"
#include "PixelWifi.h"


bool shouldSaveWifiConfig = true;
unsigned long button_press_time=0;

File ff;
File fsUploadFile;


// void button_pressed()
// {
//   if ((millis()-button_press_time)<500)
//   return;
//   show_weather=!show_weather;
//   button_press_time=millis();
//   Serial.println("Button");
// }

String filenames[100];
int no_files=0;

//void ntpSet () {
//  NTP.onNTPSyncEvent([](NTPSyncEvent_t event) {
//    ntpEvent = event;
//    syncEventTriggered = true;
//  });
//  NTP.begin(ntp.c_str(), 1, true);
//  NTP.setInterval(63);
//}
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, ntp_server, -3600 * 5, 60000);
void NTPSetup() {
  timeClient.begin();
  timeClient.update();
  Serial.println(timeClient.getFormattedTime());
  setTime(timeClient.getEpochTime());
}

void setup() {
  Serial.begin(115200);
  // yield();
  Serial.println("Setup");
  delay(1000);
  setupWifi();
  NTPSetup();

  Serial.print("\n");
  Serial.print(hour());
  Serial.print(":");
  Serial.print(minute());
  Serial.print(":");
  Serial.print(second());
  Serial.print("\n"); 
  

#ifdef SPIFFS_ENABLE
  SPIFFS.begin(true);
//  fs::Dir dir = SPIFFS.openDir("/");
//  while (dir.next()) {
//
//    Serial.println(dir.fileName());
//    filenames[no_files]=dir.fileName();
//    no_files++;
//  }
  File root = SPIFFS.open("/");
 
  File file = root.openNextFile();
 
  while(file){
      Serial.print("FILE: ");
      Serial.println(file.name());
      file = root.openNextFile();
  }

#else
  no_files=sizeof(animation_lengths);
#endif

  Serial.println("Displaying");
  display.begin(8);
  display.setFont(&TomThumb);
  display.clearDisplay();
  display.setTextColor(myCYAN);
  display.setCursor(2,6);
  display.print("HELLO");
  display.setTextColor(myBLUE);
  display.setCursor(2,12);
  display.print(" -10");
  display.setCursor(2,12);
  display.setTextColor(myWHITE);
  display_buffer.setFont(&TomThumb);

//  display_ticker.attach(0.001, display_updater);
  display_update_enable(true);
//  attachInterrupt(digitalPinToInterrupt(0),button_pressed,FALLING);
//  dimm=1;
  // yield();
  delay(3000);
  Serial.println("End of setup");
}


// This draws the time to the frame buffer in animation view
void draw_time()
{
  uint8_t this_hour= hour();
  uint8_t this_minute= minute();

  String time_string;

  if (this_hour<10)
  time_string+= "0";

  time_string+= String(this_hour);
  time_string+= ":";

  if (this_minute<10)
  time_string+="0";
  time_string+=String(this_minute);
  display_buffer.setCursor(15,6);
  display_buffer.setInvertTextColor(true);
  display_buffer.print(time_string);
  display_buffer.setInvertTextColor(false);
}


void loop() {
  if(Serial.available() > 0) {
    Serial.readString();
    show_weather = !show_weather;
  }
  // Serial.println("loop");
  // // display_buffer.fillScreen(myBLACK);
  // // memset(display_buffer.frame_buffer,0,frame_size);
  // display_buffer.clearScreen();
  // display_buffer.setCursor(1,6);
  // display_buffer.setTextColor(myBLUE);
  // String a;
  // a += String(millis());
  // display_buffer.print(a);
  // render_frame();
  // delay(100);
  // return;

  if (show_weather)
  {

    bool ret_code;
    unsigned long this_time=millis();
    yield();
    
    // Update weather data every 10 minutes
    if (this_time>next_weather_update)
    {
      ret_code=update_weather();
      if (ret_code) {
        next_weather_update=this_time+ (1000 * 60 * 60);
      }
      else {
        next_weather_update=this_time+ (1000 * 30);
      }
      weather_render_frame = true;
    }

    if (minute() != last_minute) {
      weather_render_frame = true;
    }

    if (this_time > weather_next_icon_cycle) {
      weather_next_icon_cycle += 1000;
      weather_render_frame = true;
    }

    display_buffer.clearScreen();
    draw_time_weather ();

    // Exchange sun for moon icon when past sunset
    if ((sunset<now()) && (icon_show_low[0]<2))
    icon_show_low[0]=icon_show_low[0]+9;
    if ((sunset<now()) && (icon_show_high[0]<2))
    icon_show_high[0]=icon_show_high[0]+9;

    // Flip between icons every 2 seconds to animate them a bit
    // Flip between high and lows every 5 seconds
    if ((second()%20)<10)
    {
     // Dot to indicate high or low
     display_buffer.drawPixel(0,15,myWHITE);
     draw_weather_icon(icon_show_low[0],0,temperature_show_low[0],(second()%2>=1));
     draw_weather_icon(icon_show_low[0],0,temperature_show_low[0],(second()%2<1));
     draw_weather_icon(icon_show_low[1],1,temperature_show_low[1],(second()%2>=1));
     draw_weather_icon(icon_show_low[1],1,temperature_show_low[1],(second()%2<1));
    }
    else
    {
     draw_weather_icon(icon_show_high[0],0,temperature_show_high[0],(second()%2>=1));
     draw_weather_icon(icon_show_high[0],0,temperature_show_high[0],(second()%2<1));
     draw_weather_icon(icon_show_high[1],1,temperature_show_high[1],(second()%2>=1));
     draw_weather_icon(icon_show_high[1],1,temperature_show_high[1],(second()%2<1));
     // Dot to indicate high or low
     display_buffer.drawPixel(0,11,myWHITE);
    }

    // If we had problems fetching the weather we display a red dot
    if (weather_get_error) {
      display_buffer.drawPixel(0,0,myRED);
    }

    // TODO
    // process_ntp();
    if(weather_render_frame) {
      render_frame();
      weather_render_frame = false;
    }
    last_minute = minute();

    if (!show_weather)
      return;
    
    yield();
    delay(10);
    
//
//
  }
  else
  {

    // Random animation file
    dimm = +1;
    randomSeed(micros());
    uint8_t jump=random(no_files);
    unsigned long last_animation_change = millis();
    Serial.println("Playing animation: " + String(jump));

#ifdef SPIFFS_ENABLE

    if (!filenames[jump].endsWith("rgb"))
      return;
    //Open file
    ff = SPIFFS.open(filenames[jump],"r");
    Serial.println("Now playing: " + String(filenames[jump]));
    int no_of_frames=ff.size()/frame_size;
    //int no_of_frames=sizeof(animations)/frame_size;

 #else

    int no_of_frames=animation_lengths[jump];
    int frame_offset=0;
    for (int anim_no=0;anim_no<jump;anim_no++)
      frame_offset=frame_offset+animation_lengths[anim_no];

 #endif


    while(1)
    {

      if ((millis()-last_animation_change)>10000)
        break;

      int frame=0;

      while (1)
      {
        if (show_weather)
          return;

        // // Copy current frame to frame buffer
        // unsigned long draw_start_time = millis();
 
        //  unsigned long draw_duration = millis()-draw_start_time;
        // //Serial.println("Draw time" + String(draw_duration));

        // unsigned long frame_end_time = millis();


        // // Fetch new frame byte by byte - otherwise the fs will lock
        // // and prevent timely display updates
        // unsigned long start_read = micros();

#ifdef SPIFFS_ENABLE
        for (int this_byte=0;this_byte<frame_size;this_byte++){
          ff.read(display_buffer.frame_buffer+this_byte,1);
           yield();
        }
#else
        memcpy_P(display_buffer.frame_buffer,animations+(frame_offset+frame)*frame_size,frame_size);
#endif
        // unsigned long read_latency=micros()-start_read;
        //if (read_latency>500)
           //Serial.println("Read latency problem, latency: " + String(read_latency));

        // Write time to frame buffer
        draw_time ();
        render_frame();
        unsigned long frame_end_time = millis();
        frame++;
        yield();
        // TODO
//        process_ntp();

        if (frame==no_of_frames)
        {
          ff.seek(0,SeekSet);
          break;

        }
        while ((millis()-frame_end_time)<150)
        {
          //display.display(70);
          delay(5);
          yield();
        }
      }
    }
  }
}
