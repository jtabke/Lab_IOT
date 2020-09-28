#include <Arduino.h>

//***MAX31850***
#include <OneWire.h>
#include <DallasTemperature.h>

// Data wire is plugged into port 2 on the Arduino
#define ONE_WIRE_BUS 2

// Setup a oneWire instance to communicate with any OneWire devices (not just Maxim/Dallas temperature ICs)
OneWire oneWire(ONE_WIRE_BUS);

// Pass our oneWire reference to Dallas Temperature.
DallasTemperature sensors(&oneWire);
DeviceAddress addr;

//***Wifi***
#include <ESP8266WiFi.h> //ESP8266 Core WiFi Library (you most likely already have this in your sketch)

#include <DNSServer.h>        //Local DNS Server used for redirecting all requests to the configuration portal
#include <ESP8266WebServer.h> //Local WebServer used to serve the configuration portal
#include <WiFiManager.h>      //https://github.com/tzapu/WiFiManager WiFi Configuration Magic

#define AP_PASSWORD "ap_password"

//***InfluxDB***
#include <InfluxDbClient.h>

// InfluxDB server url, e.g. http://192.168.1.48:8086 (don't use localhost, always server name or ip address)
#define INFLUXDB_URL "influxdb-url"
// InfluxDB database name
#define INFLUXDB_DB_NAME "database_name"
//***
// Client device ID, unique for each node
#define DEVICE "esp_###"
//***
#define TZ_INFO "MST7MDT,M3.2.0,M11.1.0"
#define WRITE_PRECISION WritePrecision::S
#define LOOP_WAIT_TIME 120e6 //120 seconds between measurements

// Single InfluxDB 1 instance
InfluxDBClient client(INFLUXDB_URL, INFLUXDB_DB_NAME);

// Data point
Point sensorTemp("freezer_temperature");
Point wifiStatus("wifi_status");

void timeSync()
{
  // Synchronize UTC time with NTP servers
  // Accurate time is necessary for certificate validaton and writing in batches
  configTime(0, 0, "pool.ntp.org", "time.nis.gov");
  // Set timezone
  setenv("TZ", TZ_INFO, 1); //https://github.com/G6EJD/ESP32-Time-Services-and-SETENV-variable //Needs to be tested***

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
  time_t tnow = time(nullptr);
  Serial.print("Synchronized time: ");
  Serial.println(String(ctime(&tnow)));
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
  //Serial.println("connected...yeey :)");
}

void setup()
{
  //keep wifi off when waking up from deepsleep to reduce battery draina
  WiFi.mode(WIFI_OFF);
  WiFi.forceSleepBegin();
  delay(1);

  sensors.begin();
  if (!sensors.getAddress(insideThermometer, 0))
    Serial.println("Unable to find address for Device 0");
  //Serial.begin(9600);
  //delay(3000);
  //Serial.println(" ### Hello ###");
  WiFi.forceSleepWake();
  delay(1);
  wifi_setup();

  // Sync time for certificate validation
  timeSync();
  // Set InfluxDB 1 authentication params
  client.setConnectionParamsV1(INFLUXDB_URL, INFLUXDB_DB_NAME, INFLUXDB_USER, INFLUXDB_PASSWORD);

  // Add constant tags - only once
  wifiStatus.addTag("device", DEVICE);
  wifiStatus.addTag("SSID", WiFi.SSID()); //** alter for wifimanager lib

  // Check server connection
  if (client.validateConnection())
  {
    Serial.print("Connected to InfluxDB: ");
    Serial.println(client.getServerUrl());
  }
  else
  {
    Serial.print("InfluxDB connection failed: ");
    Serial.println(client.getLastErrorMessage());
  }

  // Enable messages batching
  client.setWriteOptions(WRITE_PRECISION);
}

void loop()
{
  Serial.println();
  Serial.print("Synchronized time: ");
  Serial.println(String(ctime(&tnow)));
}