
/*
   Mute /Video button for Video Conferencing
*/
// This project was originally based off the Mute Button for MS teams
// https://create.arduino.cc/projecthub/kaneda-clone-2/mute-button-for-ms-teams-d4c881


#include <Keyboard.h>
#include <Bounce2.h>

Bounce audio = Bounce();
Bounce video = Bounce();

//keymap  https://www.oreilly.com/library/view/javascript-dhtml/9780596514082/apb.html


// use this option for Windows and Linux:
char ctrlKey = KEY_LEFT_CTRL;
char shftKey = KEY_LEFT_SHIFT;
char altKey = KEY_LEFT_ALT;
char winKey = KEY_LEFT_GUI;
//

const int ABUTTON_PIN = 16;        // Audio Button
const int VBUTTON_PIN = 10;        // Video Button

const int LEDAPIN = 14;
const int LEDVPIN = 15;

//

// wire with a common negative across all 8 on one side
// dip switch block of 8

const int OS_PIN1 = 9;   // two pins for the OS identification
const int OS_PIN2 = 8;
const int APP_PIN1 = 7;   // two pins for the App identification
const int APP_PIN2 = 6;
const int ID_PIN1 = 5;   //  if using the app id (keyboard "alt tab" like  switch to the app before sending keys)
const int ID_PIN2 = 4;
const int ID_PIN3 = 3;
const int DEBUG_PIN = 2;  // set to output the os/app/id to via key strokes (put in notepad or similar window when plugging in verify config)

#define WIN 0
#define MACOS 1
#define LINUX 2

#define MSTEAMS 0
#define ZOOM 1
#define GOOGLE 2

#define ON HIGH
#define OFF LOW

int currentAState;                // the current reading from the Audio input pin
int currentVState;                // the current reading from the Video input pin
int muteState;        // current knoen stats for A and V
int videoState;

int os;
int app;
int appid;
int sendid;
int debug;
void printSettings(int pos, int papp, int pappid) {
  type(48 + pos);
  type(':');
  type(48 + papp);
  type(':');
  type(48 + pappid);
  kprint(" OS:");
  switch (pos) {
    case WIN:
      kprint("Windows");
      break;
    case MACOS:
      kprint("MacOs");
      break;
    case LINUX:
      kprint("Linux");
      break;
  }
  kprint("  APP:");
  switch (papp) {
    case MSTEAMS:
      kprint("Microsoft Teams");
      break;
    case ZOOM:
      kprint("Zoom");
      break;
    case GOOGLE:
      kprint("Google Meet");
      break;
  }
  kprint(" AppID:");
  type(48 + pappid);
}
// for setting the LEDS for A / V
void ALED(int onoff) {
  digitalWrite(LEDAPIN, onoff);
}
void VLED(int onoff) {
  digitalWrite(LEDVPIN, onoff);
}

void resetkeys() {
  Keyboard.releaseAll();
  delay(50);
}
void kprint(String str) {
  resetkeys();
  Keyboard.print(str);
  delay(100);
}
// simple keyboard key send function
void type(int key) {
  resetkeys();
  Keyboard.press(key);
  delay(100);
  Keyboard.releaseAll();
}

void activateapp(int id) {

  //type('I');
  //type(48 + id);
  resetkeys();

  Keyboard.press(winKey);
  delay(50);
  Keyboard.press(48 + id);
  delay(300);
  Keyboard.releaseAll();
  delay (100);
  if (os == LINUX) {
    type(KEY_RETURN);
  }
  delay(400);
}

void setup() {
  // Serial.begin(115200);

  // initialize the pushbutton pin as an pull-up input
  // the pull-up input pin will be HIGH when the switch is open and LOW when the switch is closed.

  audio.attach ( ABUTTON_PIN , INPUT_PULLUP );
  video.attach ( VBUTTON_PIN , INPUT_PULLUP );
  audio.interval(100 );
  video.interval(100 );

  // this block is for the block of toggle switches

  pinMode(ABUTTON_PIN, INPUT_PULLUP);
  pinMode(VBUTTON_PIN, INPUT_PULLUP);
  pinMode(OS_PIN1, INPUT_PULLUP);
  pinMode(OS_PIN2, INPUT_PULLUP);
  pinMode(APP_PIN1, INPUT_PULLUP);
  pinMode(APP_PIN2, INPUT_PULLUP );
  pinMode(ID_PIN1, INPUT_PULLUP );
  pinMode(ID_PIN2, INPUT_PULLUP );
  pinMode(ID_PIN3, INPUT_PULLUP);
  pinMode(DEBUG_PIN, INPUT_PULLUP);


  pinMode(LEDAPIN, OUTPUT);
  pinMode(LEDVPIN, OUTPUT);
  digitalWrite(LEDAPIN, HIGH);
  digitalWrite(LEDVPIN, HIGH);

  //Serial.println("setup done");


  muteState = ON;
  videoState = ON;
  currentAState = HIGH;  // muted
  currentVState = HIGH;  // no video

  int debug = !digitalRead(DEBUG_PIN);
  os = !digitalRead(OS_PIN2) << 1 | !digitalRead(OS_PIN1);
  app = !digitalRead(APP_PIN2) << 1 | !digitalRead(APP_PIN1);
  appid = !digitalRead(ID_PIN3) << 2 | !digitalRead(ID_PIN2) << 1 | !digitalRead(ID_PIN1);
  delay(500);
  //blink LEDs to show its connecting, and set to muted as initial state.
  ALED(OFF);
  delay(200);
  ALED(ON);
  delay(200);
  VLED(OFF);
  delay(200);
  VLED(ON);
  delay(200);

  Keyboard.begin();
  delay(1000);
  if (debug == 1) {
    printSettings(os, app, appid);
  }


  // flip to app id when started to confirm it's connected to the conf app

  if (appid > 0) {
    activateapp(appid);
  }

}

void loop() {
  audio.update();
  video.update();

  // check to see if you just pressed the button
  // (i.e. the input went from LOW to HIGH), and you've waited long enough


  if (audio.changed()) {
    //  type('a');
    int audioDeboucedValue = audio.read();

    // on press of button not release..

    if (audioDeboucedValue == HIGH && currentAState == LOW) {
      //   type('g');
      if (muteState == LOW) {
        muteState = HIGH;
        ALED(ON);
      } else if (muteState == HIGH) {
        muteState = LOW;
        ALED(OFF);
      }
      //  Serial.println("Turn on Mute");
      //AUDIO
      switch (os) {
        case WIN:
          switch (app) {
            case MSTEAMS:
              if (appid > 0) {
                activateapp(appid);
              }
              Keyboard.press(ctrlKey);
              Keyboard.press(shftKey);
              Keyboard.press('o');

              break;
            case ZOOM:
              if (appid > 0) {
                activateapp(appid);
              }
              Keyboard.press(altKey);
              Keyboard.press('v');
              break;
            case GOOGLE:
              if (appid > 0) {
                activateapp(appid);
              }
              Keyboard.press(ctrlKey);
              Keyboard.press('e');
              break;
            default:
              break;
          }
          break;
        case MACOS:
          switch (app) {
            case MSTEAMS:
              if (appid > 0) {
                activateapp(appid);
              }
              Keyboard.press(winKey);
              Keyboard.press(shftKey);
              Keyboard.press('o');
              break;
            case ZOOM:
              if (appid > 0) {
                activateapp(appid);
              }
              Keyboard.press(winKey);
              Keyboard.press(shftKey);
              Keyboard.press('v');
              break;
            case GOOGLE:
              if (appid > 0) {
                activateapp(appid);
              }
              Keyboard.press(winKey);
              Keyboard.press('e');
              break;
            default:
              break;
          }
          break;
        case LINUX:
          switch (app) {
            case MSTEAMS:
              if (appid > 0) {
                activateapp(appid);
              }
              Keyboard.press(ctrlKey);
              Keyboard.press(shftKey);
              Keyboard.press('o');
              break;
            case ZOOM:
              if (appid > 0) {
                activateapp(appid);
              }
              Keyboard.press(altKey);
              Keyboard.press('v');
              break;
            case GOOGLE:
              if (appid > 0) {
                activateapp(appid);
              }
              Keyboard.press(ctrlKey);
              Keyboard.press('e');
              break;
            default:
              break;
          }
          break;
          //default:
          //  Serial.println("Unknown OS");
      }

      delay(100);
      Keyboard.releaseAll();
    }
    // save the the last steady state
    currentAState = audioDeboucedValue;
  }

  if (video.changed()) {

    int videoDeboucedValue = video.read();

    if (videoDeboucedValue == HIGH && currentVState == LOW) {
      if (videoState == LOW) {
        videoState = HIGH;
        VLED(ON);
      } else if (videoState == HIGH) {
        videoState = LOW;
        VLED(OFF);
      }
      //video
      switch (os) {
        case WIN:
          switch (app) {
            case MSTEAMS:
              if (appid > 0) {
                activateapp(appid);
              }
              Keyboard.press(ctrlKey);
              Keyboard.press(shftKey);
              Keyboard.press('m');
              break;
            case ZOOM:
              if (appid > 0) {
                activateapp(appid);
              }
              Keyboard.press(altKey);
              Keyboard.press('a');
              break;
            case GOOGLE:
              Keyboard.press(ctrlKey);
              Keyboard.press('d');
              break;
            default:
              break;
          }
          break;
        case MACOS:
          switch (app) {
            case MSTEAMS:
              if (appid > 0) {
                activateapp(appid);
              }
              Keyboard.press(winKey);
              Keyboard.press(shftKey);
              Keyboard.press('m');
              break;
            case ZOOM:
              if (appid > 0) {
                activateapp(appid);
              }
              Keyboard.press(winKey);
              Keyboard.press(shftKey);
              Keyboard.press('a');
              break;
            case GOOGLE:
              if (appid > 0) {
                activateapp(appid);
              }
              Keyboard.press(winKey);
              Keyboard.press('d');
              break;
            default:
              break;
          }
          break;
        case LINUX:
          switch (app) {
            case MSTEAMS:
              if (appid > 0) {
                activateapp(appid);
              }
              Keyboard.press(ctrlKey);
              Keyboard.press(shftKey);
              Keyboard.press('m');
              break;
            case ZOOM:
              if (appid > 0) {
                activateapp(appid);
              }
              Keyboard.press(altKey);
              Keyboard.press('a');
              break;
            case GOOGLE:
              if (appid > 0) {
                activateapp(appid);
              }
              Keyboard.press(ctrlKey);
              Keyboard.press('d');
              break;
            default:
              break;
          }
          break;
      }
      delay(100);
      Keyboard.releaseAll();
    }
    currentVState = videoDeboucedValue;
  }
}
