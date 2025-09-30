# Introduction To Networking


## MAC Address
- Each network interface (e.g. wifi, bluetooth) has a uniqe address.
- It identifies a specific netwrok interface.
- Ensures that frames reach the correct hardware.
- Used by routers to forward frames.

## What is an IP Address
- IP = Internet Protocol address.
- It's a unique idetifier for a device on a network.
- we can get the connected devices within the same network with this command `nmap -sn <ip>/24`

## Networks
### Netwrok Frame

### Local Network.
- Devices is connected with the same router in the same area.
- Each device in the local network has a unique IP.
### Global Network.
- It's a group of routers connected together.
- Public IP:
    - It's mainly for the routers that manages local network.
    - In this case the router uses the NAT to send the data to devices in the local area.
- Dynamic IP.
    - Usually dynamic IP are used for security purposes.
- Static IP.
    - Static IPs are less secure but it will be static all the time.
    - Static IPs depends on subnet when written.
    - `subnet` must be same for the devices that wants to communicate.


## Server.
- It's a services that listens for requests.
- Server is some sort of an applicatin running on a device.
- The server application allow the pc to send data.
- There server communications with the machine it's running on through port.

## Port.

## Client.

## DNS.


## Protocols
- Protocol is the form or way the communication between the server and client are transfering data.
- It defines how client requests a service from a server and vise versa.
- There are multiple types of protocols.
    - HTTP/HTTPs (Browsing websites/OTA).
    - MQTT/CoAP (IOT Device Communication).



