# MQTT Protocol

## Overview

**MQTT (Message Queuing Telemetry Transport)** is a **lightweight messaging protocol** optimized for **IoT and embedded systems** where bandwidth and power are limited.
It’s ideal for low-power, low-bandwidth, and unreliable networks.

### Key Features

* **Low Power Consumption** — minimal handshake and data overhead.
* **Small Code Footprint** — fits in microcontrollers and small devices.
* **Reliable Delivery Options (QoS)** — ensures message delivery according to reliability needs.
* **Bidirectional Communication** — devices can both send and receive updates.
* **TCP-based Transport Layer** — provides ordered, reliable packet delivery.

---

## MQTT Architecture

MQTT follows a **Publish/Subscribe (Pub/Sub)** communication model.

| Component                  | Role        | Description                                                                                            |
| -------------------------- | ----------- | ------------------------------------------------------------------------------------------------------ |
| **Broker (Server)**        | Central Hub | Receives all messages, filters them by topic, and forwards them to clients subscribed to those topics. |
| **Clients (Devices/Apps)** | End Nodes   | Devices that connect to the broker — either as publishers, subscribers, or both.                       |

---

## How Broker Manages Communication Between Nodes

1. **Publisher Node**

   * Uses the function `publish()` to send data to a **specific topic** (e.g., `sensor/temp`).
   * Example: a temperature sensor publishes readings every second.

2. **Subscriber Node**

   * Uses the function `subscribe()` **once** to register interest in that topic.
   * Whenever new data is published, the broker automatically forwards it.

3. **Broker Role**

   * Acts as a **middle layer** between publishers and subscribers.
   * Ensures proper message delivery according to **QoS** level.
   * Manages client sessions and message buffering if clients disconnect.

---

## Quality of Service (QoS) Levels

| QoS   | Guarantee     | Behavior                                                           |
| ----- | ------------- | ------------------------------------------------------------------ |
| **0** | At most once  | “Fire and forget.” Message may be lost.                            |
| **1** | At least once | Guaranteed delivery but can be duplicated.                         |
| **2** | Exactly once  | Guaranteed single delivery. Highest reliability, highest overhead. |

---

## MQTT Topic Hierarchy

* Topics are structured using **slashes `/`** as delimiters.
* Example:

  ```
  home/livingroom/temperature
  home/livingroom/humidity
  ```
* Wildcards can be used:

  * `+` for single-level (`home/+/temperature`)
  * `#` for multi-level (`home/#`)

---

## MQTT Loopback and Network Setup

### Loopback IP

* **`127.0.0.1`** refers to the local device itself.
* Used when broker and clients run on the **same machine** for testing.

### Typical Connection Flow

1. **Broker** starts listening on port **1883** (default MQTT port).
2. **Client (Publisher)** connects to broker via TCP socket.
3. **Client (Subscriber)** connects and subscribes to a topic.
4. Messages flow through the broker between connected nodes.

---

## MQTT Over TCP/UDP Model

| Protocol             | Usage               | Characteristics                                                                    |
| -------------------- | ------------------- | ---------------------------------------------------------------------------------- |
| **MQTT over TCP**    | Standard mode       | Reliable, ordered delivery using TCP’s built-in acknowledgment and retransmission. |
| **MQTT-SN over UDP** | For sensor networks | Lightweight, no TCP overhead, suitable for constrained or wireless devices.        |

**Diagram:**

```
+----------------+         TCP/UDP         +----------------+
|  Publisher(s)  |  --->  (MQTT Message)  --->  |   Broker   |
| (sensors, apps)|                           | (Mosquitto)  |
+----------------+         <--- Publish --- +----------------+
            |                                      |
            |------------> Subscribers ------------|
```

---

## Example Communication Flow

1. **Publisher:**

   ```python
   client.publish("home/temp", "25°C")
   ```
2. **Subscriber:**

   ```python
   client.subscribe("home/temp")
   ```
3. **Broker:**

   * Receives the message on `home/temp`.
   * Delivers it to all subscribed clients.

---

## Summary

* **MQTT = Lightweight, reliable, power-efficient protocol** for IoT.
* **Broker** acts as a message router.
* **Clients** can publish, subscribe, or both.
* Operates over **TCP** (standard) or **UDP** (MQTT-SN).
* Provides **QoS levels** for message reliability.
* Commonly used with brokers like **Mosquitto** or **HiveMQ** for real IoT applications.
