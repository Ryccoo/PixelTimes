#ifndef Display_H
#define Display_H

#include "Configuration.h"
#include "DrawingBuffer.h"

#define P_LAT 22
#define P_A 19
#define P_B 23
#define P_C 18
#define P_D 5
#define P_E 15
#define P_OE 2



PxMATRIX display(32,16,P_LAT, P_OE,P_A,P_B,P_C);
DrawingBuffer buffer_drawer;
int dimm=0;

Ticker display_ticker;
hw_timer_t * timer = NULL;
portMUX_TYPE timerMux = portMUX_INITIALIZER_UNLOCKED;
uint8_t display_draw_time=60;

// Some standard colors
uint16_t myRED = display.color565(255, 0, 0);
uint16_t myGREEN = display.color565(0, 255, 0);
uint16_t myBLUE = display.color565(0, 0, 255);
uint16_t myWHITE = display.color565(255, 255, 255);
uint16_t myYELLOW = display.color565(255, 255, 0);
uint16_t myCYAN = display.color565(0, 255, 255);
uint16_t myMAGENTA = display.color565(255, 0, 255);
uint16_t myBLACK = display.color565(0, 0, 0);
uint16_t myCOLORS[8]={myRED,myGREEN,myBLUE,myWHITE,myYELLOW,myCYAN,myMAGENTA,myBLACK};

void render_frame ()
{
  uint32_t i=0;
  for (int yy=0;yy<16;yy++){
    for (int xx=0;xx<32;xx++){
#if RGB==565
      this_single_double.two[0]=buffer_drawer.frame_buffer[i];
      this_single_double.two[1]=buffer_drawer.frame_buffer[i+1];

      display.drawPixelRGB565(xx,yy, this_single_double.one);
      i=i+2;
#else
      display.drawPixelRGB888(xx,yy,buffer_drawer.frame_buffer[i],buffer_drawer.frame_buffer[i+1],buffer_drawer.frame_buffer[i+2]);
      i=i+3;
#endif
    }
  }
}

#endif


#ifdef ESP32
void IRAM_ATTR display_updater(){
//  // Increment the counter and set the time of ISR
  brightness=brightness+dimm;
  if (brightness<BRIGHTNESS_MIN)
  {
   brightness=0;
   dimm=0;
  }

  if (brightness>BRIGHTNESS_MAX)
  {
   brightness=BRIGHTNESS_MAX;
   dimm=0;
  }
  display.setBrightness(brightness);

  portENTER_CRITICAL_ISR(&timerMux);
  display.display(display_draw_time);
  portEXIT_CRITICAL_ISR(&timerMux);
}
#endif



void display_update_enable(bool is_enable)
{

#ifdef ESP8266
  if (is_enable)
    display_ticker.attach(0.004, display_updater);
  else
    display_ticker.detach();
#endif

#ifdef ESP32
  Serial.println("Display update enable fun");
  if (is_enable)
  {
    timer = timerBegin(0, 80, true);
    timerAttachInterrupt(timer, &display_updater, true);
    timerAlarmWrite(timer, 4000, true);
    timerAlarmEnable(timer);
  }
  else
  {
    timerDetachInterrupt(timer);
    timerAlarmDisable(timer);
  }
#endif
}