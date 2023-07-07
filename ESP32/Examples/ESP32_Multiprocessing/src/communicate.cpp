#include <Arduino.h>

QueueHandle_t queue;

#define BUFFER_SIZE 50

void p0_tx_task(void *arg) {
  char buffer[BUFFER_SIZE];
  queue = xQueueCreate(5, sizeof(buffer));
  if (queue == 0) {
    Serial.printf("Failed to create queue=%p\n", queue);
    vTaskDelete(xTaskGetCurrentTaskHandle());
  }
  Serial.printf("Successfully created queue=%p\n", queue);

  int counter = 0;

  while (1) {
    delay(1000);

    sprintf(buffer, "Hello from p0_tx_task: %d", counter);
    xQueueSendToBack(queue, (void *)buffer, (TickType_t)0);
    Serial.printf("Sending item %d\n", counter);
    counter++;
  }
}

void p1_rx_task(void *arg) {
    delay(2000);
    char buffer[BUFFER_SIZE];
    while (1) {
        if (xQueueReceive(queue, buffer, 1000)) {
            Serial.printf("p1_rx_task: got item: '%s'\n", buffer);
            delay(1000);
        } else {
            Serial.printf("p1_rx_task: could not receive message :(\n");
        }
    }
}
