#include "../include/Device6100.h"
#include "../../../bu6100/include/bu6100.h"
#include "../../../visa/include/visa.h"
#include "../include/HardwareConnector6100.h"
#include "../include/ScpiIFVisa.h"

Device6100::Device6100(const QString& nameId, QObject* parent) noexcept : ScpiDevice(nameId, new HardwareConnector6100(nameId), new ScpiIFVisa, parent), DeviceIdentityResourcesIF(nameId) {}

void Device6100::saveSubtype(const QString& str) const {
	//todo
}

void Device6100::saveSerialnumber(const QString& str) const {
	//todo
}

void Device6100::saveFirmwareRevision(const QString& str) const {
	//todo
}

void Device6100::saveDriverRevision(const QString& str) const {
	//todo
}

QString Device6100::loadSubtype() const {
	//char subtype[2];
	//invokeFunction(bu3416_getSubType, "bu3416_getSubType", subtype);
	//return subtype;
	return "todo";
}

QString Device6100::loadSerialnumber() const {
	//ViInt32 serialnumber = 0xFFFFFFFF;
	//invokeFunction(bu3416_getSerNum, "bu3416_getSerNum", &serialnumber);
	//return QString::number(serialnumber, 16);
	return "todo";
}

QString Device6100::loadFirmwareRevision() const {
	//char firmwareRev[256];
	//invokeFunction(bu3416_revision_query, "bu3416_revision_query", nullptr, firmwareRev);
	//return firmwareRev;
	return "todo";
}

QString Device6100::loadDriverRevision() const {
	//char driverRev[256];
	//invokeFunction(bu3416_revision_query, "bu3416_revision_query", driverRev, nullptr);
	//return driverRev;
	return "todo";
}
