import paho.mqtt.client as mqtt
import pygame

# initialize pygame and set up the window
pygame.init()
width, height = 640, 480
screen = pygame.display.set_mode((width, height))
pygame.display.set_caption('MQTT-controlled Character SRX')

# setting up the character
x, y = width/2, height/2
speed = 2
character = pygame.Rect(x, y, 50, 50)

# MQTT client starts here
client = mqtt.Client()
client.connect("broker.hivemq.com", 1883)

def on_message(client, userdata, message):
    # get the message payload and decode it as a string
    newPayload = str(message.payload.decode('utf-8'))
    
    # parsing the payload as x and y values separated by a comma
    inKey = newPayload.split(" ")

    inKey[0]=float(inKey[0])
    # print(type(inKey[0]))
    x = inKey[0]

    inKey[1]=float(inKey[1])
    # print(type(inKey[1]))
    y = inKey[1]
    # x = float(inKey[0])
    # y = float(inKey[1])
    
    # move the character based on the received x and y values
    character.move_ip(x*speed, y*speed)

# subscribing to topic
client.subscribe("topic/pub")

# game loop starts here
running = True
while running:
    for event in pygame.event.get():
        if event.type == pygame.QUIT:
            running = False
            
    screen.fill((255, 255, 255))
    
    pygame.draw.rect(screen, (255, 0, 0), character)

    pygame.display.flip()
    
    # handling incoming MQTT messages
    client.on_message = on_message
    client.loop()
    
pygame.quit()
