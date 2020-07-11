#include <Arduino.h>

// ******************************* NODE CONFIGURATION **********************************

// Sketch name and version
const char sketch_name[] = "USB-exp";
const char sketch_version[] = "1.0";

//  Pin configuration
#include <PinConfig.cpp>

// MySensors configuration
#include <MySconfig.cpp>

// Power LED
#ifdef PWR_LED_PIN
#define ENABLE_POWER_LED
#endif

// Button parameters
#define DEBOUNCE_INTERVAL 20      // Debounce time in milliseconds
uint16_t ClickMinTime = 25;       //Min press time that defines a short click
uint16_t ClickMaxTime = 500;      //Max press time that defines a short click
uint16_t LongPressMinTime = 3000; //Min press time that defines a long press

// DEBUG mode
//#define MY_DEBUG
//#define F_DEBUG

// ************************ END OF CONFIG **********************************

// **************************** INIT ***************************************

// Libraries
#include <MySensors.h>
#include <Bounce2.h>

// Object initialization
#ifdef SW1_PIN
Bounce sw1_debouncer = Bounce();
#endif
#ifdef SW2_PIN
Bounce sw2_debouncer = Bounce();
#endif
#ifdef SW3_PIN
Bounce sw3_debouncer = Bounce();
#endif

// CHILD_ID
#ifdef USB_PORT_1_PIN
#define CHILD_ID_USB_PORT_1 1
#endif
#ifdef USB_PORT_2_PIN
#define CHILD_ID_USB_PORT_2 2
#endif
#ifdef USB_PORT_3_PIN
#define CHILD_ID_USB_PORT_3 3
#endif

// Messages
#ifdef CHILD_ID_USB_PORT_1
MyMessage msgUSBPort1(CHILD_ID_USB_PORT_1, V_STATUS);
#endif
#ifdef CHILD_ID_USB_PORT_2
MyMessage msgUSBPort2(CHILD_ID_USB_PORT_2, V_STATUS);
#endif
#ifdef CHILD_ID_USB_PORT_3
MyMessage msgUSBPort3(CHILD_ID_USB_PORT_3, V_STATUS);
#endif

// First run
bool first_run = true;

// USB port Global var
#ifdef CHILD_ID_USB_PORT_1
bool usb_port_1_status = false;
bool usb_port_1_set_status = false;
#endif
#ifdef CHILD_ID_USB_PORT_2
bool usb_port_2_status = false;
bool usb_port_2_set_status = false;
#endif
#ifdef CHILD_ID_USB_PORT_3
bool usb_port_3_status = false;
bool usb_port_3_set_status = false;
#endif

// Switch global var
#ifdef SW1_PIN
unsigned long sw1_press_millis;
#endif
#ifdef SW2_PIN
unsigned long sw2_press_millis;
#endif
#ifdef SW3_PIN
unsigned long sw3_press_millis;
#endif

// Time counter
unsigned long current_time_millis;

// ********************************* END OF INIT **********************************

// ****************************** CUSTOM FUNCTIONS ********************************

void on_button_release(bool &var_to_change, unsigned long button_press_millis)
{
  // On Short press (25 -350 ms) flip USB port
  if (current_time_millis - button_press_millis >= ClickMinTime && current_time_millis - button_press_millis <= ClickMaxTime)
  {
#ifdef F_DEBUG
    Serial.println("Button click type: short");
#endif
    var_to_change = !var_to_change;
  }
  // On long press time time force USB port to OFF
  else if (current_time_millis - button_press_millis >= LongPressMinTime)
  {
#ifdef DEBUG
    Serial.println("Button click type: long");
#endif
    var_to_change = false;
  }
}
// ************************** END OF CUSTOM FUNCTIONS *****************************/

// ******************** MYSESNSORS PRESENTANTION FUNCTION ***************************/

void presentation()
{
  // Send the sketch version information to the gateway and Controller
  sendSketchInfo(sketch_name, sketch_version, ack);
  // Register all sensors to gw (they will be created as child devices)
#ifdef CHILD_ID_USB_PORT_1
  present(CHILD_ID_USB_PORT_1, S_BINARY, "USP port 1", ack);
#endif
#ifdef CHILD_ID_USB_PORT_2
  present(CHILD_ID_USB_PORT_2, S_BINARY, "USP port 2", ack);
#endif
#ifdef CHILD_ID_USB_PORT_3
  present(CHILD_ID_USB_PORT_3, S_BINARY, "USP port 3", ack);
#endif
}

// ******************** END OF MYSESNSORS PRESENTANTION FUNCTION ************************/

// ********************************** ARDUINO SETUP *************************************/
void setup()
{
#ifdef PWR_LED_PIN
  pinMode(PWR_LED_PIN, OUTPUT);
  digitalWrite(PWR_LED_PIN, LOW);
#endif
#ifdef DISABLE_ERR_LED
  pinMode(DISABLE_ERR_LED, INPUT_PULLUP);
#endif
#ifdef DISABLE_TX_LED
  pinMode(DISABLE_TX_LED, INPUT_PULLUP);
#endif
#ifdef DISABLE_RX_LED
  pinMode(DISABLE_RX_LED, INPUT_PULLUP);
#endif
#ifdef SW1_PIN
  pinMode(SW1_PIN, INPUT_PULLUP);
  sw1_debouncer.attach(SW1_PIN, INPUT_PULLUP);
  sw1_debouncer.interval(DEBOUNCE_INTERVAL);
#endif
#ifdef SW2_PIN
  pinMode(SW2_PIN, INPUT_PULLUP);
  sw2_debouncer.attach(SW2_PIN, INPUT_PULLUP);
  sw2_debouncer.interval(DEBOUNCE_INTERVAL);
#endif
#ifdef SW3_PIN
  pinMode(SW3_PIN, INPUT_PULLUP);
  sw3_debouncer.attach(SW3_PIN, INPUT_PULLUP);
  sw3_debouncer.interval(DEBOUNCE_INTERVAL);
#endif
#ifdef USB_PORT_1_PIN
  pinMode(USB_PORT_1_PIN, OUTPUT);
  digitalWrite(USB_PORT_1_PIN, LOW);
#endif
#ifdef USB_PORT_2_PIN
  pinMode(USB_PORT_2_PIN, OUTPUT);
  digitalWrite(USB_PORT_2_PIN, LOW);
#endif
#ifdef USB_PORT_3_PIN
  pinMode(USB_PORT_3_PIN, OUTPUT);
  digitalWrite(USB_PORT_3_PIN, LOW);
#endif
}

// ***************************** END OF ARDUINO SETUP ******************************

// ******************************* ARDUINO LOOP ************************************

void loop()
{
  // Timer Update
  current_time_millis = millis();

// Turn on the power LED when the node is activated for the first time
#ifdef ENABLE_POWER_LED
  if (first_run)
  {
    digitalWrite(PWR_LED_PIN, HIGH);
  }
#endif

  // Send the USB ports status to the gateway and controller when the node is activated for the first time
  if (first_run)
  {
#ifdef CHILD_ID_USB_PORT_1
    send(msgUSBPort1.set(usb_port_1_status ? 1 : 0), ack);
#endif
#ifdef CHILD_ID_USB_PORT_2
    send(msgUSBPort2.set(usb_port_2_status ? 1 : 0), ack);
#endif
#ifdef CHILD_ID_USB_PORT_3
    send(msgUSBPort3.set(usb_port_3_status ? 1 : 0), ack);
#endif
  }

// Check for button press and release
#ifdef SW1_PIN
  if (sw1_debouncer.update())
  {
    if (sw1_debouncer.fell())
    {
      sw1_press_millis = current_time_millis;
#ifdef F_DEBUG
      Serial.println("Button 1 press detected, starting time counter");
#endif
    }
    else if (sw1_debouncer.rose())
    {
#ifdef F_DEBUG
      Serial.println("Button 1 release detected!");
#endif
      on_button_release(usb_port_1_set_status, sw1_press_millis);
    }
  }
#endif

#ifdef SW2_PIN
  if (sw2_debouncer.update())
  {
    if (sw2_debouncer.fell())
    {
      sw2_press_millis = current_time_millis;
#ifdef F_DEBUG
      Serial.println("Button 2 press detected, starting time counter");
#endif
    }
    else if (sw2_debouncer.rose())
    {
#ifdef F_DEBUG
      Serial.println("Button 2 release detected!");
#endif
      on_button_release(usb_port_2_set_status, sw2_press_millis);
    }
  }
#endif

#ifdef SW3_PIN
  if (sw3_debouncer.update())
  {
    if (sw3_debouncer.fell())
    {
      sw3_press_millis = current_time_millis;
#ifdef F_DEBUG
      Serial.println("Button 3 press detected, starting time counter");
#endif
    }
    else if (sw3_debouncer.rose())
    {
#ifdef F_DEBUG
      Serial.println("Button 3 release detected!");
#endif
      on_button_release(usb_port_3_set_status, sw3_press_millis);
    }
  }
#endif

  // Update USB ports
#ifdef CHILD_ID_USB_PORT_1
  if (usb_port_1_status != usb_port_1_set_status)
  {
    usb_port_1_status = usb_port_1_set_status;
    digitalWrite(USB_PORT_1_PIN, usb_port_1_status ? HIGH : LOW);
    send(msgUSBPort1.set(usb_port_1_status ? 1 : 0), ack);
#ifdef F_DEBUG
    Serial.print("USB port 1 staus changed to ");
    Serial.println(usb_port_1_status);
#endif
  }
#endif
#ifdef CHILD_ID_USB_PORT_2
  if (usb_port_2_status != usb_port_2_set_status)
  {
    usb_port_2_status = usb_port_2_set_status;
    digitalWrite(USB_PORT_2_PIN, usb_port_2_status ? HIGH : LOW);
    send(msgUSBPort2.set(usb_port_2_status ? 1 : 0), ack);
#ifdef F_DEBUG
    Serial.print("USB port 2 staus changed to ");
    Serial.println(usb_port_2_status);
#endif
  }
#endif
#ifdef CHILD_ID_USB_PORT_3
  if (usb_port_3_status != usb_port_3_set_status)
  {
    usb_port_3_status = usb_port_3_set_status;
    digitalWrite(USB_PORT_3_PIN, usb_port_3_status ? HIGH : LOW);
    send(msgUSBPort3.set(usb_port_3_status ? 1 : 0), ack);
#ifdef F_DEBUG
    Serial.print("USB port 3 staus changed to ");
    Serial.println(usb_port_3_status);
#endif
  }
#endif

  // Disable first run
  first_run = false;
}
// ******************************* END OF LOOP ************************************

// ******************************* MYSENSORS RECEIVE ************************************

void receive(const MyMessage &message)
{
#ifdef F_DEBUG
  Serial.println("Message received");
#endif
  // We only expect one type of message from controller. But we better check anyway.
  if (message.isAck())
  {
#ifdef F_DEBUG
    Serial.println("Type: ack from gateway");
#endif
  }
  if (message.type == V_STATUS)
  {
#ifdef F_DEBUG
    Serial.print("Type: V_STATUS, target CHILD_ID: ");
    Serial.println(message.sensor);
#endif
#ifdef CHILD_ID_USB_PORT_1
    if (message.sensor == CHILD_ID_USB_PORT_1)
    {
#ifdef F_DEBUG
      Serial.print("Payload (bool): ");
      Serial.println(message.getBool());
#endif
      usb_port_1_set_status = message.getBool();
    }
#endif
#ifdef CHILD_ID_USB_PORT_2
    if (message.sensor == CHILD_ID_USB_PORT_2)
    {
#ifdef F_DEBUG
      Serial.print("Payload (bool): ");
      Serial.println(message.getBool());
#endif
      usb_port_2_set_status = message.getBool();
    }
#endif
#ifdef CHILD_ID_USB_PORT_3
    if (message.sensor == CHILD_ID_USB_PORT_3)
    {
#ifdef F_DEBUG
      Serial.print("Payload (bool): ");
      Serial.println(message.getBool());
#endif
      usb_port_3_set_status = message.getBool();
    }
#endif
  }
}
// ******************************* END OF RECEIVE ************************************