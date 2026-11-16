import os

import paho.mqtt.client as paho
import psycopg
from dotenv import load_dotenv

load_dotenv()  # reads .env into os.environ

conn = psycopg.connect(
    host=os.environ["POSTGRES_HOST"],
    port=os.environ["POSTGRES_PORT"],
    user=os.environ["POSTGRES_USERNAME"],
    password=os.environ["POSTGRES_PASSWORD"],
    dbname=os.environ["POSTGRES_DATABASE"],
    autocommit=True,
)

def on_connect(client, userdata, flags, reason_code, properties):
    print(f"Connected: {reason_code}")
    client.subscribe("cat/readings", qos=1)


def on_message(client, userdata, msg):
    text = msg.payload.decode()
    try:
        float(text)
        conn.execute(
            "INSERT INTO readings (time, message) VALUES (now(), %s)",
            (text,),
        )
    except psycopg.Error as e:
        print(f"Skipped message on {msg.topic}: {e}")
        return

    print(f"{msg.topic}: {text}")


client = paho.Client(paho.CallbackAPIVersion.VERSION2, client_id="ingest", protocol=paho.MQTTv5)
client.on_connect = on_connect
client.on_message = on_message

client.connect("localhost", 1883)
client.loop_forever()
