#include <Arduino.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <esp_ipc.h>

void setup() {
  Serial.begin(115200);
}

void LoopOnProCpu(void *arg) {
  (void)arg;
  Serial.println("This loop runs on PRO_CPU which id is:");
  Serial.println(xPortGetCoreID());
  Serial.println();
  Serial.println();
}

void loop() {
  //Default loop runs on APP_CPU
  Serial.println("This loop runs on APP_CPU which id is:");
  Serial.println(xPortGetCoreID());
  Serial.println();
  Serial.println();
  //Execute LoopOnAppCpu on PRO_CPU
  esp_ipc_call(PRO_CPU_NUM, LoopOnProCpu, NULL);
}
