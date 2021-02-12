#pragma once

#include <cstdint>
#include <components/ble/StepCountService.h>

namespace Pinetime {
  namespace Applications {
    class StepCountTask;
  }
  namespace System {
    class SystemTask;
  }
  namespace Controllers {
    class StepCountController {
    public:
      enum class States { Stopped, NotEnoughData, NoTouch, Running};

      explicit StepCountController(System::SystemTask& systemTask);

      void Start();
      void Stop();
      void Update(States newState, uint8_t stepCount);

      void SetStepCountTask(Applications::StepCountTask* task);
      States State() const { return state; }
      uint8_t StepCount() const { return stepCount; }

      void SetService(Pinetime::Controllers::StepCountService *service);

    private:
      System::SystemTask& systemTask;
      Applications::StepCountTask* task = nullptr;
      States state = States::Stopped;
      uint8_t stepCount = 0;
      Pinetime::Controllers::StepCountService* service = nullptr;
    };
  }
}