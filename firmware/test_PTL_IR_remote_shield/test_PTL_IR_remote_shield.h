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


#define ESPID           "lm00"

#define FQDN(x)         x".local"
#define AP(x)           x"_rescue"
#define PREFIX(x)       "pub/"x"/workshop_test"


#define HOSTNAME        FQDN(ESPID)
#define WIFI_AP_NAME    AP(ESPID)
#define MQTTnamespace   PREFIX(ESPID)
#define MQTT_PORT       1883  // Default port used by MQTT servers.
#define MQTT_RECONNECT_TIME 5000 

#define FW_UPDATE_URL   "http://mqtt.localdomain:8080/firmware"
#define MQTT_SERVER     "mqtt.localdomain"

void setup_wifi();
//void resetWifi();
//void stopWifiUDP();

#endif //__TEST_PTL_IR_H__
