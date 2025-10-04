import asyncio
from aiocoap import *
import aiocoap.resource as resource

async def main():
    protocol = await Context.create_client_context()

    # GET request
    request = Message(code=GET, uri='coap://127.0.0.1/battery')
    response = await protocol.request(request).response
    print(f"GET Response: {response.payload.decode()}")

    # POST request
    request = Message(code=POST, uri='coap://127.0.0.1/battery', payload=b"Charging")
    response = await protocol.request(request).response
    print(f"POST Response: {response.payload.decode()}")

if __name__ == "__main__":
    asyncio.run(main())
