import serial.tools.list_ports
import paho.mqtt.client as paho
import time

ports = serial.tools.list_ports.comports()
serialInst = serial.Serial()

portList = []

for onePort in ports:
    portList.append(str(onePort))
    print(str(onePort))


val = input("select port: COM")

for x in range(0,len(portList)):
    if portList[x].startswith("COM" + str(val)):
        portVar = "COM" + str(val)
        print(portList[x])


BROKER_NAME = "broker.hivemq.com"
PORT = 1883
TOPIC_PUB = "topic/pub"
TOPIC_SUB = "topic/pub"
fConnected = False

def on_connect(client, userdata, flags, rc):
    print(rc)

    if rc == 0:
        print("Client Connected to Broker!")
        global fConnected
        fConnected = True


def on_message(client, userdata, msg):
    print(msg.topic)
    print(msg.payload.decode("utf-8"))


def on_publish(client, userdata, msg):
    print("Message Published")


client = paho.Client("MQTT")
client.on_connect = on_connect
client.on_message = on_message
client.on_publish = on_publish
client.connect(BROKER_NAME,port = PORT)
client.loop_start()

while(fConnected != True):
    time.sleep(1)


client.subscribe(TOPIC_SUB)


serialInst.baudrate = 115200
serialInst.port = portVar
serialInst.open()

while True:
    if serialInst.in_waiting:
        packet = serialInst.readline()
        finalPacket = packet.decode('unicode_escape')
        client.publish(TOPIC_PUB, finalPacket)
        print(finalPacket)
        
