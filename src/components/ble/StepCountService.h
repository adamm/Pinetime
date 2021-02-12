#pragma once
#define min // workaround: nimble's min/max macros conflict with libstdc++
#define max
#include <host/ble_gap.h>
#undef max
#undef min

namespace Pinetime {
  namespace System {
    class SystemTask;
  }
  namespace Controllers {
    class StepCountController;
    class StepCountService {
      public:
        StepCountService(Pinetime::System::SystemTask &system, Controllers::StepCountController& stepCountController);
        void Init();
        int OnStepCountRequested(uint16_t connectionHandle, uint16_t attributeHandle, ble_gatt_access_ctxt *context);
        void OnNewStepCountValue(uint8_t hearRateValue);

    private:
        Pinetime::System::SystemTask &system;
        Controllers::StepCountController& stepCountController;
        static constexpr uint16_t stepCountServiceId {0x180D};
        static constexpr uint16_t stepCountMeasurementId {0x2A37};

        static constexpr ble_uuid16_t stepCountServiceUuid {
                .u {.type = BLE_UUID_TYPE_16},
                .value = stepCountServiceId
        };

        static constexpr ble_uuid16_t stepCountMeasurementUuid {
                .u {.type = BLE_UUID_TYPE_16},
                .value = stepCountMeasurementId
        };

        struct ble_gatt_chr_def characteristicDefinition[3];
        struct ble_gatt_svc_def serviceDefinition[2];

        uint16_t stepCountMeasurementHandle;

    };
  }
}
