#include "StepCountTask.h"
#include <drivers/Bma42x.h>
#include <components/stepcount/StepCountController.h>
#include <nrf_log.h>

using namespace Pinetime::Applications;

StepCountTask::StepCountTask(Drivers::Bma42x &stepCountSensor, Controllers::StepCountController& controller) :
      stepCountSensor{stepCountSensor},
      controller{controller} {
  messageQueue = xQueueCreate(10, 1);
  controller.SetStepCountTask(this);
}

void StepCountTask::Start() {
  if (pdPASS != xTaskCreate(StepCountTask::Process, "Stepcount", 500, this, 0, &taskHandle))
    APP_ERROR_HANDLER(NRF_ERROR_NO_MEM);
}

void StepCountTask::Process(void *instance) {
  auto *app = static_cast<StepCountTask *>(instance);
  app->Work();
}

void StepCountTask::Work() {
  int lastCount = 0;
  while (true) {
    Messages msg;
    uint32_t delay;
    if (state == States::Running) {
      if (measurementStarted) delay = 40;
      else delay = 100;
    } else
      delay = portMAX_DELAY;

    if (xQueueReceive(messageQueue, &msg, delay)) {
      switch (msg) {
        case Messages::GoToSleep:
          StopMeasurement();
          state = States::Idle;
          break;
        case Messages::WakeUp:
          state = States::Running;
          if(measurementStarted) {
            lastCount = 0;
            StartMeasurement();
          }
          break;
        case Messages::StartMeasurement:
          if(measurementStarted) break;
          lastCount = 0;
          StartMeasurement();
          measurementStarted = true;
          break;
        case Messages::StopMeasurement:
          if(!measurementStarted) break;
          StopMeasurement();
          measurementStarted = false;
          break;
      }
    }

    if (measurementStarted) {
      auto hrs = stepCountSensor.ReadHrs();
      auto count = ppg.StepCount();

      if (lastCount == 0 && count == 0) controller.Update(Controllers::StepCountController::States::NotEnoughData, 0);
      if(count != 0) {
        lastCount = count;
        controller.Update(Controllers::StepCountController::States::Running, lastCount);
      }
    }
  }
}

void StepCountTask::PushMessage(StepCountTask::Messages msg) {
  BaseType_t xHigherPriorityTaskWoken;
  xHigherPriorityTaskWoken = pdFALSE;
  xQueueSendFromISR(messageQueue, &msg, &xHigherPriorityTaskWoken);
  if (xHigherPriorityTaskWoken) {
    /* Actual macro used here is port specific. */
    // TODO : should I do something here?
  }
}

void StepCountTask::StartMeasurement() {
  stepCountSensor.Enable();
  vTaskDelay(100);
}

void StepCountTask::StopMeasurement() {
  stepCountSensor.Disable();
  vTaskDelay(100);
}
