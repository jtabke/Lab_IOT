# Global Imports
from Adafruit_MAX31856 import MAX31856 as MAX31856
import logging
import time
import Adafruit_GPIO
import csv

data_file_path = "/home/pi/usbdrv/temp_log.csv"

# Local Imports


def log_data(temp, internal, elapsed):
    with open(data_file_path, 'a', newline='') as log:
        log.write("{0},{1},{2},{3}\n".format(time.strftime(
            "%Y-%m-%d %H:%M:%S"), str(elapsed), str(internal), str(temp)))

#logging.basicConfig(filename='simpletest.log', level=logging.DEBUG, format='%(asctime)s - %(name)s - %(levelname)s - %(message)s')
#_logger = logging.getLogger(__name__)

# Uncomment one of the blocks of code below to configure your Pi to use software or hardware SPI.


# Raspberry Pi software SPI configuration.
software_spi = {"clk": 25, "cs": 8, "do": 9, "di": 10}
sensor = MAX31856(software_spi=software_spi)

# Raspberry Pi hardware SPI configuration.
#SPI_PORT   = 0
#SPI_DEVICE = 0
#sensor = MAX31856(hardware_spi=Adafruit_GPIO.SPI.SpiDev(SPI_PORT, SPI_DEVICE))


start = time.time()
# Loop printing measurements every second.
print('Press Ctrl-C to quit.')
while True:
    temp = sensor.read_temp_c()
    internal = sensor.read_internal_temp_c()
    end = time.time()
    elapsed = end - start
    print('Thermocouple Temperature: {0:0.3F}*C'.format(temp))
    print('    Internal Temperature: {0:0.3F}*C'.format(internal))
    print(elapsed)
    log_data(temp, internal, elapsed)
    time.sleep(10.0)
