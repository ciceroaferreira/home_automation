/*********
  Complete project details at https://randomnerdtutorials.com
  
  This is an example for our Monochrome OLEDs based on SSD1306 drivers. Pick one up today in the adafruit shop! ------> http://www.adafruit.com/category/63_98
  This example is for a 128x32 pixel display using I2C to communicate 3 pins are required to interface (two I2C and one reset).
  Adafruit invests time and resources providing this open source code, please support Adafruit and open-source hardware by purchasing products from Adafruit!
  Written by Limor Fried/Ladyada for Adafruit Industries, with contributions from the open source community. BSD license, check license.txt for more information All text above, and the splash screen below must be included in any redistribution. 
*********/

#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

#define NUMFLAKES     10 // Number of snowflakes in the animation example

#define LOGO_HEIGHT   16
#define LOGO_WIDTH    16

const int TEMP_AGUA = 27;

static const unsigned char logo_bmp [] PROGMEM = {
  // 'piscina, 40x32px
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xc3, 0xff, 0xff, 
  0xff, 0xfc, 0x01, 0xff, 0xff, 0xff, 0xc0, 0x01, 0xff, 0xff, 0xff, 0xc0, 0x01, 0xff, 0xff, 0xff, 
  0x80, 0x0f, 0xff, 0xff, 0xff, 0xc0, 0xff, 0xff, 0xff, 0xff, 0xc1, 0xfc, 0x7f, 0xff, 0xff, 0xe0, 
  0xf0, 0x3f, 0xff, 0xff, 0xe0, 0xf0, 0x1f, 0xff, 0xff, 0xf0, 0x60, 0x1f, 0xff, 0xff, 0xe0, 0x20, 
  0x1f, 0xff, 0xff, 0x80, 0x30, 0x1f, 0xff, 0xfe, 0x00, 0x10, 0x3f, 0xff, 0xfc, 0x00, 0x18, 0x7f, 
  0xff, 0xf0, 0x00, 0x0f, 0xff, 0xff, 0xc0, 0x00, 0x07, 0xff, 0xff, 0x00, 0x00, 0x07, 0xff, 0xfe, 
  0x01, 0xe0, 0x1f, 0xff, 0xf1, 0x86, 0x18, 0x61, 0xff, 0xf0, 0xcc, 0x0c, 0xc0, 0xff, 0xf0, 0x38, 
  0x03, 0x80, 0x7f, 0xfe, 0x01, 0xe0, 0x1e, 0x0f, 0xff, 0x03, 0xf0, 0x3f, 0x0f, 0xf1, 0xfe, 0x1f, 
  0xe1, 0xff, 0xf0, 0x78, 0x07, 0x80, 0x7f, 0xf8, 0x30, 0x03, 0x00, 0x3f, 0xfe, 0x01, 0xe0, 0x1e, 
  0x0f, 0xff, 0x83, 0xf8, 0x7f, 0x0f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff
};

//SUN
#define pic_4_width 57
#define pic_4_height 32
static unsigned char pic_4_bits[] = {
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x10,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x78, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x7c, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x7c,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3c, 0x7c, 0x70, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x7e, 0x7c, 0xf8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x7e, 0x7c,
  0xfc, 0x00, 0x00, 0x00, 0x00, 0x00, 0xfc, 0xff, 0x7e, 0x00, 0x00, 0x00,
  0x00, 0x00, 0xf8, 0xff, 0x3f, 0x00, 0x00, 0x00, 0x00, 0x00, 0xf0, 0xff,
  0x1f, 0x00, 0x00, 0x00, 0x00, 0x00, 0xe0, 0x83, 0x0f, 0x00, 0x00, 0x00,
  0x00, 0x00, 0xe0, 0x01, 0x0f, 0x00, 0x00, 0x00, 0x00, 0x00, 0xe0, 0x01,
  0x1e, 0x00, 0x00, 0x00, 0x00, 0xc0, 0xff, 0x00, 0xfe, 0x07, 0x00, 0x00,
  0x00, 0xc0, 0xff, 0x00, 0xfe, 0x0f, 0x00, 0x00, 0x00, 0xc0, 0xff, 0x00,
  0xfe, 0x0f, 0x00, 0x00, 0x00, 0xc0, 0xff, 0x00, 0xfe, 0x07, 0x00, 0x00,
  0x00, 0x00, 0xe0, 0x01, 0x1e, 0x00, 0x00, 0x00, 0x00, 0x00, 0xe0, 0x03,
  0x0f, 0x00, 0x00, 0x00, 0x00, 0x00, 0xe0, 0x87, 0x0f, 0x00, 0x00, 0x00,
  0x00, 0x00, 0xf0, 0xff, 0x1f, 0x00, 0x00, 0x00, 0x00, 0x00, 0xf8, 0xff,
  0x7f, 0x00, 0x00, 0x00, 0x00, 0x00, 0xfc, 0x7c, 0x7e, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x7e, 0x7c, 0xfc, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3e, 0x7c,
  0xf8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3c, 0x7c, 0x70, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x7c, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x7c,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x78, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00
};

//DISLIKE
const unsigned char myBitmap [] PROGMEM = {
  // 'Dislike_36, 36x36px
  0xff, 0xff, 0xff, 0xff, 0xf0, 0x80, 0x00, 0x00, 0x07, 0xf0, 0x80, 0x00, 0x00, 0x01, 0xf0, 0x80, 
  0x0f, 0xff, 0xf0, 0xf0, 0x80, 0x0f, 0xff, 0xfc, 0x70, 0x80, 0x0f, 0xff, 0xfe, 0x70, 0x87, 0x0f, 
  0xff, 0xfe, 0x70, 0x87, 0x0f, 0xff, 0xfe, 0x70, 0x86, 0x0f, 0xff, 0xff, 0x30, 0x80, 0x0f, 0xff, 
  0xff, 0x30, 0x80, 0x0f, 0xff, 0xff, 0x30, 0x80, 0x0f, 0xff, 0xff, 0x30, 0x80, 0x0f, 0xff, 0xff, 
  0x30, 0x80, 0x0f, 0xff, 0xff, 0x30, 0x80, 0x0f, 0xff, 0xff, 0x10, 0x80, 0x0f, 0xff, 0xff, 0x90, 
  0x80, 0x0f, 0xff, 0xff, 0x90, 0x80, 0x0f, 0xff, 0xff, 0x10, 0x80, 0x0f, 0xff, 0xff, 0x30, 0x80, 
  0x07, 0xff, 0xff, 0x30, 0xff, 0xe3, 0xff, 0xff, 0x30, 0xff, 0xf1, 0xff, 0xff, 0x30, 0xff, 0xf8, 
  0xfe, 0x0e, 0x30, 0xff, 0xfc, 0xfc, 0x00, 0x70, 0xff, 0xfe, 0x7c, 0xf0, 0xf0, 0xff, 0xfe, 0x7c, 
  0xff, 0xf0, 0xff, 0xff, 0x3c, 0xff, 0xf0, 0xff, 0xff, 0x3c, 0xff, 0xf0, 0xff, 0xff, 0x3c, 0xff, 
  0xf0, 0xff, 0xff, 0x9c, 0xff, 0xf0, 0xff, 0xff, 0x9c, 0xff, 0xf0, 0xff, 0xff, 0xcc, 0xff, 0xf0, 
  0xff, 0xff, 0xc4, 0xff, 0xf0, 0xff, 0xff, 0xe0, 0xff, 0xf0, 0xff, 0xff, 0xf1, 0xff, 0xf0, 0xff, 
  0xff, 0xff, 0xff, 0xf0
};

void setup() {
  Serial.begin(115200);

  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { 
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }

  // Show initial display buffer contents on the screen --
  // the library initializes this with an Adafruit splash screen.
  display.display();
  delay(2000); // Pause for 2 seconds

  // Clear the buffer
  display.clearDisplay();

  // Draw a single pixel in white
  display.drawPixel(10, 10, WHITE);

  // Show the display buffer on the screen. You MUST call display() after
  // drawing commands to make them visible on screen!
  display.display();
  delay(2000);
  // display.display() is NOT necessary after every single drawing command,
  // unless that's what you want...rather, you can batch up a bunch of
  // drawing operations and then update the screen all at once by calling
  // display.display(). These examples demonstrate both approaches...

  //testdrawline();      // Draw many lines

  //testdrawrect();      // Draw rectangles (outlines)

  //testfillrect();      // Draw rectangles (filled)

  //testdrawcircle();    // Draw circles (outlines)

  //testfillcircle();    // Draw circles (filled)

  //testdrawroundrect(); // Draw rounded rectangles (outlines)

  //testfillroundrect(); // Draw rounded rectangles (filled)

  //testdrawtriangle();  // Draw triangles (outlines)

  //testfilltriangle();  // Draw triangles (filled)

  //testdrawchar();      // Draw characters of the default font

  //testdrawstyles();    // Draw 'stylized' characters

  //testscrolltext();    // Draw scrolling text

  //testdrawbitmap();    // Draw a small bitmap image

  // Invert and restore display, pausing in-between
  display.invertDisplay(true);
  delay(1000);
  display.invertDisplay(false);
  delay(1000);

  //testanimate(pic_4_bits, LOGO_WIDTH, LOGO_HEIGHT); // Animate bitmaps
}

void loop() {
  testdrawstyles();
  delay(15000);
  testscrolltext();
  //testanimate(pic_4_bits, LOGO_WIDTH, LOGO_HEIGHT); // Animate bitmaps
}


//DESENHA A MOLDURA
void testdrawrect(void) {
  display.clearDisplay();

  display.drawRect(1, 1, display.width()-2, display.height()-2, WHITE);
  display.display();
  delay(2000);
}


// LOGICA
void testdrawstyles(void) {
  
  testdrawrect();
  
  display.setTextSize(1);             // Normal 1:1 pixel scale
  display.setTextColor(WHITE);        // Draw white text
  display.setCursor(20,7);             // Start at top-left corner
  display.println(F("Temp. Piscina"));

  display.setTextSize(4);             // Draw 2X-scale text
  display.setTextColor(WHITE);
  display.setCursor(60,18);
  display.println(TEMP_AGUA);

  display.setTextSize(1);             // Normal 1:1 pixel scale
  display.setTextColor(WHITE);        // Draw white text  
  display.setCursor(65,50);
  display.println(F("graus"));
  display.display();

  if (TEMP_AGUA >= 27) {
    display.drawBitmap(10, 20, logo_bmp, 40, 32, WHITE); // display.drawBitmap(x position, y position, bitmap data, bitmap width, bitmap height, color)
  }
  if (TEMP_AGUA < 27) {
    display.drawBitmap(10, 20, myBitmap, 36, 36, BLACK,WHITE); // display.drawBitmap(x position, y position, bitmap data, bitmap width, bitmap height, color)
  }

  
  display.display();


  //myBitmap
  //testdrawbitmap();
 
}

void testscrolltext(void) {
  display.clearDisplay();

  display.setTextSize(2); // Draw 2X-scale text
  display.setTextColor(WHITE);
  display.setCursor(60,18);
  if (TEMP_AGUA >= 27) {
    display.drawBitmap(10, 20, logo_bmp, 40, 32, WHITE); // display.drawBitmap(x position, y position, bitmap data, bitmap width, bitmap height, color)
  }
  if (TEMP_AGUA < 27) {
    display.drawBitmap(10, 20, myBitmap, 36, 36, BLACK,WHITE); // display.drawBitmap(x position, y position, bitmap data, bitmap width, bitmap height, color)
  }
  display.display();
  delay(100);

  // Scroll in various directions, pausing in-between:
  display.startscrollright(0x00, 0x0F);
  delay(1000); //2000
  display.stopscroll();
  delay(1000);
  display.startscrollleft(0x00, 0x0F);
  delay(1000); //2000
  display.stopscroll();
  delay(1000);
  display.startscrolldiagright(0x00, 0x07);
  delay(1000); //2000
  display.startscrolldiagleft(0x00, 0x07);
  delay(1000); //2000
  display.stopscroll();
  delay(1000);
}

#define XPOS   0 // Indexes into the 'icons' array in function below
#define YPOS   1
#define DELTAY 2

void testanimate(const uint8_t *bitmap, uint8_t w, uint8_t h) {
  int8_t f, icons[NUMFLAKES][3];

  // Initialize 'snowflake' positions
  for(f=0; f< NUMFLAKES; f++) {
    icons[f][XPOS]   = random(1 - LOGO_WIDTH, display.width());
    icons[f][YPOS]   = -LOGO_HEIGHT;
    icons[f][DELTAY] = random(1, 6);
    Serial.print(F("x: "));
    Serial.print(icons[f][XPOS], DEC);
    Serial.print(F(" y: "));
    Serial.print(icons[f][YPOS], DEC);
    Serial.print(F(" dy: "));
    Serial.println(icons[f][DELTAY], DEC);
  }

  for(;;) { // Loop forever...
    display.clearDisplay(); // Clear the display buffer

    // Draw each snowflake:
    for(f=0; f< NUMFLAKES; f++) {
      display.drawBitmap(icons[f][XPOS], icons[f][YPOS], bitmap, w, h, WHITE);
    }

    display.display(); // Show the display buffer on the screen
    delay(200);        // Pause for 1/10 second

    // Then update coordinates of each flake...
    for(f=0; f< NUMFLAKES; f++) {
      icons[f][YPOS] += icons[f][DELTAY];
      // If snowflake is off the bottom of the screen...
      if (icons[f][YPOS] >= display.height()) {
        // Reinitialize to a random position, just off the top
        icons[f][XPOS]   = random(1 - LOGO_WIDTH, display.width());
        icons[f][YPOS]   = -LOGO_HEIGHT;
        icons[f][DELTAY] = random(1, 6);
      }
    }
  }
}