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

PxMATRIX display(32,16, P_LAT, P_OE,P_A,P_B,P_C);
DrawingBuffer buffer_drawer;


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