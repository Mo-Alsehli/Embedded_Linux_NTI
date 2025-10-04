import paho.mqtt.client as mqtt
import time
import psutil # for ready laptop battery level.


def connection(client, userdata, flags, rc):
    if rc == 0:
        print("Connection established successfully to broker")
        client.subscribe("command")
        print("subscripbed to command topic...")
    else:
        print("Connection Failed !!!")


def message_arrived(client, userdata, msg):
    payload = msg.payload.decode()
    if(payload == "on"):
        print("Recieved <on>")
    elif(payload == "off"):
        print("Recieved <off>")
    else:
        print("unknown message")

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
    battery = psutil.sensors_battery()
    if battery:
        percent = int(battery.percent)
        client.publish("battery", percent)
        print("Published to topic battery: ", percent)
        time.sleep(5)
    else:
        print("Failed to publish on battery topic")

