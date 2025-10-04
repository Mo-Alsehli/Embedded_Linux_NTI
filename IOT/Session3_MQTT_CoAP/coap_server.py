import asyncio
from aiocoap import *
import aiocoap.resource as resource

class BatteryResource(resource.Resource):
    async def render_get(self, request):
        print("GET request received")
        payload = b"Battery: 85%"   # mock value, later can use psutil
        return Message(payload=payload)

    async def render_post(self, request):
        print(f"POST request with data: {request.payload.decode()}")
        return Message(code=CONTENT, payload=b"POST received")

async def main():
    root = resource.Site()
    root.add_resource(['battery'], BatteryResource())
    await Context.create_server_context(root, bind=('127.0.0.1', 5683))

    print("CoAP Server running at coap://127.0.0.1:5683")
    await asyncio.get_running_loop().create_future()  # keep running

if __name__ == "__main__":
    asyncio.run(main())
