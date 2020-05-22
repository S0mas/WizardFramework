#include "../include/DeviceT028.h"
#include "../../../bu5716/t028/include/t028.h"
#include "../include/HardwareConnectorT028.h"

DeviceT028::DeviceT028(const QString& nameId, const ViSession* masterViSession, QObject* parent) noexcept : AbstractDevice(nameId, new HardwareConnectorT028(masterViSession), parent), ChannelsIF(16) {}
