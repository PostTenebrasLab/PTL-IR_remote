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

#include "test_PTL_IR_remote_shield.h"

#define TEST_WIFI
#ifdef TEST_WIFI
#include <DNSServer.h>
#include <WiFiManager.h>
#include <ESP8266mDNS.h>

WiFiManager wifiManager;
#endif


/****** MQTT Debug *********/
#define MQTT
#ifdef MQTT

#include <PubSubClient.h>  // v2.6.0
WiFiClient espClient;

// Callback function, when the gateway receive an MQTT value on the topics
// subscribed this function is called
void callback(char* topic, byte* payload, unsigned int length) {
  // In order to republish this payload, a copy must be made
  // as the orignal payload buffer will be overwritten whilst
  // constructing the PUBLISH packet.
  // Allocate the correct amount of memory for the payload copy
  byte* payload_copy = reinterpret_cast<byte*>(malloc(length + 1));
  // Copy the payload to the new buffer
  memcpy(payload_copy, payload, length);

  // Conversion to a printable string
  payload_copy[length] = '\0';
  String callback_string = String(reinterpret_cast<char*>(payload_copy));
  String topic_name = String(reinterpret_cast<char*>(topic));

  // launch the function to treat received data
  //receivingMQTT(topic_name, callback_string);

  // Free the memory
  free(payload_copy);
}
PubSubClient mqtt_client(MQTT_SERVER, MQTT_PORT, callback, espClient);

String mqtt_clientid = ESPID;
uint32_t lastReconnectAttempt = 0;  // MQTT last attempt reconnection number
bool boot = true;
const char* mqtt_user = "";
const char* mqtt_password = "";
#endif   // 


/****** OTA firmware update *********/
#define OTA
#ifdef OTA
#include <ESP8266httpUpdate.h>

bool otaUpdate = true;
#endif

#define TEST_PERIOD 5000

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
    if (!MDNS.begin(HOSTNAME)) {
        Serial.println("Error setting up MDNS responder!");
    }
    Serial.print("WiFi connected. IP address:\t");
    Serial.println(WiFi.localIP().toString());
    Serial.print("WiFi connected. MAC address:\t");
    Serial.println(WiFi.macAddress());
}
#endif  //TEST_WIFI


#ifdef MQTT

bool reconnect() {
  // Loop a few times or until we're reconnected
  uint16_t tries = 1;
  while (!mqtt_client.connected() && tries <= 3) {
    int connected = false;
    // Attempt to connect
    Serial.println("Attempting MQTT connection to " MQTT_SERVER ":" + String(MQTT_PORT) +"... ");
    if (mqtt_user && mqtt_password)
      connected = mqtt_client.connect(mqtt_clientid.c_str(), mqtt_user,
                                      mqtt_password);
    else
      connected = mqtt_client.connect(mqtt_clientid.c_str());
    if (connected) {
    // Once connected, publish an announcement...
      mqtt_client.publish(MQTTnamespace, "Connected");
      Serial.println("connected.");
      // Subscribing to topic(s)
//      subscribing(MQTTcommand);
    } else {
      Serial.println("failed, rc=" + String(mqtt_client.state()) +" Try again in a bit.");
      // Wait for a bit before retrying
      delay(tries << 7);  // Linear increasing back-off (x128)
    }
    tries++;
  }
  return mqtt_client.connected();
}


void mqttClient(){
  // MQTT client connection management
  if (!mqtt_client.connected()) {
    uint32_t now = millis();
    // Reconnect if it's longer than MQTT_RECONNECT_TIME since we last tried.
    if (now - lastReconnectAttempt > MQTT_RECONNECT_TIME) {
      lastReconnectAttempt = now;
      Serial.println("client mqtt not connected, trying to connect");
      // Attempt to reconnect
      if (reconnect()) {
        lastReconnectAttempt = 0;
        if (boot) {
          mqtt_client.publish(MQTTnamespace, "IR Server just booted");
          boot = false;
        } else {
          mqtt_client.publish(MQTTnamespace, "IR Server just (re)connected to MQTT");
        }
      }
    }
  } else {
    // MQTT loop
    mqtt_client.loop();
  }
}

#endif //MQTT


#ifdef OTA
/**
 * 
 */
bool updateOTA(){

    t_httpUpdate_return  ret = ESPhttpUpdate.update(FW_UPDATE_URL);

//    Serial.println(ESPhttpUpdate.getLastError());
    switch(ESPhttpUpdate.getLastError()) {
        case 11:
            Serial.println("OTA : Updating firmware...");
            break;
        case -11:
            Serial.println("OTA : esp unknown in PTL esp DB...");
            break;
        case -1:
            Serial.println("OTA : No server found");
            break;
        case -102:
            Serial.println("OTA : No update needed current firmware is up to date.");
            break;
        default:
           Serial.println("OTA : Unknown error");
    }
    return (ret == 11) ? true : false;    
}
#endif   // OTA


#ifdef TEST_PHOTORES
/** Get luminosity from photo-resistor
 *
 * @return a mapped value (0..100%)
 */
unsigned int get_luminosity() {

    pinMode(ANALOG_PIN, INPUT);
    unsigned int val = analogRead(ANALOG_PIN);

    Serial.print("Luminosity   test OK (2/5) : ");
    Serial.print(map(val, 0, 1023, 0, 100));
    Serial.println("/100 (no calibration)");

    return map(val, 0, 1023, 0, 100);
}
#endif

#ifdef TEST_IR
void readIr(){
  // Check if the IR code has been received.
  if (irrecv.decode(&results)) {
    // Display a crude timestamp.
    uint32_t now = millis();
    Serial.printf("receive IR   test OK (5/5) : %06u.%03u\n", now / 1000, now % 1000);
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

    const char * msg = String(resultToSourceCode(&results)).c_str();
#ifdef MQTT
    mqtt_client.publish(MQTTnamespace, resultToHumanReadableBasic(&results).c_str());
#endif
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

#ifdef MQTT
    mqttClient();
#endif

#ifdef OTA
  if ( otaUpdate ) {
      updateOTA();
      otaUpdate = false;
  }
#endif // OTA

#ifdef TEST_DHT
  if(millis() > timer_dht) {

    float humidity = dht.getHumidity();
    float temperature = dht.getTemperature();

    Serial.print("DHT22 thermo test ");  
    Serial.print(dht.getStatusString());
    Serial.print(" (1/5) : temp : ");
    Serial.print(temperature, 1);
    Serial.print(",  humidity : ");
    Serial.print(humidity, 1);
    Serial.print(",  adjusted heat : ");    
    Serial.println(dht.computeHeatIndex(temperature, humidity, false), 1);
    Serial.println("temp to mqtt");
#ifdef MQTT
    String dht_payload = "temp : "+String(temperature)+", hum : "+String(humidity);
    mqtt_client.publish(MQTTnamespace, dht_payload.c_str());
#endif
    timer_dht = millis() + TEST_PERIOD;
  }
#endif  // TEST_DHT

#ifdef TEST_PHOTORES

  if(millis() > timer_photores) {
    get_luminosity();
    timer_photores = millis() + TEST_PERIOD;

#ifdef MQTT
    String lum = "luminosity : "+String(get_luminosity());
    mqtt_client.publish(MQTTnamespace, lum.c_str());
#endif
  }
#endif // TEST_PHOTORES


#ifdef TEST_IR
  readIr(); 
  
  /***** send *****/
  if(millis() > timer_ir) {
    Serial.println("Send IR code test OK (3/5) : 0x00FFE01FUL");
    irsend.sendNEC(0x00FFE01FUL, 32);
    timer_ir = millis() + TEST_PERIOD;
#ifdef MQTT
    mqtt_client.publish(MQTTnamespace, "IR sent 0x00FFE01FUL");
#endif
  }
#endif  //  TEST_IR


#ifdef TEST_LED
  ws2812fx.service();

  if(millis() > timer_led) {
    uint8_t next_mode = (ws2812fx.getMode() + 1) % ws2812fx.getModeCount();
    ws2812fx.setMode(next_mode);
    Serial.print("LED ws2813b  test OK (4/5) : mode is "); Serial.println(ws2812fx.getModeName(ws2812fx.getMode()));
    timer_led = millis() + TEST_PERIOD;
  }
#endif  // TEST_LED


}
