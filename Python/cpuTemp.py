import os
import sys
import time
from influxdb import InfluxDBClient

# Configure InfluxDB connection variables
host = "0.0.0.0"
port = 8086  # default port
# user =
# password =
dbname = "DBNAME"  # database name
interval = 60  # sample period in seconds

measurement = "NAME"
location = "LOCATION"

# Create the InfluxDB client object
client = InfluxDBClient(host=host, port=port, database=dbname)


def measure_temp():
    temp = os.popen("vcgencmd measure_temp").readline()
    temp = temp.replace("temp=", "").replace("'C\n", "")
    temp = float(temp)
    # print(temp)
    return (temp)


try:
    while True:
        # Read the sensor using the configured driver and gpio
        temperature = measure_temp()
        iso = time.ctime()
        # Print for debugging, uncomment the below line
        print("[%s] Temp: %s" % (iso, temperature))
        # Create the JSON data structure
        data = [
            {
                "measurement": measurement,
                "tags": {
                    "location": location,
                },
                "time": iso,
                "fields": {
                    "temperature": temperature,
                }
            }
        ]
        # Send the JSON data to InfluxDB
        # client.write_points(data)
        # Wait until it's time to query again...
        time.sleep(interval)

except KeyboardInterrupt:
    pass
