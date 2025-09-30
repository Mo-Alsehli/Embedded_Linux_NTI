# MQTT Protocol


## MQTT Protocol.
- Message Queuing Telemetry Transport.
- It's a lightweight protocol designed for IOT constrained devices.
    - Low power consumption.
    - small code footprint.
    - reliable delivery options via Qos(quality of service).
- Works on publish/subscribe model.

## MQTT Architecture
- Broker (server).
    - central hub the recieves, filters and forwards messages.
- Clients (Devices, Apps)
    - Publishers (send messages on a topic).
    - Subscribers (recieve messages on a topic).

**How Broker Manages Communications between Nodes (Broker Service)**.
- A node(publisher) publishs data on a specific topic.
    - The function `publish()` is called whenever there is data need to be transfered.
- Another node(subscriber) fetch data from a subscribed topic.
    - The function `subscripe()` called only once.


### Loopback IP
- Always `127.0.0.1`


### MQTT Model with TCP/UDB