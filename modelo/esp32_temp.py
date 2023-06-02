import picamera
import time
import os
import datetime
from PIL import Image
import tensorflow as tf
from tensorflow.keras.optimizers import Adam
import numpy as np
import signal
import sys
import paho.mqtt.client as paho

broker = "172.20.10.7"
port = 1883



def photo(name):
    camera = picamera.PiCamera()
    os.environ.setdefault('XAUTHORITY', '/home/user/.Xauthority')
    os.environ.setdefault('DISPLAY', ':0.0')
    date = str(datetime.date.today())
    hrs = time.strftime("%H:%M:%S")
    
    camera.resolution = (1024, 768)
    camera.start_preview()
    time.sleep(3)
    nameFile = name
    camera.capture(nameFile)
    camera.stop_preview()
    camera.close()
    time.sleep(4)
    return nameFile

def modelo(photoName):
    model = tf.keras.models.load_model('/home/pi/Desktop/project/model.h5',compile=False)
    model.compile(optimizer=Adam(learning_rate=1e-3),
                loss='sparse_categorical_crossentropy',
                metrics=['sparse_categorical_accuracy'])
    image = Image.open(photoName).convert('L')
    image = image.resize((28,28))
    image = np.asarray(image).astype(np.float32).reshape(28,28,1)

    return model.predict(np.array([image]))
    
def on_publish(client,userdata,result):
    #print("data publich\n")
    pass


    

def start():
    kat = ['T-shirt/top',
'Trouser',
'Pullover',
'Dress',
'Coat',
'Sandal',
'Shirt',
'Sneaker',
'Bag',
'Ankle boot']
    
    name = f"temp.png"
    ret= client1.publish("esp32/motors","banda")
    photo(name)
    detec = modelo(name)
    print(kat[np.argmax(detec)])
    ret= client1.publish("esp32/motors","door")
    time.sleep(5)
    ret= client1.publish("esp32/motors","cerrar")
    #print("Take the garment")

client1 = paho.Client("controlMotors")
client1.on_publish= on_publish
client1.connect(broker,port)
start()
