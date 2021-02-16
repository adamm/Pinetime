#pragma once
#include <FreeRTOS.h>
#include <task.h>
#include <queue.h>


namespace Pinetime {
  namespace Drivers {
    class Bma42x;
  }
  namespace Controllers{
    class StepCountController;
  }
  namespace Applications {
    class StepCountTask {
    public:
      enum class Messages : uint8_t {GoToSleep, WakeUp, StartMeasurement, StopMeasurement };
      enum class States {Idle, Running};

      explicit StepCountTask(Drivers::Bma42x& stepCountSensor, Controllers::StepCountController& controller);
      void Start();
      void Work();
      void PushMessage(Messages msg);

    private:
      static void Process(void* instance);
      void StartMeasurement();
      void StopMeasurement();

      TaskHandle_t taskHandle;
      QueueHandle_t messageQueue;
      States state = States::Running;
      Drivers::Bma42x &stepCountSensor;
      Controllers::StepCountController& controller;
      bool measurementStarted = false;

    };

  }
}

