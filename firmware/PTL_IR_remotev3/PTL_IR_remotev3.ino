/*
  WS2812FX Webinterface.
  
  Harm Aldick - 2016
  www.aldick.org

  
  FEATURES
    * Webinterface with mode, color, speed and brightness selectors


  LICENSE

  The MIT License (MIT)

  Copyright (c) 2016  Harm Aldick 

  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files (the "Software"), to deal
  in the Software without restriction, including without limitation the rights
  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
  copies of the Software, and to permit persons to whom the Software is
  furnished to do so, subject to the following conditions:

  The above copyright notice and this permission notice shall be included in
  all copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
  THE SOFTWARE.

  
  CHANGELOG
  2016-11-26 initial version
  2018-01-06 added custom effects list option and auto-cycle feature
  
*/

#include <WiFiManager.h>
#include <DNSServer.h>
#include <ESP8266mDNS.h>

#include <ESP8266WebServer.h>
#include <ESP8266OTA.h>
#include <ESP8266httpUpdate.h>

#include <DHTesp.h>
#include <WS2812FX.h>

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

#include "PTL_IR_remotev3.h"

extern const char index_html[];
extern const char ws2813_html[];
extern const char ir_html[];

extern const char main_js[];
extern const char ir_js[];
extern const char ws2813_js[];

#define WIFI_TIMEOUT 30000              // checks WiFi every ...ms. Reset after this time, if WiFi cannot reconnect.
#define HTTP_PORT 80

#define CAPTURE_BUFFER_SIZE 1024
#define TIMEOUT 15U  // Suits most messages, while not swallowing many repeats.
#define MIN_UNKNOWN_SIZE 12
#define DEFAULT_COLOR 0x00FF00
#define DEFAULT_BRIGHTNESS 255  // 0..255
#define DEFAULT_SPEED 1000
#define DEFAULT_MODE FX_MODE_BREATH

unsigned long auto_last_change = 0;
unsigned long last_wifi_check_time = 0;
String modes = "";
uint8_t myModes[] = {}; // *** optionally create a custom list of effect/mode numbers
boolean auto_cycle = false;
int8_t brightness = DEFAULT_BRIGHTNESS;
uint8_t lastMode;

/*****  WIFI  *****/
WiFiManager wifiManager;
ESP8266WebServer server(HTTP_PORT);
ESP8266OTA otaUpdater;

/*****  IR  *****/
IRsend irsend = IRsend(IR_LED_PIN);
IRrecv irrecv(IR_RECV_PIN, 1024, 15u, true);
decode_results results;  // Somewhere to store the results
bool ir_lock = false;  // Primitive locking for gating the IR LED.
uint16_t * newCodeArray(const uint16_t size);
uint16_t countValuesInStr(const String str, char sep);
uint32_t sendReqCounter = 0;
// HTML arguments we will parse for IR code information.
#define argType "type"
#define argData "code"
#define argBits "bits"
#define argRepeat "repeats"

/*****  DHT and LED  *****/
DHTesp dht;
WS2812FX ws2812fx = WS2812FX(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);
long timer_serial = 0;


// QUICKFIX...See https://github.com/esp8266/Arduino/issues/263
#define min(a,b) ((a)<(b)?(a):(b))
#define max(a,b) ((a)>(b)?(a):(b))

/**
 * 
 */
void wifi_setup() {
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
           Serial.println("Unknown error");
    }
    return (ret == 11) ? true : false;    
}

void calibrateIr(int8_t* offset){

  irsend.begin();

  *offset = irsend.calibrate();

}


void irInit(){
  irrecv.setUnknownThreshold(MIN_UNKNOWN_SIZE);
  irrecv.enableIRIn();  // Start the receiver
}


void readIr(){
   // Check if the IR code has been received.
  if (irrecv.decode(&results)) {
    
    translateIR();

    if (results.overflow)
      Serial.printf("WARNING: IR code is too big for buffer (>= %d). "
                    "This result shouldn't be trusted until this is resolved. "
                    "Edit & increase CAPTURE_BUFFER_SIZE.\n",
                    CAPTURE_BUFFER_SIZE);
    yield();  // Feed the WDT (again)

  }
}

void translateIR() // takes action based on IR code received

// describing Car MP3 IR codes 

{

  switch(results.value)

  {

  case 0xFF00FF:  
    Serial.println(" On             ");
    ws2812fx.setMode(lastMode);
    ws2812fx.setBrightness(brightness);
    break;

  case 0xFF807F:  
    Serial.println(" Off            ");
    lastMode = ws2812fx.getMode();
    ws2812fx.setBrightness(0);
    break;

  case 0xFF40BF:  
    Serial.println(" Flash          ");
    (ws2812fx.getMode() == 34) ? ws2812fx.setMode(0) : ws2812fx.setMode(34);
    break;

  case 0xFF20DF:  
    Serial.println(" Strobe         ");
    (ws2812fx.getMode() == 34) ? ws2812fx.setMode(0) : ws2812fx.setMode(34);
    break;

  case 0xFFA05F:  
    Serial.println(" Fade           ");
    (ws2812fx.getMode() == 15) ? ws2812fx.setMode(0) : ws2812fx.setMode(15);
    break;

  case 0xFF609F:  
    Serial.println(" Smooth         "); 
    (ws2812fx.getMode() == 15) ? ws2812fx.setMode(0) : ws2812fx.setMode(15);
    break;

  case 0xFF10EF:  
    Serial.println(" Light Up       ");
    brightness = (brightness >= 230) ? 255 : brightness + 25;
    ws2812fx.setBrightness(brightness);
    break;

  case 0xFF906F:  
    Serial.println(" Light Down     ");
    brightness = (brightness <= 25) ? 0 : brightness - 25;
    ws2812fx.setBrightness(brightness);
    break;

  case 0xFF50AF:  
    Serial.println(" W              ");
    ws2812fx.setColor(WHITE);
    break;

  case 0xFF30CF:  
    Serial.println(" Red            "); 
    ws2812fx.setColor(RED);
    break;

  case 0xFFB04F:  
    Serial.println(" Green          "); 
    ws2812fx.setColor(GREEN);
    break;

  case 0xFF708F:  
    Serial.println(" Blue           "); 
    ws2812fx.setColor(BLUE);
    break;

  case 0xFF08F7:  
    Serial.println(" c1             "); 
    ws2812fx.setColor(ORANGE);
    break;

  case 0xFF8877:  
    Serial.println(" c2             "); 
    ws2812fx.setColor(GREEN);
    break;

  case 0xFF48B7:  
    Serial.println(" c3             "); 
    ws2812fx.setColor(BLUE);
    break;

  case 0xFF28D7:  
    Serial.println(" c4             "); 
    ws2812fx.setColor(GREEN);
    break;

  case 0xFFA857:
    Serial.println(" c5             "); 
    ws2812fx.setColor(CYAN);
    break;

  case 0xFF6897:  
    Serial.println(" c6             "); 
    ws2812fx.setColor(PINK);
    break;

  case 0xFF18E7:  
    Serial.println(" c7             "); 
    ws2812fx.setColor(YELLOW);
    break;

  case 0xFF9867:  
    Serial.println(" c8             "); 
    ws2812fx.setColor(GREEN);
    break;

  case 0xFF58A7:  
    Serial.println(" c9             "); 
    ws2812fx.setColor(MAGENTA);
    break;

  case 0xFF52AD:  
    Serial.println(" 9              "); 
    ws2812fx.setColor(PURPLE);
    break;

  default: 
    Serial.println(" other button   ");

  }

  delay(500);


} //END translateIR




// needed by http server (display counter)
uint32_t getReqCounter(){
  return sendReqCounter;
}

// Arduino framework doesn't support strtoull(), so make our own one.
uint64_t getUInt64fromHex(char const *str) {
    uint64_t result = 0;
    uint16_t offset = 0;
    // Skip any leading '0x' or '0X' prefix.
    if (str[0] == '0' && (str[1] == 'x' || str[1] == 'X'))
       offset = 2;
    for (; isxdigit((unsigned char)str[offset]); offset++) {
        char c = str[offset];
        result *= 16;
        if (isdigit(c)) /* '0' .. '9' */
          result += c - '0';
        else if (isupper(c)) /* 'A' .. 'F' */
          result += c - 'A' + 10;
        else /* 'a' .. 'f'*/
          result += c - 'a' + 10;
    }
    return result;
}

// Parse an IRremote Raw Hex String/code and send it.
// Args:
//   str: A comma-separated String containing the freq and raw IR data.
//        e.g. "38000,9000,4500,600,1450,600,900,650,1500,..."
//        Requires at least two comma-separated values.
//        First value is the transmission frequency in Hz or kHz.
void parseStringAndSendRaw(const String str) {
  uint16_t count;
  uint16_t freq = 38000;  // Default to 38kHz.
  uint16_t *raw_array;

  // Find out how many items there are in the string.
  count = countValuesInStr(str, ',');

  // We expect the frequency as the first comma separated value, so we need at
  // least two values. If not, bail out.
  if (count < 2) return;
  count--;  // We don't count the frequency value as part of the raw array.

  // Now we know how many there are, allocate the memory to store them all.
  raw_array = newCodeArray(count);

  // Grab the first value from the string, as it is the frequency.
  int16_t index = str.indexOf(',', 0);
  freq = str.substring(0, index).toInt();
  uint16_t start_from = index + 1;
  // Rest of the string are values for the raw array.
  // Now convert the strings to integers and place them in raw_array.
  count = 0;
  do {
    index = str.indexOf(',', start_from);
    raw_array[count] = str.substring(start_from, index).toInt();
    start_from = index + 1;
    count++;
  } while (index != -1);

  irsend.sendRaw(raw_array, count, freq);  // All done. Send it.
  free(raw_array);  // Free up the memory allocated.
}


// Dynamically allocate an array of uint16_t's.
// Args:
//   size:  Nr. of uint16_t's need to be in the new array.
// Returns:
//   A Ptr to the new array. Restarts the ESP8266 if it fails.
uint16_t * newCodeArray(const uint16_t size) {
  uint16_t *result;

  result = reinterpret_cast<uint16_t*>(malloc(size * sizeof(uint16_t)));
  // Check we malloc'ed successfully.
  if (result == NULL) {  // malloc failed, so give up.
    Serial.printf("\nCan't allocate %d bytes. (%d bytes free)\n",
                  size * sizeof(uint16_t), ESP.getFreeHeap());
    Serial.println("Giving up & forcing a reboot.");
    ESP.restart();  // Reboot.
    delay(500);  // Wait for the restart to happen.
    return result;  // Should never get here, but just in case.
  }
  return result;
}


// Count how many values are in the String.
// Args:
//   str:  String containing the values.
//   sep:  Character that separates the values.
// Returns:
//   The number of values found in the String.
uint16_t countValuesInStr(const String str, char sep) {
  int16_t index = -1;
  uint16_t count = 1;
  do {
    index = str.indexOf(sep, index + 1);
    count++;
  } while (index != -1);
  return count;
}


// Display the human readable state of an A/C message if we can.
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

void setup(){
  Serial.begin(115200);
  Serial.println();
  Serial.println();
  Serial.println("Starting...");

  modes.reserve(5000);
  modes_setup();

  Serial.println("WS2812FX setup");
  ws2812fx.init();
  ws2812fx.setMode(DEFAULT_MODE);
  ws2812fx.setColor(DEFAULT_COLOR);
  ws2812fx.setSpeed(DEFAULT_SPEED);
  ws2812fx.setBrightness(DEFAULT_BRIGHTNESS);
  ws2812fx.start();

  Serial.println("Wifi setup");
  wifi_setup();

  Serial.println("web OTA updater");
  updateOTA();
  //set web UI
  otaUpdater.setUpdaterUi("Title","LemanMake PTL workshop OTA updater","Build : 1.3","Branch : master","Device info : LemanMake workshop","footer");
  otaUpdater.setup(&server);
  
  Serial.println("DHT22 setup");
  dht.setup(DHT_PIN, DHTesp::DHT22);
 
  Serial.println("HTTP server setup");
  server.on("/", srv_handle_index_html);
  server.on("/led", srv_handle_ws2813_html);
  server.on("/ir", srv_handle_ir_html);
  server.on("/ir_rest", srv_handle_ir_rest);
  server.on("/temp", srv_handle_temp);
  server.on("/hum", srv_handle_hum);
  server.on("/lum", srv_handle_lum);
  server.on("/main.js", srv_handle_main_js);
  server.on("/ir.js", srv_handle_ir_js);
  server.on("/ws2813.js", srv_handle_ws2813_js);
  server.on("/modes", srv_handle_modes);
  server.on("/set", srv_handle_set);
  server.onNotFound(srv_handle_not_found);
  server.begin();
  Serial.println("HTTP server started.");

  Serial.println("IR server setup");
  irInit();

  Serial.println("ready!");
}


void loop() {
  unsigned long now = millis();

  server.handleClient();
  ws2812fx.service();
  readIr();

  if(millis() > timer_serial) {

    Serial.print("Temp :");
    Serial.println(getTemperature());

    Serial.print("Hum :");
        Serial.println(getHumidity());

    Serial.print("Lum :");
    Serial.println(getLuminosity());

    timer_serial = millis() + 2000;
  }

  if(auto_cycle && (now - auto_last_change > 10000)) { // cycle effect mode every 10 seconds
    uint8_t next_mode = (ws2812fx.getMode() + 1) % ws2812fx.getModeCount();
    if(sizeof(myModes) > 0) { // if custom list of modes exists
      for(uint8_t i=0; i < sizeof(myModes); i++) {
        if(myModes[i] == ws2812fx.getMode()) {
          next_mode = ((i + 1) < sizeof(myModes)) ? myModes[i + 1] : myModes[0];
          break;
        }
      }
    }
    ws2812fx.setMode(next_mode);
    Serial.print("mode is "); Serial.println(ws2812fx.getModeName(ws2812fx.getMode()));
    auto_last_change = now;
  }
}


/*
 * Build <li> string for all modes.
 */
void modes_setup() {
  modes = "";
  uint8_t num_modes = sizeof(myModes) > 0 ? sizeof(myModes) : ws2812fx.getModeCount();
  for(uint8_t i=0; i < num_modes; i++) {
    uint8_t m = sizeof(myModes) > 0 ? myModes[i] : i;
    modes += "<li><a href='#' class='m' id='";
    modes += m;
    modes += "'>";
    modes += ws2812fx.getModeName(m);
    modes += "</a></li>";
  }
}

/* #####################################################
#  Webserver Functions
##################################################### */

void srv_handle_not_found() {
  server.send(404, "text/plain", "File Not Found");
}

void srv_handle_index_html() {
  server.send_P(200,"text/html", index_html);
}

void srv_handle_ws2813_html() {
  server.send_P(200,"text/html", ws2813_html);
}

void srv_handle_ir_html() {
  server.send_P(200,"text/html", ir_html);
}

// Parse the URL args to find the IR code.
void srv_handle_ir_rest() {
    uint64_t data = 0;
    String data_str = "";
  
    for (uint16_t i = 0; i < server.args(); i++) {
        if (server.argName(i) == argData) {
            data = getUInt64fromHex(server.arg(i).c_str());
            data_str = server.arg(i);
        }
    }
  
    Serial.println("New code received via HTTP");
    parseStringAndSendRaw(data_str.c_str());
    srv_handle_index_html();
}

void srv_handle_main_js() {
  server.send_P(200,"application/javascript", main_js);
}

void srv_handle_ws2813_js() {
  server.send_P(200,"application/javascript", ws2813_js);
}

void srv_handle_ir_js() {
  server.send_P(200,"application/javascript", ir_js);
}

void srv_handle_modes() {
  server.send(200,"text/plain", modes);
}

void srv_handle_set() {
  for (uint8_t i=0; i < server.args(); i++){
    if(server.argName(i) == "c") {
      uint32_t tmp = (uint32_t) strtol(&server.arg(i)[0], NULL, 16);
      if(tmp >= 0x000000 && tmp <= 0xFFFFFF) {
        ws2812fx.setColor(tmp);
      }
    }

    if(server.argName(i) == "m") {
      uint8_t tmp = (uint8_t) strtol(&server.arg(i)[0], NULL, 10);
      ws2812fx.setMode(tmp % ws2812fx.getModeCount());
      Serial.print("mode is "); Serial.println(ws2812fx.getModeName(ws2812fx.getMode()));
    }

    if(server.argName(i) == "b") {
      if(server.arg(i)[0] == '-') {
        ws2812fx.setBrightness(ws2812fx.getBrightness() * 0.8);
      } else if(server.arg(i)[0] == ' ') {
        ws2812fx.setBrightness(min(max(ws2812fx.getBrightness(), 5) * 1.2, 255));
      } else { // set brightness directly
        uint8_t tmp = (uint8_t) strtol(&server.arg(i)[0], NULL, 10);
        ws2812fx.setBrightness(tmp);
      }
      Serial.print("brightness is "); Serial.println(ws2812fx.getBrightness());
    }

    if(server.argName(i) == "s") {
      if(server.arg(i)[0] == '-') {
        ws2812fx.setSpeed(ws2812fx.getSpeed() * 0.8);
      } else {
        ws2812fx.setSpeed(max(ws2812fx.getSpeed(), 5) * 1.2);
      }
      Serial.print("speed is "); Serial.println(ws2812fx.getSpeed());
    }

    if(server.argName(i) == "a") {
      if(server.arg(i)[0] == '-') {
        auto_cycle = false;
      } else {
        auto_cycle = true;
        auto_last_change = 0;
      }
    }
  }
  server.send(200, "text/plain", "OK");
}

float getTemperature() {
  return dht.getTemperature();
}

void srv_handle_temp() {
  char temp[32];
  sprintf(temp, "Temperature : %.2f °C", getTemperature());
  server.send(200, "text/plain", temp);
}

float getHumidity() {
  return dht.getHumidity();
}

void srv_handle_hum() {
  char temp[32];
  sprintf(temp, "Humidity : %.2f %%", getHumidity());
  server.send(200, "text/plain", temp);
}

int getLuminosity() {
  return map(analogRead(ANALOG_PIN), 0, 1023, 0, 100);
}


void srv_handle_lum() {
  char temp[32];
  sprintf(temp, "Luminosity : %d %%", (int)round(getHumidity()));
  server.send(200, "text/plain", temp);
}
