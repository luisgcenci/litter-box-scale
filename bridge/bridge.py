import serial
import paho.mqtt.client as paho

PORT = "/dev/cu.usbmodem1201"  # replace with your Uno's port

ser = serial.Serial(PORT, 115200)

client = paho.Client(paho.CallbackAPIVersion.VERSION2, client_id="bridge", protocol=paho.MQTTv5)
client.connect("localhost", 1883)
client.loop_start()

while True:
    line = ser.readline().decode(errors="ignore").strip()
    try:
        weight = float(line)
    except ValueError:
        continue  # skip anything that isn't a number
    client.publish("cat/weight", payload=f"{weight:.1f}", qos=0)
    print(weight)