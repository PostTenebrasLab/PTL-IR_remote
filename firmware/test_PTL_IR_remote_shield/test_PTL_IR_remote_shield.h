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


#define ESPID           "leman15"

#define FQDN(x)         x".local"
#define AP(x)           x"_rescue"

#define HOSTNAME        FQDN(ESPID)
#define WIFI_AP_NAME    AP(ESPID)

#define FW_UPDATE_URL   "http://172.16.10.128:8080/firmware"

void setup_wifi();
void resetWifi();
void stopWifiUDP();

#endif //__TEST_PTL_IR_H__
