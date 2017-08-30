import paho.mqtt.client as mqtt
import pycuckoo
from enum import Enum, auto

class states (Enum):
    IDLE = auto()
    CALC= auto()

state = states.IDLE
header = "cane"
nonce = 0

def on_message(client, userdata, message):
    global state
    global header

    print("message received ", str(message.payload.decode("utf-8")))
    print("message topic=", message.topic)
    print("message qos=", message.qos)
    print("message retain flag=", message.retain)

    if message.topic.startswith("clients/"):
        solution = str(message.payload.decode("utf-8")).split()
        header_nonce = int(solution[0])
        nonces= [int(x,16) for x in solution[1:]]
        print(nonces)
        pycuckoo.verify(nonces, header=header, nonce = header_nonce)
        
HOST_NAME = "localhost"

client = mqtt.Client("client")
client.on_message=on_message
client.connect(HOST_NAME)
client.loop_start()
client.subscribe("clients/#")

while True:
    pass

client.loop_stop()
