import time
import os
import sys
import datetime
import Adafruit_GPIO
from Adafruit_MAX31856 import MAX31856 as MAX31856
from influxdb import InfluxDBClient

# Configure InfluxDB connection variables
host = "0.0.0.0"
port = 8086  # default port
# user =
# password =
dbname = "DBNAME"  # database name
interval = 60  # sample period in seconds

# Create the InfluxDB client object
client = InfluxDBClient(host=host, port=port, database=dbname)

# Raspberry Pi hardware SPI configuration.
SPI_PORT = 0
SPI_DEVICE = 0
sensor = MAX31856(hardware_spi=Adafruit_GPIO.SPI.SpiDev(SPI_PORT, SPI_DEVICE))

measurement = "rpi-max31856"
location = "lab"


def measure_temp_cpu():
    temp = os.popen("vcgencmd measure_temp").readline()
    temp = temp.replace("temp=", "").replace("'C\n", "")
    temp = float(temp)
    # print(temp)
    return (temp)


# Run until you get a ctrl^c
try:
    while True:
        # Read the sensor using the configured driver and gpio
        cpu_temp = measure_temp_cpu()
        end_temp = sensor.read_temp_c()
        ambient_temp = sensor.read_internal_temp_c()
        temperature_freezer = end_temp - ambient_temp
        iso = time.ctime()
        # Print for debugging, uncomment the below line
        print("[%s] Temp_freeze: %s Temp_ambient: %s Temp_CPU: %s" %
              (iso, temperature_freezer, ambient_temp, cpu_temp))
        # Create the JSON data structure
        data = [
            {
                "measurement": measurement,
                "tags": {
                    "location": location,
                },
                "time": iso,
                "fields": {
                    "temperature_freezer": temperature_freezer,
                    "ambient_temp": ambient_temp,
                    "cpu_temp": cpu_temp,
                }
            }
        ]
        # Send the JSON data to InfluxDB
        client.write_points(data)
        # Wait until it's time to query again...
        time.sleep(interval)

except KeyboardInterrupt:
    pass
