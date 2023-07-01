import paho.mqtt.client as mqtt

# Define callback function for handling received messages
def on_message(client, userdata, message):
    print("Received message: " + str(message.payload.decode("utf-8")))

# Create MQTT client instance
client = mqtt.Client()

# Set username and password (if required)
# client.username_pw_set("username", "password")

# Set callback function for handling received messages
client.on_message = on_message

# Connect to HiveMQ broker
client.connect("broker.hivemq.com", 1883)

# Subscribe to topic
client.subscribe("topic/pub")

# Start the loop for receiving messages
client.loop_forever()
