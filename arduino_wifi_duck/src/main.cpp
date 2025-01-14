#include "main.h"
#include <Arduino.h>


//#include <Keyboard_CM.h>
//Modified by TheMMcOfficial
#include <Keyboard.h>
//Works with the US keyboard by default other layout are not supported...
//Modified by TheMMcOfficial
#include <SPI.h>
#include <SD.h>
char myChar;
char command[255];
//Modified by james.  Add  mouse function-------
#include <Mouse.h>
#define SCREEN_MAX_X 2560
#define SCREEN_MAX_Y 1440
//-----------------------------
#define BAUD_RATE 57200

#define ExternSerial Serial1

String bufferStr = "";
String last = "";
//Modified by TheMMcOfficial
String FileName;
File myFile;

int defaultDelay = 0;

int range=5;   //Between 1~127
void mouse_init()
{
  for(int x=0;x*range<=SCREEN_MAX_X;x++)
  Mouse.move(-range,0);
  for(int y=0;y*range<=SCREEN_MAX_Y;y++)
  Mouse.move(0,-range);
  }

void mouse_move(int px,int py)
{
  mouse_init();
  delay(300);
  for(int x=0;x*range<=px;x++)
  Mouse.move(range,0);
  for(int y=0;y*range<=py;y++)
  Mouse.move(0,range);
  }

void Press(String b){
  if(b.length() == 1) Keyboard.press(char(b[0]));
  else if (b.equals("ENTER")) Keyboard.press(KEY_RETURN);
  else if (b.equals("CTRL")) Keyboard.press(KEY_LEFT_CTRL);
  else if (b.equals("SHIFT")) Keyboard.press(KEY_LEFT_SHIFT);
  else if (b.equals("ALT")) Keyboard.press(KEY_LEFT_ALT);
  else if (b.equals("GUI")) Keyboard.press(KEY_LEFT_GUI);
  else if (b.equals("UP") || b.equals("UPARROW")) Keyboard.press(KEY_UP_ARROW);
  else if (b.equals("DOWN") || b.equals("DOWNARROW")) Keyboard.press(KEY_DOWN_ARROW);
  else if (b.equals("LEFT") || b.equals("LEFTARROW")) Keyboard.press(KEY_LEFT_ARROW);
  else if (b.equals("RIGHT") || b.equals("RIGHTARROW")) Keyboard.press(KEY_RIGHT_ARROW);
  else if (b.equals("DELETE")) Keyboard.press(KEY_DELETE);
  else if (b.equals("PAGEUP")) Keyboard.press(KEY_PAGE_UP);
  else if (b.equals("PAGEDOWN")) Keyboard.press(KEY_PAGE_DOWN);
  else if (b.equals("HOME")) Keyboard.press(KEY_HOME);
  else if (b.equals("ESC")) Keyboard.press(KEY_ESC);
  else if (b.equals("BACKSPACE")) Keyboard.press(KEY_BACKSPACE);
  else if (b.equals("INSERT")) Keyboard.press(KEY_INSERT);
  else if (b.equals("TAB")) Keyboard.press(KEY_TAB);
  else if (b.equals("END")) Keyboard.press(KEY_END);
  else if (b.equals("CAPSLOCK")) Keyboard.press(KEY_CAPS_LOCK);
  else if (b.equals("F1")) Keyboard.press(KEY_F1);
  else if (b.equals("F2")) Keyboard.press(KEY_F2);
  else if (b.equals("F3")) Keyboard.press(KEY_F3);
  else if (b.equals("F4")) Keyboard.press(KEY_F4);
  else if (b.equals("F5")) Keyboard.press(KEY_F5);
  else if (b.equals("F6")) Keyboard.press(KEY_F6);
  else if (b.equals("F7")) Keyboard.press(KEY_F7);
  else if (b.equals("F8")) Keyboard.press(KEY_F8);
  else if (b.equals("F9")) Keyboard.press(KEY_F9);
  else if (b.equals("F10")) Keyboard.press(KEY_F10);
  else if (b.equals("F11")) Keyboard.press(KEY_F11);
  else if (b.equals("F12")) Keyboard.press(KEY_F12);
  else if (b.equals("SPACE")) Keyboard.press(' ');
  else if (b.equals("LCLK")) Mouse.click();
  else if (b.equals("RCLK")) Mouse.click(MOUSE_RIGHT);
  else if (b.equals("MCLK")) Mouse.click(MOUSE_MIDDLE);
  else if (b.equals("LRCLK") or b.equals("RLCLK")) Mouse.click(MOUSE_RIGHT|MOUSE_LEFT);
  else if (b.equals("LDWN"))
  { 
    if (!Mouse.isPressed(MOUSE_LEFT)) {
      Mouse.press(MOUSE_LEFT);
      }
   }
  else if (b.equals("LUP"))
    {
      if (Mouse.isPressed(MOUSE_LEFT)) {
      Mouse.release(MOUSE_LEFT);
      }
    }
 else if (b.equals("RDWN"))
    {
     if (!Mouse.isPressed(MOUSE_RIGHT)) {
      Mouse.press(MOUSE_RIGHT);
  } 
     }
   else if (b.equals("RUP"))
   {
      if (Mouse.isPressed(MOUSE_RIGHT)) {
      Mouse.release(MOUSE_RIGHT);
      }
    }
   else if (b.equals("MDWN"))
   {
     if (!Mouse.isPressed(MOUSE_MIDDLE)) {
      Mouse.press(MOUSE_MIDDLE);
     } 
   }
   else if (b.equals("MUP"))
    {
      if (Mouse.isPressed(MOUSE_MIDDLE)) {
      Mouse.release(MOUSE_MIDDLE);
      }
   }
}
 
void Line(String _line)
{
  int firstSpace = _line.indexOf(" ");
  if(firstSpace == -1) Press(_line);
  else if(_line.substring(0,firstSpace) == "STRING"){
    for(int i=firstSpace+1;i<_line.length();i++) Keyboard.write(_line[i]);
  }
  else if(_line.substring(0,firstSpace) == "DELAY"){
    int delaytime = _line.substring(firstSpace + 1).toInt();
    delay(delaytime);
  }
  else if(_line.substring(0,firstSpace) == "DEFAULTDELAY") defaultDelay = _line.substring(firstSpace + 1).toInt();
  else if(_line.substring(0,firstSpace) == "REM"){} //nothing :/
  //  Add by james. for mouse moving------------------------------
  else if(_line.substring(0,firstSpace) == "MOVE")
  {
    int comma = _line.indexOf(","); 
    int point_x = _line.substring(firstSpace+1,comma).toInt();
    int point_y = _line.substring(comma+1).toInt();
    mouse_move(point_x,point_y);
    }

   //---------------------------------------------------------
  else if(_line.substring(0,firstSpace) == "REPLAY") {
    int replaynum = _line.substring(firstSpace + 1).toInt();
    while(replaynum)
    {
      Line(last);
      --replaynum;
    }
  } else if (_line.substring(0,firstSpace) == "EXECSD") //add sd read card capability by TheMMcOfficial
    {
      FileName = _line.substring(firstSpace +1);
      myFile = SD.open(FileName);
      delay(500);
      ExecScript();
    } else{
      String remain = _line;

      while(remain.length() > 0){
        int latest_space = remain.indexOf(" ");
        if (latest_space == -1){
          Press(remain);
          remain = "";
        }
        else{
          Press(remain.substring(0, latest_space));
          remain = remain.substring(latest_space + 1);
        }
        delay(5);
      }
  }

  Keyboard.releaseAll();
  delay(defaultDelay);
}

void setup() {
  
  Serial.begin(BAUD_RATE);
  ExternSerial.begin(BAUD_RATE);

  pinMode(13,OUTPUT);
  digitalWrite(13,HIGH);

  Keyboard.begin();
  Mouse.begin();

  if (!SD.begin(4)) {
  }
}

void loop() {
  if(ExternSerial.available()) {
    bufferStr = ExternSerial.readStringUntil(255);
    Serial.println(bufferStr);
  }
  
  if(bufferStr.length() > 0){
    
    bufferStr.replace("\r","\n");
    bufferStr.replace("\n\n","\n");
    
    while(bufferStr.length() > 0){
      int latest_return = bufferStr.indexOf("\n");
      if(latest_return == -1){
        Serial.println("run: "+bufferStr);
        Line(bufferStr);
        bufferStr = "";
      } else{
        Serial.println("run: '"+bufferStr.substring(0, latest_return)+"'");
        Line(bufferStr.substring(0, latest_return));
        last=bufferStr.substring(0, latest_return);
        bufferStr = bufferStr.substring(latest_return + 1);
      }
    }
    
    bufferStr = "";
    ExternSerial.write(0x99);
    Serial.println("done");
  }
}

//Add by TheMMcOfficial
void ExecScript(void) {
  int i; 
  if (myFile) {
    i = 0;

    while (myFile.available()) {
      myChar = myFile.read();

      if ((myChar == (13)) || (myChar == '\n')) {
        command[i] = '\x00';
        if (strlen(command) > 0) {
          delay(500);
          Serial.println(command); //for debugging
          Line(String(command));
        }
        i = 0;
      }
      else
      {
        command[i] = myChar;
        i++;
      }
    }
    myFile.close();
  }
}
