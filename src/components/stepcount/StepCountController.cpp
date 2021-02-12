#include "StepCountController.h"
#include <stepcounttask/StepCountTask.h>
#include <systemtask/SystemTask.h>

using namespace Pinetime::Controllers;

StepCountController::StepCountController(Pinetime::System::SystemTask &systemTask) : systemTask{systemTask} {

}


void StepCountController::Update(StepCountController::States newState, uint8_t stepCount) {
  this->state = newState;
  if(this->stepCount != stepCount) {
    this->stepCount = stepCount;
    service->OnNewStepCountValue(stepCount);
  }
}

void StepCountController::Start() {
  if(task != nullptr) {
    state = States::NotEnoughData;
    task->PushMessage(Pinetime::Applications::StepCountTask::Messages::StartMeasurement);
  }
}

void StepCountController::Stop() {
  if(task != nullptr) {
    state = States::Stopped;
    task->PushMessage(Pinetime::Applications::StepCountTask::Messages::StopMeasurement);
  }
}

void StepCountController::SetStepCountTask(Pinetime::Applications::StepCountTask *task) {
  this->task = task;
}

void StepCountController::SetService(Pinetime::Controllers::StepCountService *service) {
  this->service = service;
}

