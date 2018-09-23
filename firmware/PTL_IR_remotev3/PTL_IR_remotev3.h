/*
 * =====================================================================================
 *
 *       Filename:  test_PTL_IR_remote_shield.h
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

#ifndef __TEST_PTL_IR_H__
#define __TEST_PTL_IR_H__

#define OTA
bool updateOTA();


#define ANALOG_PIN  A0
#define DHT_PIN     5
#define IR_LED_PIN  0
#define IR_RECV_PIN 14
#define LED_PIN     2
#define LED_COUNT   2

#define ESPID           "NNNN"

#define FQDN(x)         x".local"
#define AP(x)           x"_rescue"
#define PREFIX(x)       "pub/"x


#define HOSTNAME        FQDN(ESPID)
#define WIFI_AP_NAME    AP(ESPID)
#define MQTTnamespace   PREFIX(ESPID)
#define MQTT_PORT       1883  // Default port used by MQTT servers.
#define MQTT_RECONNECT_TIME 5000 

#define FW_UPDATE_URL   "http://mqtt.localdomain:8080/firmware"
#define MQTT_SERVER     "CCCC"


#endif //__TEST_PTL_IR_H__
