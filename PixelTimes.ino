#include "Configuration.h"
#include "anim_data.h"

#include <HTTPClient.h>

#include <Time.h>
#include <TimeLib.h>
#include <WebServer.h>
//#include <ESP8266WebServer.h>
#include <WiFiManager.h>
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

//void saveConfigCallback () {
//  Serial.println("Should save config");
//  shouldSaveWifiConfig = true;
//}
//void configModeCallback (WiFiManager *myWiFiManager) {
//  Serial.println("Entered config mode");
//  Serial.println(WiFi.softAPIP());
//  Serial.println(myWiFiManager->getConfigPortalSSID());
//}
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
  // delay(5000);
  Serial.println("Setup");
////  wifi_station_set_hostname(const_cast<char*>(HOSTNAME));
//  WiFiManager wifiManager;
////  wifiManager.resetSettings(); 
////  wifiManager.setSaveConfigCallback(saveConfigCallback);
////  wifiManager.setAPCallback(configModeCallback);
//  wifiManager.setMinimumSignalQuality();
//  wifiManager.setTimeout(300);
////  WiFi.setSleepMode(WIFI_NONE_SLEEP);
//  Serial.println ("start wifi connection");
////  wifiManager.resetSettings(); // for testing / debugging
//  if (!wifiManager.autoConnect("PixelTime", "foobar123")) {
//    Serial.println("failed to connect and hit timeout");
//    delay(3000);
//    ESP.restart();
//    delay(5000);
//  }
//  Serial.println ("wifi connected ok");
//  Serial.println(WiFi.localIP());
//
//  NTPSetup();

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
  buffer_drawer.setFont(&TomThumb);

//  display_ticker.attach(0.001, display_updater);
  display_update_enable(true);
//  attachInterrupt(digitalPinToInterrupt(0),button_pressed,FALLING);
//  dimm=1;
  // yield();
  delay(3000);
  Serial.println("End of setup");
}

// // This draws the weather icons and temperature
void draw_weather_icon (uint8_t icon, uint8_t location, int temp,bool ab)
{
  buffer_drawer.setFont(&TomThumb);

  if (location>2)
  location=2;

  if (icon>10)
  icon=10;
  for (int yy=0; yy<10;yy++)
  {
    for (int xx=0; xx<10;xx++)
    {
      uint16_t byte_pos=(xx+icon*10)*2+yy*220;
      if (ab){
        this_single_double.two[1]=weather_icons_a[byte_pos];
        this_single_double.two[0]=weather_icons_a[byte_pos+1];
      }
      else
      {
        this_single_double.two[1]=weather_icons_b[byte_pos];
        this_single_double.two[0]=weather_icons_b[byte_pos+1];
      }
      buffer_drawer.drawPixel(1+xx+location*12,yy,this_single_double.one);
    }

  }

  int pixel_shift=0;
  if ((temp>-10)&&(temp<10)) {
    pixel_shift=2;
  }

  if (location==0) {
    buffer_drawer.setCursor(2+pixel_shift,16);
  } else {
    buffer_drawer.setCursor(14+pixel_shift,16);
  }

  if (temp<0)
  {
    temp=temp*-1;
    if (location==0) {
      buffer_drawer.drawPixel(pixel_shift,13,myWHITE);
      buffer_drawer.drawPixel(pixel_shift+1,13,myWHITE);
    } else {
      buffer_drawer.drawPixel(12+pixel_shift,13,myWHITE);
      buffer_drawer.drawPixel(12+pixel_shift+1,13,myWHITE);
    }
  }
  buffer_drawer.println(temp);
}

// This draws the time for the weather view
void draw_time_weather ()
{
  uint8_t this_hour= hour();
  uint8_t this_minute= minute();
  uint8_t this_second= second();
  buffer_drawer.setFont(&TomThumb);
  buffer_drawer.setTextColor(myWHITE);
  buffer_drawer.setCursor(25,6);
  if (this_hour<10)
  buffer_drawer.println("0"+String(this_hour));
  else
  buffer_drawer.println(this_hour);
  buffer_drawer.setCursor(25,16);

  if (this_minute<10)
  buffer_drawer.println("0"+String(this_minute));
  else
  buffer_drawer.println(this_minute);

  // Dots
  buffer_drawer.drawPixel(27,8,myWHITE);
  buffer_drawer.drawPixel(29,8,myWHITE);

}

// This draws the pixel animation to the frame buffer in animation view
// void draw_animation ()
// {
//   uint32_t i=0;
//   for (int yy=0;yy<16;yy++){
//     for (int xx=0;xx<32;xx++){
// #if RGB==565
//       this_single_double.two[0]=buffer_drawer.frame_buffer[i];
//       this_single_double.two[1]=buffer_drawer.frame_buffer[i+1];

//       display.drawPixelRGB565(xx,yy, this_single_double.one);
//       i=i+2;
// #else
//       display.drawPixelRGB888(xx,yy,buffer_drawer.frame_buffer[i],buffer_drawer.frame_buffer[i+1],buffer_drawer.frame_buffer[i+2]);
//       i=i+3;
// #endif
//     }
//   }
// }

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
  buffer_drawer.setCursor(15,6);
  buffer_drawer.setInvertTextColor(true);
  buffer_drawer.print(time_string);
  buffer_drawer.setInvertTextColor(false);
}

void loop1() {
//  display.clearDisplay();
  // display.fillScreen(myBLACK);


  // display.setTextColor(myCYAN);
  // display.setCursor(6, 5);
  // display.print("HELLO");

  // if(Serial.available() > 0) {
  //   String c = Serial.readString();
  //   display.print(c);
  // }
  // Serial.println("loop");
  // buffer_drawer.fillScreen(myBLACK);
  // buffer_drawer.setCursor(1,6);
  // buffer_drawer.println(millis());
  // draw_animation();
  // yield();
  delay(100);
}

int test_i  = 0;
int test_ab = false;

void loop() {
  if(Serial.available() > 0) {
    Serial.readString();
    show_weather = !show_weather;
  }
  // Serial.println("loop");
  // // buffer_drawer.fillScreen(myBLACK);
  // // memset(buffer_drawer.frame_buffer,0,frame_size);
  // buffer_drawer.clearScreen();
  // buffer_drawer.setCursor(1,6);
  // buffer_drawer.setTextColor(myBLUE);
  // String a;
  // a += String(millis());
  // buffer_drawer.print(a);
  // render_frame();
  // delay(100);
  // return;

  if (show_weather)
  {

    bool ret_code;
    unsigned long this_time=millis();
    yield();


    if(this_time > next_weather_update) {
      next_weather_update = this_time + 5000;
      if (test_ab == true) {
        test_i++;
        test_i = test_i % 11; 
        test_ab = false;
      }
      else {
        test_ab = true;
      }
      Serial.println(test_i);
      // Serial.println(test_ab);
      Serial.println(test_ab ? "true" : "false");
    }

    buffer_drawer.clearScreen();
    draw_weather_icon(test_i, 0, -12, test_ab);
    draw_weather_icon(test_i, 1, -23, test_ab);
    draw_time_weather();

    render_frame();


//    brightness=brightness+dimm;
//    if (brightness<BRIGHTNESS_MIN)
//    {
//      brightness=BRIGHTNESS_MIN;
//      dimm=+1;
//    }
//  
//    if (brightness>BRIGHTNESS_MAX)
//    {
//      brightness=BRIGHTNESS_MAX;
//      dimm=-1;
//    }
//    display.setBrightness(brightness);

    
    // Update weather data every 10 minutes
//    if (this_time>next_weather_update)
//    {
//      ret_code=update_weather();
//      if (ret_code)
//      next_weather_update=this_time+600000;
//      else
//      next_weather_update=this_time+5000;
//    }

    // display.clearDisplay();
    // draw_time_weather ();

//    // Exchange sun for moon icon when past sunset
//    if ((sunset<now()) && (icon_show_low[0]<2))
//    icon_show_low[0]=icon_show_low[0]+9;
//    if ((sunset<now()) && (icon_show_high[0]<2))
//    icon_show_high[0]=icon_show_high[0]+9;
//
//    // Flip between icons every 2 seconds to animate them a bit
//    // Flip between high and lows every 5 seconds
//    if ((second()%20)<10)
//    {
//      // Dot to indicate high or low
//      display.drawPixel(0,15,myWHITE);
//      draw_weather_icon(icon_show_low[0],0,temperature_show_low[0],(second()%4>=2));
//      draw_weather_icon(icon_show_low[0],0,temperature_show_low[0],(second()%4<2));
//      draw_weather_icon(icon_show_low[1],1,temperature_show_low[1],(second()%4>=2));
//      draw_weather_icon(icon_show_low[1],1,temperature_show_low[1],(second()%4<2));
//    }
//    else
//    {
//      draw_weather_icon(icon_show_high[0],0,temperature_show_high[0],(second()%4>=2));
//      draw_weather_icon(icon_show_high[0],0,temperature_show_high[0],(second()%4<2));
//      draw_weather_icon(icon_show_high[1],1,temperature_show_high[1],(second()%4>=2));
//      draw_weather_icon(icon_show_high[1],1,temperature_show_high[1],(second()%4<2));
//      // Dot to indicate high or low
//      display.drawPixel(0,11,myWHITE);
//    }
//
//    // If we had problems fetching the weather we display a red dot
//    if (weather_get_error)
//    display.drawPixel(0,0,myRED);

// TODO
//    process_ntp();

    if (!show_weather)
      return;
    
    yield();
    delay(100);
    
//
//
  }
  else
  {

    // Random animation file
    randomSeed(micros());
    uint8_t jump=random(no_files);
    unsigned long last_animation_change = millis();

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

        // Copy current frame to frame buffer
        unsigned long draw_start_time = millis();

        render_frame();
         unsigned long draw_duration = millis()-draw_start_time;
        //Serial.println("Draw time" + String(draw_duration));

        unsigned long frame_end_time = millis();


        // Fetch new frame byte by byte - otherwise the fs will lock
        // and prevent timely display updates
        unsigned long start_read = micros();

#ifdef SPIFFS_ENABLE
        for (int this_byte=0;this_byte<frame_size;this_byte++){
          ff.read(buffer_drawer.frame_buffer+this_byte,1);
           yield();
        }
#else
        memcpy_P(buffer_drawer.frame_buffer,animations+(frame_offset+frame)*frame_size,frame_size);
#endif
        unsigned long read_latency=micros()-start_read;
        //if (read_latency>500)
           //Serial.println("Read latency problem, latency: " + String(read_latency));

        // Write time to frame buffer
        draw_time ();
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
          //delay(1);
          yield();

        }
      }
    }
  }
}
