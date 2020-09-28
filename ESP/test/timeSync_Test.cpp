#include <Arduino.h>

// *** Wifi ***
#include <ESP8266WiFi.h> //ESP8266 Core WiFi Library (you most likely already have this in your sketch)

#include <DNSServer.h>        //Local DNS Server used for redirecting all requests to the configuration portal
#include <ESP8266WebServer.h> //Local WebServer used to serve the configuration portal
#include <WiFiManager.h>      //https://github.com/tzapu/WiFiManager WiFi Configuration Magic

#define AP_PASSWORD "ap_password"

// *** Time Sync ***
#include <time.h>

#define NTP_SERVER "0.0.0.0"
#define TZ_INFO "MST7MDT,M3.2.0,M11.1.0"
tm timeinfo;
time_t now;

// #define WRITE_PRECISION WritePrecision::S
// #define LOOP_WAIT_TIME 120e6 //120 seconds between measurements

void timeSync()
{
    // Synchronize UTC time with NTP servers
    // Accurate time is necessary for certificate validaton and writing in batches
    configTime(0, 0, NTP_SERVER);
    // See https://github.com/nayarsystems/posix_tz_db/blob/master/zones.csv for Timezone codes for your region
    setenv("TZ", TZ_INFO, 1);

    // Wait till time is synced
    Serial.print("Syncing time");
    int i = 0;
    while (time(nullptr) < 1000000000ul && i < 100)
    {
        Serial.print(".");
        delay(100);
        i++;
    }
    Serial.println();

    // Show time
    now = time(nullptr);
    Serial.println();
    Serial.print("Synchronized time: ");
    Serial.println(String(ctime(&now)));
}

void wifi_setup()
{
    //WiFiManager
    //Local intialization. Once its business is done, there is no need to keep it around
    WiFiManager wifiManager;
    //reset saved settings - for testing
    //wifiManager.resetSettings();

    //fetches ssid and pass from eeprom and tries to connect
    //if it does not connect it starts an access point with the specified name
    //here  "AutoConnectAP"
    //and goes into a blocking loop awaiting configuration
    wifiManager.setConfigPortalTimeout(120); //timout after 2min
    if (!wifiManager.autoConnect("CPL_Node_AutoConnectAP", AP_PASSWORD))
    {
        //Serial.println("failed to connect and hit timeout");
        //delay(3000);
        //reset and try again, or maybe put it to deep sleep
        //ESP.reset();
        //delay(5000);
        ESP.deepSleep();
    }
    //if you get here you have connected to the WiFi
    Serial.println("connected...yeey :)");
}

void setup()
{
    Serial.Begin(9600);
    while (!Serial)
        ;
    wifi_setup();

    // Sync time for certificate validation
    //timeSync();
    timeClient.begin();
}

void loop()
{
    // Show time
    now = time(nullptr);
    Serial.println();
    Serial.print("Synchronized time: ");
    Serial.println(String(ctime(&now)));
    delay(2000);
}