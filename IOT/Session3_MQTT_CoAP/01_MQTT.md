# MQTT

## 1) MQTT Model and Layers

MQTT (Message Queuing Telemetry Transport) is a lightweight publish/subscribe messaging protocol designed for constrained devices and low-bandwidth, high-latency networks.

It relies on a layered structure:

1. **Application Layer**

   * Defines how publishers and subscribers exchange messages through topics.
   * MQTT client libraries are implemented here.

2. **Transport Layer (TCP/UDP)**

   * MQTT usually runs over **TCP** for reliable, ordered delivery.
   * In some lightweight variants (MQTT-SN), UDP may be used.

3. **Network Layer (IP)**

   * Provides addressing and routing over IP networks.
   * Ensures packets are sent to the correct broker/device.

This layered model ensures reliable communication between IoT devices, even with unstable connections.

---

## 2) Implementing an MQTT Client with Mosquitto Server

Mosquitto is an open-source MQTT broker widely used for testing and deployment.

**Solid Steps to Implement:**

1. **Install Mosquitto (Broker + Client tools)**

   ```bash
   sudo apt update
   sudo apt install mosquitto mosquitto-clients
   sudo systemctl enable mosquitto
   sudo systemctl start mosquitto
   ```

2. **Test with Basic Clients**

   * Open two terminals.
   * **Subscriber:**

     ```bash
     mosquitto_sub -h localhost -t "test/topic"
     ```
   * **Publisher:**

     ```bash
     mosquitto_pub -h localhost -t "test/topic" -m "Hello MQTT"
     ```
   * The subscriber terminal should display the published message.

3. **Write a Python MQTT Client (using `paho-mqtt`)**

   ```python
   import paho.mqtt.client as mqtt

   def on_connect(client, userdata, flags, rc):
       print("Connected with result code", rc)
       client.subscribe("test/topic")

   def on_message(client, userdata, msg):
       print(f"Message received: {msg.payload.decode()} on topic {msg.topic}")

   client = mqtt.Client("client1")
   client.on_connect = on_connect
   client.on_message = on_message

   client.connect("localhost", 1883, 60)
   client.loop_forever()
   ```

---

## 3) Using Node-RED for GUI

Node-RED provides a visual flow-based environment to connect MQTT messages with dashboards.

**Steps:**

1. **Install Node-RED**

   ```bash
   sudo npm install -g --unsafe-perm node-red
   node-red
   ```

2. **Access GUI**

   * Open browser: `http://localhost:1880`
   * Drag **MQTT In** and **MQTT Out** nodes from the palette.
   * Configure broker as `localhost:1883`.

3. **Add Dashboard**

   * Install dashboard nodes:

     ```bash
     cd ~/.node-red
     npm install node-red-dashboard
     ```
   * Access UI at: `http://localhost:1880/ui`
   * You can add buttons, switches, and charts linked to MQTT topics.

---

## 4) Difference Between MQTT and HTTP

| Feature           | MQTT                             | HTTP                        |
| ----------------- | -------------------------------- | --------------------------- |
| **Protocol Type** | Publish/Subscribe messaging      | Request/Response            |
| **Transport**     | TCP (lightweight)                | TCP (heavy headers)         |
| **Overhead**      | Very small (2 bytes)             | High (header-heavy)         |
| **Communication** | Asynchronous (broker forwards)   | Synchronous (client–server) |
| **Use Case**      | IoT, sensor data, real-time apps | Web pages, REST APIs        |
| **Power Usage**   | Low (good for embedded devices)  | Higher (not IoT friendly)   |

**Summary:**

* **MQTT** is best for IoT and real-time communication.
* **HTTP** is best for web browsing and traditional client-server communication.
