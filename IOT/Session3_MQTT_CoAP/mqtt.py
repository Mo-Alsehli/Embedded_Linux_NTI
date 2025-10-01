import paho.mqtt.client as mqtt
import time


def connection(client, userdata, flags, rc):
    if rc == 0:
        print("Connection established successfully to broker")
        client.subscribe("temp")
        print("subscripbed to temp topic...")
    else:
        print("Connection Failed !!!")


def message_arrived(client, userdata, msg):
    print(client._client_id.decode(), "Recieved: \n", "data: ", msg.payload.decode(), "On Topic: ", msg.topic)


broker_ip = "127.0.0.1"
broker_port = 1883

client = mqtt.Client("magdi")
client.on_connect = connection
client.on_message = message_arrived
try:
    client.connect(broker_ip, broker_port)
except:
    print("Connection failed")

client.loop_start()


while True:
    pass
