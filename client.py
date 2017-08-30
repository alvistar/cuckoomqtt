import paho.mqtt.client as mqtt
import pycuckoo
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

    print("message received ", str(message.payload.decode("utf-8")))
    print("message topic=", message.topic)
    print("message qos=", message.qos)
    print("message retain flag=", message.retain)

    if message.topic == "header":
        if state is states.IDLE:
            state = states.CALC
            header = str(message.payload.decode("utf-8"))
            nonce = 0
        
HOST_NAME = "localhost"

client = mqtt.Client(HOST_NAME)
client.on_message=on_message
client.connect(HOST_NAME)
client.loop_start()
client.subscribe("header")

while True:
    if state is states.CALC:
        solutions = pycuckoo.cuckoo(nonce, header)
        if len(solutions)>0:
            state = states.IDLE
            solstring = str(nonce)
            for nonce in solutions[0]:
                solstring = solstring + " " + hex (nonce)

            client.publish("clients/myid", solstring)
        nonce = nonce +1 

client.loop_stop()
