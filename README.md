# Lab Monitoring IOT system
This project is designed to be used as a low cost IOT solution for monitoring laboratory environments and processes. It was originally developed for the central monitoring of -80&deg;C lab freezers to ensure stored sample viability. This project can easily be extended for the monitoring of additional variables.

- [Lab Monitoring IOT system](#lab-monitoring-iot-system)
    - [TODO](#todo)
      - [Server](#server)
      - [ESP Nodes](#esp-nodes)
      - [Hardware](#hardware)
  - [Current stack](#current-stack)
    - [Hardware](#hardware-1)
      - [Server](#server-1)
      - [Nodes](#nodes)
    - [Software](#software)
      - [Server](#server-2)
      - [Nodes](#nodes-1)
  - [InfluxDB Setup](#influxdb-setup)
  - [NTP Time Sync Setup](#ntp-time-sync-setup)

### TODO
#### Server
- [ ] Raspberry pi setup procedure/script
- [ ] Set static IP for server
- [ ] Stack installation procedure/script
  - [ ] Setup Docker container
- [ ] Push notifications
- [ ] Add user and pass to InfluxDB and Grafana
#### ESP Nodes
- [ ] [OTA Updates?](https://arduino-esp8266.readthedocs.io/en/latest/ota_updates/readme.html)
- [ ] Add wiring table
- [ ] Deepsleep between measurements for nodes
- [ ] Batch write to Influx and store during deepsleep
- [ ] Read battery level through analog input
- [ ] Thermocouple measurements
  - [ ] Write to InfluxDB
#### Hardware
- [ ] Measure current consumption for battery life estimation
- [ ] Complete CAD modeling of case for nodes

## Current stack
### Hardware
#### Server
- Raspberry Pi
  - Running InfluxDB and Grafana
#### Nodes
- ESP-12f
  - [MAX31850K](https://learn.adafruit.com/adafruit-1-wire-thermocouple-amplifier-max31850k/wiring-and-test)
- 18650 LiPo Batteries
- [MCP1700](https://www.digikey.com/product-detail/en/microchip-technology/MCP1700-3302E-TO/MCP1700-3302E-TO-ND/652680) IC REG LINEAR 3.3V 250MA

### Software
#### Server
- Python 3
- [InfluxDB](https://www.influxdata.com/products/influxdb-overview/)
  - Time series database used to store data measurements 
- [Grafana](https://grafana.com/grafana/)
  - Used for visualization of live data, and adjustable time period visualization
#### Nodes
- C/C++
  - Written using [PlatformIO](https://docs.platformio.org/en/latest/platforms/espressif8266.html)
- Libraries
  - [InfluxDB Arduino Client](https://github.com/tobiasschuerg/InfluxDB-Client-for-Arduino)
  - OneWire
  - DallasTemperature
  - [WiFiManager](https://github.com/tzapu/WiFiManager)

## InfluxDB Setup
- Downsample and retain data [Docs](https://docs.influxdata.com/influxdb/v1.8/guides/downsample_and_retain/)

## NTP Time Sync Setup
- Server side
  - ~~Setup of [ntpdate](https://raspberrytips.com/time-sync-raspberry-pi/) on raspberry pi server~~
    - [ntpdate](https://ae.koroglu.org/how-to-sync-time-properly-ntpdate-or-ntpd/) is depreciated as of Sept. 2012 and only allows for single time syncing
  - Will use [ntpd](http://raspberrypi.tomasgreno.cz/ntp-client-and-server.html) instead
    - For this system the LAN blocks the UDP port required to sync with public NTP servers. Instead we will use the Pi to sync with NTP servers located on the LAN and will sync the nodes to the Pi.
      - The router of each subnet typically serves as a NTP server on the LAN. For example at IP: 10.x.x.1. To search the network for NTP servers the following [nmap](https://nmap.org/book/man-target-specification.html) script can be run on the specificed targets.
    - More info on the configuraiton of the ntp.conf file can be found [Here](https://access.redhat.com/documentation/en-us/red_hat_enterprise_linux/6/html/deployment_guide/s1-understanding_the_ntpd_configuration_file)
- ESP Node
  - [Example](https://github.com/SensorsIot/NTP-time-for-ESP8266-and-ESP32/blob/master/NTP_Example/NTP_Example.ino) of retriving NTP time from server and formatting. [Video](https://www.youtube.com/watch?v=r2UAmBLBBRM)
  - [POSIX time zones](https://github.com/nayarsystems/posix_tz_db/blob/master/zones.csv)
