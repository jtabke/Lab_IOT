#include <Arduino.h>

#include <ESP8266WiFi.h> //ESP8266 Core WiFi Library (you most likely already have this in your sketch)

#include <DNSServer.h>        //Local DNS Server used for redirecting all requests to the configuration portal
#include <ESP8266WebServer.h> //Local WebServer used to serve the configuration portal
#include <WiFiManager.h>      //https://github.com/tzapu/WiFiManager WiFi Configuration Magic
#include <InfluxDb.h>

#define INFLUXDB_HOST "0.0.0.0" //InfluxDB Host IP Address

Influxdb influx(INFLUXDB_HOST);

void setup()
{
  Serial.begin(9600);
  delay(3000);
  Serial.println(" ### Hello ###");

  //WiFiManager
  //Local intialization. Once its business is done, there is no need to keep it around
  WiFiManager wifiManager;
  //reset saved settings
  //wifiManager.resetSettings();

  //set custom ip for portal
  //wifiManager.setAPStaticIPConfig(IPAddress(10,0,1,1), IPAddress(10,0,1,1), IPAddress(255,255,255,0));

  //fetches ssid and pass from eeprom and tries to connect
  //if it does not connect it starts an access point with the specified name
  //here  "AutoConnectAP"
  //and goes into a blocking loop awaiting configuration
  wifiManager.autoConnect("AutoConnectAP");
  //or use this for auto generated name ESP + ChipID
  //wifiManager.autoConnect();

  //if you get here you have connected to the WiFi
  Serial.println("connected...yeey :)");

  influx.setDb("test");
  // Uncomment the following lines to use the v2.0 InfluxDB
  // influx.setVersion(2);
  // influx.setOrg("myOrganization");
  // influx.setBucket("myBucket");
  // influx.setToken("myToken");
  Serial.println("Setup done");
}

int loopCount = 0;

void loop()
{
  loopCount++;

  InfluxData row("temperature");
  row.addTag("device", "alpha");
  row.addTag("sensor", "one");
  row.addTag("mode", "pwm");
  row.addValue("loopCount", loopCount);
  row.addValue("value", random(10, 40));

  influx.write(row);

  delay(5000);
}