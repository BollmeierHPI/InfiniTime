#pragma once
#include <cstdint>
#include <array>

#include <host/ble_gap.h>

namespace Pinetime {
  namespace System {
    class SystemTask;
  }
  namespace Drivers {
    class SpiNorFlash;
  }
  namespace Controllers {
    class Ble;
    class DfuService {
      public:
        DfuService(Pinetime::System::SystemTask& systemTask, Pinetime::Controllers::Ble& bleController,
                   Pinetime::Drivers::SpiNorFlash& spiNorFlash);
        void Init();
        void Validate();

        int OnServiceData(uint16_t connectionHandle, uint16_t attributeHandle, ble_gatt_access_ctxt *context);
      private:
        Pinetime::System::SystemTask& systemTask;
        Pinetime::Controllers::Ble& bleController;
        Pinetime::Drivers::SpiNorFlash& spiNorFlash;

        static constexpr uint16_t dfuServiceId {0x1530};
        static constexpr uint16_t packetCharacteristicId {0x1532};
        static constexpr uint16_t controlPointCharacteristicId {0x1531};
        static constexpr uint16_t revisionCharacteristicId {0x1534};

        uint16_t revision {0x0008};

        static constexpr ble_uuid128_t serviceUuid {
                .u { .type = BLE_UUID_TYPE_128},
                .value = {0x23, 0xD1, 0xBC, 0xEA, 0x5F, 0x78, 0x23, 0x15,
                          0xDE, 0xEF, 0x12, 0x12, 0x30, 0x15, 0x00, 0x00}
        };

        static constexpr ble_uuid128_t packetCharacteristicUuid {
                .u { .type = BLE_UUID_TYPE_128},
                .value = {0x23, 0xD1, 0xBC, 0xEA, 0x5F, 0x78, 0x23, 0x15,
                          0xDE, 0xEF, 0x12, 0x12, 0x32, 0x15, 0x00, 0x00}
        };

        static constexpr ble_uuid128_t controlPointCharacteristicUuid {
                .u { .type = BLE_UUID_TYPE_128},
                .value = {0x23, 0xD1, 0xBC, 0xEA, 0x5F, 0x78, 0x23, 0x15,
                          0xDE, 0xEF, 0x12, 0x12, 0x31, 0x15, 0x00, 0x00}
        };

        static constexpr ble_uuid128_t revisionCharacteristicUuid {
                .u { .type = BLE_UUID_TYPE_128},
                .value = {0x23, 0xD1, 0xBC, 0xEA, 0x5F, 0x78, 0x23, 0x15,
                          0xDE, 0xEF, 0x12, 0x12, 0x34, 0x15, 0x00, 0x00}
        };

        struct ble_gatt_chr_def characteristicDefinition[4];
        struct ble_gatt_svc_def serviceDefinition[2];
        uint16_t packetCharacteristicHandle;
        uint16_t controlPointCharacteristicHandle;
        uint16_t revisionCharacteristicHandle;

        enum class States : uint8_t {Idle, Init, Start, Data, Validate, Validated};
        States state = States::Idle;

        enum class ImageTypes : uint8_t {
            NoImage = 0x00,
            SoftDevice = 0x01,
            Bootloader = 0x02,
            SoftDeviceAndBootloader = 0x03,
            Application = 0x04
        };

        enum class Opcodes : uint8_t {
            StartDFU = 0x01,
            InitDFUParameters = 0x02,
            ReceiveFirmwareImage = 0x03,
            ValidateFirmware = 0x04,
            ActivateImageAndReset = 0x05,
            PacketReceiptNotificationRequest = 0x08,
            Response = 0x10,
            PacketReceiptNotification = 0x11
        };

        enum class ErrorCodes { NoError = 0x01};

        uint8_t nbPacketsToNotify = 0;
        uint32_t nbPacketReceived = 0;
        uint32_t bytesReceived = 0;
        uint32_t writeOffset = 0x40000;

        uint32_t softdeviceSize = 0;
        uint32_t bootloaderSize = 0;
        uint32_t applicationSize = 0;
        static constexpr uint32_t maxImageSize = 475136;

        int SendDfuRevision(os_mbuf *om) const;
        void SendNotification(uint16_t connectionHandle, const uint8_t *data, const size_t size);
        int WritePacketHandler(uint16_t connectionHandle, os_mbuf *om);
        int ControlPointHandler(uint16_t connectionHandle, os_mbuf *om);

        uint8_t tempBuffer[200];
        uint16_t ComputeCrc(uint8_t const * p_data, uint32_t size, uint16_t const * p_crc);

        bool firstCrc = true;
        uint16_t tempCrc = 0;

        void WriteMagicNumber();
    };
  }
}