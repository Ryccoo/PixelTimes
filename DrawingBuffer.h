// Printing text with Adafruit GFX takes forever, so we create a frame buffer where
// we construct the frame (off-line) and then write it to the display

#ifndef DrawingBuffer_H
#define DrawingBuffer_H

#include "Configuration.h"


union single_double{
  uint8_t two[2];
  uint16_t one;
} this_single_double;

class DrawingBuffer : public Adafruit_GFX {
public:
  uint8_t frame_buffer[frame_size]={0};
  DrawingBuffer() : Adafruit_GFX(40,16) {}
  uint8_t bg_color[3];
  bool invert_color = false;

  void render() {

  }

  void setInvertTextColor(bool invert) {
  	invert_color = invert;
  }

  void clearScreen() {
  	memset(frame_buffer,0,frame_size);
  }

  // We need that one so we can call ".print"
  void drawPixel(int16_t x, int16_t y, uint16_t color)
  {
    if ((x>31)| (y>15))
      return;

#if RGB==565
  	if(!invert_color) {
  		this_single_double.one = color;
		frame_buffer[x*2+y*64]=this_single_double.two[0];
	    frame_buffer[x*2+y*64+1]=this_single_double.two[1];	
	    return;
  	}	

    this_single_double.two[0]=frame_buffer[40];
    this_single_double.two[1]=frame_buffer[41];
    uint8_t r = ((((this_single_double.one >> 11) & 0x1F) * 527) + 23) >> 6;
    uint8_t g = ((((this_single_double.one >> 5) & 0x3F) * 259) + 33) >> 6;
    uint8_t b = (((color & 0x1F) * 527) + 23) >> 6;
    // We only do black or white for the writing
    if (((r+g+b)/3)<120)
    {
      frame_buffer[x*2+y*64]=255;
      frame_buffer[x*2+y*64+1]=255;

    }
    else
    {
      frame_buffer[x*2+y*64]=0;
      frame_buffer[x*2+y*64+1]=0;
    }
#else
    // We only do black or white for the writing
    if (((frame_buffer[60]+frame_buffer[61]+frame_buffer[62])/3)<120)
    {
      frame_buffer[x*3+y*96]=255;
      frame_buffer[x*3+y*96+1]=255;
      frame_buffer[x*3+y*96+2]=255;
    }
    else
    {
      frame_buffer[x*3+y*96]=0;
      frame_buffer[x*3+y*96+1]=0;
      frame_buffer[x*3+y*96+2]=0;
    }
#endif
  }


};


#endif