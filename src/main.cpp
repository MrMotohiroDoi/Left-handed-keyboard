#include <Arduino.h>
#include <Adafruit_TinyUSB.h>

const int numRows=5;
const int numCols=6;
int rowPins[numRows]={4,3,2,1,0};
int colPins[numCols]={D5,D6,D9,D8,D7,D10};

uint8_t const desc_hid_report[]={
    TUD_HID_REPORT_DESC_KEYBOARD()
};
Adafruit_USBD_HID usb_hid(desc_hid_report, sizeof(desc_hid_report), HID_ITF_PROTOCOL_KEYBOARD, 2, false);
uint8_t hidcode[] = {HID_KEY_TAB, HID_KEY_1, HID_KEY_2, HID_KEY_F1, HID_KEY_F2, HID_KEY_P, HID_KEY_ENTER, HID_KEY_Q, HID_KEY_W, HID_KEY_E, HID_KEY_R, HID_KEY_G, HID_KEY_SHIFT_LEFT, HID_KEY_A, HID_KEY_S, HID_KEY_D, HID_KEY_F, HID_KEY_B, HID_KEY_CONTROL_LEFT, HID_KEY_Z, HID_KEY_X, HID_KEY_C, HID_KEY_V, HID_KEY_ARROW_RIGHT, HID_KEY_GUI_LEFT, HID_KEY_ALT_LEFT, HID_KEY_ARROW_LEFT, HID_KEY_SPACE, HID_KEY_DELETE, HID_KEY_ESCAPE};

void setup() {

    usb_hid.begin();
    for(int row=0; row<numRows; row++){
        pinMode(rowPins[row], INPUT_PULLDOWN);
    }
    for(int col=0; col<numCols; col++){
        pinMode(colPins[col], OUTPUT);
        digitalWrite(colPins[col],LOW);
    }
    while(!TinyUSBDevice.mounted()) delay(1);
}

void loop() {
    static bool keyPressedPreviously=false;
    u_int8_t count=0;
    u_int8_t keycode[6]={ 0 };
    for(int col=0; col<numCols; col++){
        digitalWrite(colPins[col],HIGH);
        for(int row=0; row<numRows; row++){
            if(digitalRead(rowPins[row])==HIGH){
                keycode[count++]=hidcode[col+row*6];
                if(count==6){

                    break;
                }
                
            }
        }
        digitalWrite(colPins[col],LOW);
    }
    
    if ( TinyUSBDevice.suspended() && count ){
    // Wake up host if we are in suspend mode
    // and REMOTE_WAKEUP feature is enabled by host
    TinyUSBDevice.remoteWakeup();
    }
    // skip if hid is not ready e.g still transferring previous report
    if ( !usb_hid.ready() ) return;
    if ( count ){
    // Send report if there is key pressed
    uint8_t const report_id = 0;
    uint8_t const modifier = 0;

    keyPressedPreviously = true;
    usb_hid.keyboardReport(report_id, modifier, keycode);
    }
    else{
    // Send All-zero report to indicate there is no keys pressed
    // Most of the time, it is, though we don't need to send zero report
    // every loop(), only a key is pressed in previous loop()
    if ( keyPressedPreviously )
    {
      keyPressedPreviously = false;
      usb_hid.keyboardRelease(0);
    }
    }

}