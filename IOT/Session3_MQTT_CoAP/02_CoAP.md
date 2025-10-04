# CoAP – A Lightweight Protocol

> Constrained Application Protocol (CoAP)

---

## 1) What is CoAP?

CoAP is a **specialized web transfer protocol** for use with constrained nodes and constrained networks, often found in IoT applications. It is designed to be lightweight, energy-efficient, and simple compared to HTTP.

* **Lightweight** – optimized for resource-constrained devices (limited CPU, memory, and power).
* **Smaller code size** – CoAP implementations require far less code compared to HTTP.
* **Faster than HTTP** – because it runs over **UDP** instead of TCP.
* **Less reliable than HTTP** – since UDP does not guarantee delivery, but CoAP compensates with its own reliability mechanisms.
* **IoT-Oriented** – minimal handshake, better for sensors and embedded devices.

---

## 2) How CoAP Saves Power

### HTTP Model

1. Initial handshake (SYN + SYN-ACK) for TCP connection.
2. Send CONNECT request.
3. Data transfer begins only after multiple steps.
4. Devices must remain connected, preventing deep sleep modes.

### CoAP Model

1. Sends **only the data packet** containing useful payload.
2. Minimal control overhead (no TCP handshake).
3. Device can enter **sleep** or **deep sleep** immediately after transmission.
4. This reduces **energy consumption**, making it ideal for battery-powered IoT devices.

---

## 3) CoAP Methods

Like HTTP, CoAP supports **RESTful operations** on resources:

* **GET** – Retrieve resource state.
* **POST** – Create new resource or trigger action.
* **PUT** – Update/replace resource state.
* **DELETE** – Remove resource.

---

## 4) CoAP Message Types

CoAP defines four types of messages to balance reliability and efficiency:

1. **CON (Confirmable)** – Requires ACK, retransmitted until acknowledged.
2. **NON (Non-Confirmable)** – Fire-and-forget, no ACK required.
3. **ACK (Acknowledgement)** – Confirms receipt of a CON message.
4. **RST (Reset)** – Indicates message not understood or cannot be processed.

> **Key Difference:**
>
> * In **HTTP**, acknowledgments happen at the **TCP layer**.
> * In **CoAP**, acknowledgments happen at the **application layer**.

---

## 5) Writing a CoAP Server (Python Example)

In CoAP, endpoints are modeled as **resources** (similar to REST APIs).

* **Root URL:**

  ```
  coap://127.0.0.1
  ```
* **Resource Example:**

  ```
  coap://127.0.0.1/sensor/temperature
  ```

### Example with `aiocoap` Library

Install `aiocoap`:

```bash
pip install aiocoap
```

Create a simple CoAP server:

```python
import asyncio
from aiocoap import *

class CoAPResource(resource.Resource):
    async def render_get(self, request):
        return Message(payload=b"Hello from CoAP Server!")

async def main():
    root = resource.Site()
    root.add_resource(('hello',), CoAPResource())

    await Context.create_server_context(root)
    await asyncio.get_running_loop().create_future()

if __name__ == "__main__":
    asyncio.run(main())
```

### Running the Server

```bash
python coap_server.py
```

### Testing with CoAP Client

Install a client (e.g., `coap-client` from `libcoap` tools):

```bash
coap-client -m get coap://127.0.0.1/hello
```

Expected output:

```
Hello from CoAP Server!
```

---

## 6) Summary

* **CoAP = Lightweight HTTP for IoT.**
* Runs over **UDP** (faster, less overhead).
* Supports **REST methods** (GET, POST, PUT, DELETE).
* Provides **message types** (CON, NON, ACK, RST) for reliability.
* Designed for **power efficiency**, enabling devices to **sleep** after sending data.
* Easily implemented using Python with the `aiocoap` library.
