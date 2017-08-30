import paho.mqtt.client as mqtt
import pycuckoo
import time
import random
from enum import Enum, auto

class states (Enum):
    IDLE = auto()
    CALC= auto()

state = states.IDLE
header = ""
nonce = 0

def on_message(client, userdata, message):
    global state
    global header

    print("-----------------------------------")
    print("MESSAGE RECEIVED")
    print("message received ", str(message.payload.decode("utf-8")))
    print("message topic=", message.topic)
    print("message qos=", message.qos)
    print("message retain flag=", message.retain)
    print("-----------------------------------")

    if message.topic.startswith("clients/"):
        id = message.topic.split("/")[1]
        print ("Got solution from {}".format(id))
        solution = str(message.payload.decode("utf-8")).split()
        header_nonce = int(solution[0])
        nonces= [int(x,16) for x in solution[1:]]
        print(nonces)
        if pycuckoo.verify(nonces, header=header, nonce = header_nonce):
            new_header()

def new_header():
    global header
    print ("Generating new header")
    header = time.ctime () + ":" + str(random.random())
    client.publish ("header", header)
    print ("Header sent: \"{}\"".format(header))

HOST_NAME = "localhost"

client = mqtt.Client("client")
client.on_message=on_message
client.connect(HOST_NAME)
client.loop_start()
client.subscribe("clients/#")
new_header()

while True:
    pass

client.loop_stop()
