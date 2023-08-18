#include "SimpleMenu.h"
#include "Button2.h"
#include "Free_Fonts.h"
#include <TFT_eSPI.h> 

#define IO868VERSION "v0.4"
#define BUTTON_UP  0
#define BUTTON_DOWN  47
#define LONGCLICK_MS 300
#define DOUBLECLICK_MS 300

#define WIDTH  128
#define HEIGHT 128

Button2 butUp = Button2(BUTTON_UP);
Button2 butDown = Button2(BUTTON_DOWN);

bool needsRefresh = true;
bool needsExitFromAction = false;
bool menuActive = false;
long lastClick;

//TODO: REFACTOR
#define MAXSIGS 10
int pcurrent = 0;
int posH = 2;
int menu_name_length;



SimpleMenu* active_menu = NULL;
TFT_eSPI tft = TFT_eSPI();

void SMN_printAt(String s, int x, int y) {
  tft.drawString(s, x, y, GFXFF);
  Serial.println(s);
}

void SMN_alert(String msg, int wait, int timeout) {
  delay(wait);
  tft.setFreeFont(FMBO9);
  tft.setTextColor(TFT_MAROON, TFT_DARKGREY);
  tft.fillRect(0, 20, WIDTH, HEIGHT-40, TFT_DARKGREY);
  SMN_printAt("X", WIDTH-16, 24); 
  SMN_printAt(msg, 5, HEIGHT/2-10); 
  if (timeout > 0) {
    delay(timeout);
    needsRefresh = true;
  }
}

void SMN_handler(Button2& btn) {
    //Serial.println(String(btn.getClickType()) + " click type");
    lastClick = millis();
    needsRefresh = true;
    if (needsExitFromAction) {
       needsExitFromAction = false;
       return;
    } 
    switch (btn.getClickType()) {
        case SINGLE_CLICK:
            if (btn.getAttachPin() == BUTTON_UP) {
              int c = active_menu->getSelectedChild();
              if (c > 0) active_menu->setSelectedChild(--c);
            } else {
              int c = active_menu->getSelectedChild();
              if (c < _SM_MAXCHILD) active_menu->setSelectedChild(++c);
           }
            break;
        case LONG_CLICK:
            if (btn.getAttachPin() == BUTTON_UP) {
              if (active_menu->parent != NULL) active_menu = active_menu->parent;
            } else {
              int c = active_menu->getSelectedChild();
              SimpleMenu* clicked_menu = active_menu->child[c];
              if (clicked_menu->getChildNum() > 0) {
                active_menu = clicked_menu;
              } else {
                if (clicked_menu->actionSelect != NULL) {
                  needsRefresh = false;
                  needsExitFromAction = true;
                  tft.fillScreen(TFT_BLACK);
                  tft.drawRect(0, 0, WIDTH-1, HEIGHT-1, TFT_WHITE);
                  tft.setFreeFont(FMBO18);
                  tft.setTextColor(TFT_RED, TFT_BLACK);
                  menu_name_length = 0;

                  if (clicked_menu->displayMenuName == true) { //allows to not display menu name if it ends up being hidden
                    for (int i = 0; clicked_menu->name[i] != '\0'; i++) {
                      menu_name_length++;
                    }
                    if (menu_name_length == 6) {
                      posH = 1;
                    }else if (menu_name_length == 4) {
                      posH = 20;
                    }else {
                      posH = 5;
                    }
                  SMN_printAt(String(clicked_menu->name), posH, 48); 
                  
                  }
                  if (clicked_menu->actionSelect != NULL) clicked_menu->actionSelect();
                  if (clicked_menu->alertDone) SMN_alert(String(clicked_menu->name)+" done!",500,0);
                  lastClick = millis();
                   
                 }
              }
           }
            break;
        case DOUBLE_CLICK:
            if (btn.getAttachPin() == BUTTON_DOWN) {
              pcurrent ++;
              if (pcurrent >= MAXSIGS) pcurrent = 0;
            }
            else {
              pcurrent --;
              if (pcurrent < 0) pcurrent = MAXSIGS -1;
            }
            break;
        default:
            Serial.println(String(btn.getClickType()) + " click type / Not implemented ");
            break;
    }
}

bool SMN_isAnyButtonPressed() {
  return (butUp.isPressedRaw() || butDown.isPressedRaw());
}
bool SMN_isUpButtonPressed() {
  return butUp.isPressedRaw();
}
bool SMN_isDownButtonPressed() {
  return butDown.isPressedRaw();
}


void SMN_initMenu(SimpleMenu *menu) {
  butUp.setLongClickTime(300);
  butUp.setDoubleClickTime(300);
  butUp.setClickHandler(SMN_handler);
  butUp.setLongClickHandler(SMN_handler);
  butUp.setDoubleClickHandler(SMN_handler);
 
  butDown.setLongClickTime(300);
  butDown.setDoubleClickTime(300);  
  butDown.setClickHandler(SMN_handler);
  butDown.setLongClickHandler(SMN_handler);
  butDown.setDoubleClickHandler(SMN_handler);
 
  active_menu = menu;

  tft.init();
  tft.setRotation(2);
  tft.fillScreen(TFT_BLACK);
  tft.setTextColor(TFT_RED, TFT_BLACK);
  tft.setFreeFont(FMB9);
  delay(500);
 
}

void SMN_printMenu() {
  tft.fillScreen(TFT_BLACK);

  tft.setFreeFont(FMB12);         
  tft.setTextColor(TFT_YELLOW, TFT_BLACK);
  SMN_printAt(active_menu->name, 5, 2);

  tft.setTextColor(TFT_RED, TFT_BLACK);
  String p = "[";
  p+=pcurrent;
  p+="]";
  SMN_printAt(p, WIDTH-44, 3);
 
  tft.setFreeFont(FM9);         
  tft.setTextColor(TFT_GREEN, TFT_BLACK);
 
  for (int i=0; i < active_menu->getChildNum(); i++) {
    String entry = "";
    if (active_menu->getSelectedChild() == i) entry += "(*)";  
    else entry += "( )";
    entry += active_menu->child[i]->name;  
    SMN_printAt(entry,2,33+i*24); 
  }
  
  tft.drawLine(0, 29, WIDTH-2, 29, TFT_GREEN);
  tft.drawRect(0, 0, WIDTH-1, HEIGHT-1, TFT_YELLOW);
  needsRefresh = false;
}


long SMN_idleMS() {
  return millis() - lastClick;
}

void SMN_loop() {
  butUp.loop();
  yield();
  butDown.loop();
  yield();
  if (needsRefresh) {
    //Serial.println("Refreshing screen");
    SMN_printMenu();
  }
  yield();
}