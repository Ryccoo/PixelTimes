#ifndef PixelWifi_H
#define PixelWifi_H

#include <WiFiManager.h>

void saveConfigCallback () {
	Serial.println("saveConfigCallback");
	// shouldSaveWifiConfig = true;
}
void configModeCallback (WiFiManager *myWiFiManager) {
	Serial.println("Entered config mode");
	Serial.println(WiFi.softAPIP());
	Serial.println(myWiFiManager->getConfigPortalSSID());
}


void setupWifi() {
	WiFiManager wifiManager;
//  wifiManager.resetSettings(); 
 	wifiManager.setSaveConfigCallback(saveConfigCallback);
 	wifiManager.setAPCallback(configModeCallback);
	wifiManager.setMinimumSignalQuality();
	wifiManager.setTimeout(300);
 	// WiFi.setSleepMode(WIFI_NONE_SLEEP);
	Serial.println ("start wifi connection");
//  wifiManager.resetSettings(); // for testing / debugging
	if (!wifiManager.autoConnect("PixelTime", "foobar123")) {
		Serial.println("failed to connect and hit timeout");
		delay(3000);
		ESP.restart();
		delay(5000);
 	}
 	Serial.println ("wifi connected ok");
 	Serial.println(WiFi.localIP());
}

#endif