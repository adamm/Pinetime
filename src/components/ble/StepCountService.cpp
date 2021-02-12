#include "StepCountService.h"
#include "components/stepcount/StepCountController.h"
#include "systemtask/SystemTask.h"

using namespace Pinetime::Controllers;

constexpr ble_uuid16_t StepCountService::stepCountServiceUuid;
constexpr ble_uuid16_t StepCountService::stepCountMeasurementUuid;

namespace {
  int StepCountServiceServiceCallback(uint16_t conn_handle, uint16_t attr_handle, struct ble_gatt_access_ctxt *ctxt, void *arg) {
    auto* stepCountService = static_cast<StepCountService*>(arg);
    return stepCountService->OnStepCountRequested(conn_handle, attr_handle, ctxt);
  }
}

// TODO Refactoring - remove dependency to SystemTask
StepCountService::StepCountService(Pinetime::System::SystemTask &system, Controllers::StepCountController& stepCountController) :
        system{system},
        stepCountController{stepCountController},
        characteristicDefinition{
                {
                        .uuid = (ble_uuid_t *) &stepCountMeasurementUuid,
                        .access_cb = StepCountServiceServiceCallback,
                        .arg = this,
                        .flags = BLE_GATT_CHR_F_READ | BLE_GATT_CHR_F_NOTIFY,
                        .val_handle = &stepCountMeasurementHandle
                },
                {
                        0
                }
        },
        serviceDefinition{
                {
                        /* Device Information Service */
                        .type = BLE_GATT_SVC_TYPE_PRIMARY,
                        .uuid = (ble_uuid_t *) &stepCountServiceUuid,
                        .characteristics = characteristicDefinition
                },
                {
                        0
                },
        }{
  // TODO refactor to prevent this loop dependency (service depends on controller and controller depends on service)
  stepCountController.SetService(this);
}

void StepCountService::Init() {
  int res = 0;
  res = ble_gatts_count_cfg(serviceDefinition);
  ASSERT(res == 0);

  res = ble_gatts_add_svcs(serviceDefinition);
  ASSERT(res == 0);
}

int StepCountService::OnStepCountRequested(uint16_t connectionHandle, uint16_t attributeHandle,
                                           ble_gatt_access_ctxt *context) {
  if(attributeHandle == stepCountMeasurementHandle) {
    NRF_LOG_INFO("BATTERY : handle = %d", stepCountMeasurementHandle);
    static uint8_t batteryValue = stepCountController.StepCount();

    uint8_t buffer[2] = {0, stepCountController.StepCount()}; // [0] = flags, [1] = hr value

    int res = os_mbuf_append(context->om, buffer, 2);
    return (res == 0) ? 0 : BLE_ATT_ERR_INSUFFICIENT_RES;
  }
  return 0;
}

void StepCountService::OnNewStepCountValue(uint8_t stepCountValue) {
  uint8_t buffer[2] = {0, stepCountController.StepCount()}; // [0] = flags, [1] = hr value
  auto *om = ble_hs_mbuf_from_flat(buffer, 2);

  uint16_t connectionHandle = system.nimble().connHandle();

  if (connectionHandle == 0 || connectionHandle == BLE_HS_CONN_HANDLE_NONE) {
    return;
  }

  ble_gattc_notify_custom(connectionHandle, stepCountMeasurementHandle, om);
}
