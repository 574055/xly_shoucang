# ESP32 MQTT
This example demonstrates using ESP32 as an MQTT client.

To see it in action, run ESP32 and prepare the online broker:

1. Open http://www.hivemq.com/demos/websocket-client/.
2. Connect to `broker.mqtt-dashboard.com`.
3. To send data from online to ESP...
    * Publish to the in-topic configured from ESP (e.g. `smart-tray-test-in-topic`).
4. To receive data from ESP:
    * Subscribe to the out-topic configured from ESP (e.g. `smart-tray-test-out-topic`).

Notes:
* This example uses the [`knolleary/PubSubClient`](https://github.com/knolleary/pubsubclient) library. You can leaf around on PlatformIO or GitHub for more examples of that library.
