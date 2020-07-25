import paho.mqtt.client as mqtt
import os
import time

local_path = "home/pi/log_"  # add file path
server_path = "home/pi/CPL/___"  # add full path

broker = "0.0.0.0"
port = 1883


def on_publish(client, userdata, result):  # create function for callback
    print("data published \n")
    pass


client1 = mqtt.Client("GreenPi")  # create client object
client1.on_publish = on_publish  # assign function to callback
client1.connect(broker, port)  # establish connection
# ret = client1.publish("Green/CpuTemp", "Hello basecamp!")             #publish


def measure_core_temp():
    temp = os.popen("vcgencmd measure_temp").readline()
    return (temp.replace("temp=", "").replace("'C\n", ""))


def log_data_local(temp, internal, elapsed):
    with open(local_path, 'a', newline='') as log:
        log.write("{0},{1},{2},{3}\n".format(time.strftime(
            "%Y-%m-%d %H:%M:%S"), str(elapsed), str(internal), str(temp)))


def log_data_server(temp, internal, elapsed):
    with open(server_path, 'a', newline='') as log:
        log.write("{0},{1},{2},{3}\n".format(time.strftime(
            "%Y-%m-%d %H:%M:%S"), str(elapsed), str(internal), str(temp)))


try:
    while True:
        ret = client1.publish("Green/CpuTemp", measure_core_temp())
        time.sleep(5)

except KeyboardInterrupt:
    pass
