# ESP32 Multiprocessing
This example demonstrates multiprocessing and interprocess communication with an ESP32 board.

See main.cpp.

Notes:
* The `loop()` function is run in Core 1.
* The `loop2()` function is pinned to and run in Core 0.
* We can get the currently running core using `xPortGetCoreID()`.
* To assign a task to a core, declare a function with signature `void foo(void* params)` and pass it to [`xTaskCreatePinnedToCore`](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/system/freertos_idf.html#_CPPv423xTaskCreatePinnedToCore14TaskFunction_tPCKcK22configSTACK_DEPTH_TYPEPCv11UBaseType_tPC12TaskHandle_tK10BaseType_t).
* We use an RTOS queue to send data from one task to another (see communication.cpp).

Further reading: [How to Write Parallel Multitasking Applications for ESP32 using FreeRTOS & Arduino](https://circuitstate.com/tutorials/how-to-write-parallel-multitasking-applications-for-esp32-using-freertos-arduino/)