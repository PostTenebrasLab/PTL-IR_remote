/*
 * =====================================================================================
 *
 *       Filename:  test_PTL_IR_remote_shield.ino
 *
 *    Description:  PTL_ir_remote running on Wemos (esp8266 D1)
 *                  esp8266 IR remote control with web server and MQTT (reder/writer)
 *
 *        Version:  1.0
 *        Created:  04/07/2018 11:12:31
 *       Revision:  none
 *       Compiler:  gcc-avr
 *
 *         Author:  Sebastien Chassot (sinux), seba.ptl@sinux.net
 *        Company:  Post Tenebras Lab (Geneva's Hackerspace)
 *
 * =====================================================================================
 */

#include "test_PTL_IR_remote_shield.h";

#define TEST_WIFI
#ifdef TEST_WIFI
#include <WiFiManager.h>

WiFiManager wifiManager;
#endif


/****** DHT22 thermo+hum *********/
#include "DHTesp.h"

#define TEST_DHT
#define DHT_PIN 5

DHTesp dht;
long timer_dht = 0;

/****** photoresistor *********/
#define TEST_PHOTORES
#define ANALOG_PIN  A0
long timer_photores = 0;


/****** IR *********/
#include <IRremoteESP8266.h>
#include <IRrecv.h>
#include <IRsend.h>
#include <IRutils.h>
#include <IRremoteESP8266.h>
#include <IRrecv.h>
#include <IRsend.h>
#include <IRutils.h>
#include <ir_Daikin.h>
#include <ir_Fujitsu.h>
#include <ir_Gree.h>
#include <ir_Haier.h>
#include <ir_Kelvinator.h>
#include <ir_Midea.h>
#include <ir_Toshiba.h>

#define TEST_IR
#define IR_LED 0
#define RECV_PIN 14

IRsend irsend = IRsend(IR_LED);
IRrecv irrecv(RECV_PIN, 1024, 15U, true);

decode_results results;  // Somewhere to store the results
void dumpACInfo(decode_results *results);
long timer_ir = 0;


/****** led WS2813b *********/
#include <WS2812FX.h>

#define TEST_LED
#define LED_PIN 2
#define LED_COUNT 20

#define DEFAULT_COLOR 0xFF5900
#define DEFAULT_BRIGHTNESS 255
#define DEFAULT_SPEED 1000
#define DEFAULT_MODE FX_MODE_STATIC


WS2812FX ws2812fx = WS2812FX(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);
long timer_led = 0;

/****** BME280 shield *********/
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
#define TEST_BME280
#define LED_PIN 2


void setup() {

  Serial.begin(115200);
  Serial.println();
  Serial.println();
  Serial.println("Starting...");


#ifdef TEST_WIFI
  setup_wifi();
#endif


/****** DHT22 thermo+hum *********/
  Serial.println("Status\tHumidity (%)\tTemperature (C)\t(F)\tHeatIndex (C)\t(F)");
  dht.setup(DHT_PIN, DHTesp::DHT22); // Connect DHT sensor to GPIO 17

#ifdef TEST_IR
  irrecv.setUnknownThreshold(12);
  irrecv.enableIRIn();  // Start the receiver
#endif

/****** led WS2813b *********/
  Serial.println("WS2812FX setup");
  ws2812fx.init();
  ws2812fx.setMode(DEFAULT_MODE);
  ws2812fx.setColor(DEFAULT_COLOR);
  ws2812fx.setSpeed(DEFAULT_SPEED);
  ws2812fx.setBrightness(DEFAULT_BRIGHTNESS);
  ws2812fx.start();
}

#ifdef TEST_WIFI
/**
 * 
 */
void setup_wifi() {
    delay(10);
    
    // We start by connecting to a WiFi network
    wifiManager.setTimeout(300);  // Time out after 5 mins.
    if (!wifiManager.autoConnect(WIFI_AP_NAME)) {
        Serial.print("Wifi failed to connect and hit timeout.");
        // Reboot. A.k.a. "Have you tried turning it Off and On again?"
        ESP.reset();
    }

    Serial.print("WiFi connected. IP address:\t");
    Serial.println(WiFi.localIP().toString());
    Serial.print("WiFi connected. MAC address:\t");
    Serial.println(WiFi.macAddress());
}
#endif

#ifdef TEST_PHOTORES
/** Get luminosity from photo-resistor
 *
 * @return a mapped value (0..100%)
 */
unsigned int get_luminosity() {

    pinMode(ANALOG_PIN, INPUT);
    unsigned int val = analogRead(ANALOG_PIN);

    Serial.print("    Luminosity = ");
    Serial.print(map(val, 0, 1023, 0, 100));
    Serial.println();

    return map(val, 0, 1023, 0, 100);
}
#endif

#ifdef TEST_IR
void readIr(){
   // Check if the IR code has been received.
  if (irrecv.decode(&results)) {
    // Display a crude timestamp.
    uint32_t now = millis();
    Serial.printf("Timestamp : %06u.%03u\n", now / 1000, now % 1000);
    if (results.overflow)
      Serial.printf("WARNING: IR code is too big for buffer (>= %d). "
                    "This result shouldn't be trusted until this is resolved. "
                    "Edit & increase CAPTURE_BUFFER_SIZE.\n",
                    1024);
    // Display the basic output of what we found.
    Serial.print(resultToHumanReadableBasic(&results));
    dumpACInfo(&results);  // Display any extra A/C info if we have it.
    yield();  // Feed the WDT as the text output can take a while to print.

    // Display the library version the message was captured with.
    Serial.print("Library   : v");
    Serial.println(_IRREMOTEESP8266_VERSION_);
    Serial.println();

//    // Output RAW timing info of the result.
//    Serial.println(resultToTimingInfo(&results));
//    yield();  // Feed the WDT (again)
//
//    // Output the results as source code
//    Serial.println(resultToSourceCode(&results));
//    Serial.println("");  // Blank line between entries
//    yield();  // Feed the WDT (again)

    const char * msg = String(resultToSourceCode(&results)).c_str();
  }
}

void dumpACInfo(decode_results *results) {
  String description = "";
#if DECODE_DAIKIN
  if (results->decode_type == DAIKIN) {
    IRDaikinESP ac(0);
    ac.setRaw(results->state);
    description = ac.toString();
  }
#endif  // DECODE_DAIKIN
#if DECODE_FUJITSU_AC
  if (results->decode_type == FUJITSU_AC) {
    IRFujitsuAC ac(0);
    ac.setRaw(results->state, results->bits / 8);
    description = ac.toString();
  }
#endif  // DECODE_FUJITSU_AC
#if DECODE_KELVINATOR
  if (results->decode_type == KELVINATOR) {
    IRKelvinatorAC ac(0);
    ac.setRaw(results->state);
    description = ac.toString();
  }
#endif  // DECODE_KELVINATOR
#if DECODE_TOSHIBA_AC
  if (results->decode_type == TOSHIBA_AC) {
    IRToshibaAC ac(0);
    ac.setRaw(results->state);
    description = ac.toString();
  }
#endif  // DECODE_TOSHIBA_AC
#if DECODE_GREE
  if (results->decode_type == GREE) {
    IRGreeAC ac(0);
    ac.setRaw(results->state);
    description = ac.toString();
  }
#endif  // DECODE_GREE
#if DECODE_MIDEA
  if (results->decode_type == MIDEA) {
    IRMideaAC ac(0);
    ac.setRaw(results->value);  // Midea uses value instead of state.
    description = ac.toString();
  }
#endif  // DECODE_MIDEA
#if DECODE_HAIER_AC
  if (results->decode_type == HAIER_AC) {
    IRHaierAC ac(0);
    ac.setRaw(results->state);
    description = ac.toString();
  }
#endif  // DECODE_HAIER_AC
  // If we got a human-readable description of the message, display it.
  if (description != "")  Serial.println("Mesg Desc.: " + description);
}
#endif

void loop() {

#ifdef TEST_DHT
  if(millis() > timer_dht) {

    float humidity = dht.getHumidity();
    float temperature = dht.getTemperature();
  
    Serial.print(dht.getStatusString());
    Serial.print("\t");
    Serial.print(humidity, 1);
    Serial.print("\t\t");
    Serial.print(temperature, 1);
    Serial.print("\t\t");
    Serial.print(dht.toFahrenheit(temperature), 1);
    Serial.print("\t\t");
    Serial.print(dht.computeHeatIndex(temperature, humidity, false), 1);
    Serial.print("\t\t");
    Serial.println(dht.computeHeatIndex(dht.toFahrenheit(temperature), humidity, true), 1);    
    
    timer_dht = millis() + 3000;
  }
#endif

#ifdef TEST_PHOTORES
  if(millis() > timer_photores) {
    get_luminosity();
    timer_photores = millis() + 3000;
  }
#endif


#ifdef TEST_IR
  readIr(); 
  
  /***** send *****/
  if(millis() > timer_ir) {
    Serial.println("Sending IR code...");
    irsend.sendNEC(0x00FFE01FUL, 32);
    timer_ir = millis() + 3000;
  }

#endif


#ifdef TEST_LED
  ws2812fx.service();

  if(millis() > timer_led) {
    uint8_t next_mode = (ws2812fx.getMode() + 1) % ws2812fx.getModeCount();
    ws2812fx.setMode(next_mode);
    Serial.print("mode is "); Serial.println(ws2812fx.getModeName(ws2812fx.getMode()));
    timer_led = millis() + 3000;
  }
#endif


}
