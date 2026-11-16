import paho.mqtt.client as paho

client = paho.Client(paho.CallbackAPIVersion.VERSION2, protocol=paho.MQTTv5)

client.connect("localhost", 1883)
client.loop_start()

message = client.publish("cat/visit", payload="cat used the litter box", qos=1)
message.wait_for_publish()

client.loop_stop()
client.disconnect()
