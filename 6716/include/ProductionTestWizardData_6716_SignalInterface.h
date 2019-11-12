#pragma once
#include <QObject>
#include <QString>
#include "ProductionTestWizardData_6716.h"

class ProductionTestWizardData_6716_SignalInterface : public QObject {
	Q_OBJECT
public:
	ProductionTestWizardData_6716_SignalInterface(const std::unique_ptr<ProductionTestWizardData_6716>& dataObject) {
		connect(dataObject.get(), &ProductionTestWizardData_6716::driverRevision, this, &ProductionTestWizardData_6716_SignalInterface::driverRevision);
		connect(dataObject.get(), &ProductionTestWizardData_6716::firmwareRevision, this, &ProductionTestWizardData_6716_SignalInterface::firmwareRevision);
		connect(dataObject.get(), &ProductionTestWizardData_6716::logMsg, this, &ProductionTestWizardData_6716_SignalInterface::logMsg);
		connect(dataObject.get(), &ProductionTestWizardData_6716::serialNumber, this, &ProductionTestWizardData_6716_SignalInterface::serialNumber);
		connect(dataObject.get(), &ProductionTestWizardData_6716::snT028, this, &ProductionTestWizardData_6716_SignalInterface::snT028);
		connect(dataObject.get(), &ProductionTestWizardData_6716::sn3416_6716, this, &ProductionTestWizardData_6716_SignalInterface::sn3416_6716);
		connect(dataObject.get(), &ProductionTestWizardData_6716::sn3416_T028, this, &ProductionTestWizardData_6716_SignalInterface::sn3416_T028);
		connect(dataObject.get(), &ProductionTestWizardData_6716::subtype, this, &ProductionTestWizardData_6716_SignalInterface::subtype);
		connect(dataObject.get(), &ProductionTestWizardData_6716::testsDone, this, &ProductionTestWizardData_6716_SignalInterface::testsDone);
		connect(dataObject.get(), &ProductionTestWizardData_6716::versionT028, this, &ProductionTestWizardData_6716_SignalInterface::versionT028);
		connect(dataObject.get(), &ProductionTestWizardData_6716::version3416_6716, this, &ProductionTestWizardData_6716_SignalInterface::version3416_6716);
		connect(dataObject.get(), &ProductionTestWizardData_6716::version3416_T028, this, &ProductionTestWizardData_6716_SignalInterface::version3416_T028);
		connect(dataObject.get(), &ProductionTestWizardData_6716::connectionStatus, this, &ProductionTestWizardData_6716_SignalInterface::connectionStatus);


		connect(this, &ProductionTestWizardData_6716_SignalInterface::connectDevices, dataObject.get(), &ProductionTestWizardData_6716::connectDevices);
		connect(this, &ProductionTestWizardData_6716_SignalInterface::disconnectDevices, dataObject.get(), &ProductionTestWizardData_6716::disconnectDevices);
		connect(this, &ProductionTestWizardData_6716_SignalInterface::setChannelMask, dataObject.get(), &ProductionTestWizardData_6716::setChannelMask);
		connect(this, &ProductionTestWizardData_6716_SignalInterface::requestTestEquipmentData, dataObject.get(), &ProductionTestWizardData_6716::sendTestEquipmentData);
		connect(this, &ProductionTestWizardData_6716_SignalInterface::requestUnitUnderTestData, dataObject.get(), &ProductionTestWizardData_6716::sendUnitUnderTestData);
		connect(this, &ProductionTestWizardData_6716_SignalInterface::setActiveUser, dataObject.get(), &ProductionTestWizardData_6716::setActiveUser);
		connect(this, &ProductionTestWizardData_6716_SignalInterface::setChannelMask, dataObject.get(), &ProductionTestWizardData_6716::setChannelMask);
		connect(this, &ProductionTestWizardData_6716_SignalInterface::setFirmwareRevision, dataObject.get(), &ProductionTestWizardData_6716::setFirmwareRevision);
		connect(this, &ProductionTestWizardData_6716_SignalInterface::setMdaTestPassed, dataObject.get(), &ProductionTestWizardData_6716::setMdaTestPassed);
		connect(this, &ProductionTestWizardData_6716_SignalInterface::setSerialNumber, dataObject.get(), &ProductionTestWizardData_6716::setSerialNumber);
		connect(this, &ProductionTestWizardData_6716_SignalInterface::setStoreCalibrationDataToEeprom, dataObject.get(), &ProductionTestWizardData_6716::setShouldStoreCalibrationDataToEeprom);
		connect(this, &ProductionTestWizardData_6716_SignalInterface::setSubtype, dataObject.get(), &ProductionTestWizardData_6716::setSubtype);
		connect(this, &ProductionTestWizardData_6716_SignalInterface::startPreTests, dataObject.get(), &ProductionTestWizardData_6716::startPreTests);
		connect(this, &ProductionTestWizardData_6716_SignalInterface::startTests, dataObject.get(), &ProductionTestWizardData_6716::startTests);
		connect(this, &ProductionTestWizardData_6716_SignalInterface::stopTests, dataObject.get(), &ProductionTestWizardData_6716::stopTests);
		connect(this, &ProductionTestWizardData_6716_SignalInterface::requestConnectionStatus, dataObject.get(), &ProductionTestWizardData_6716::checkConnectionStatus);
	}

	//void initialize() override;

//For qml gui interface
signals:
	//out
	void driverRevision(const QString& value) const;
	void firmwareRevision(const QString& value) const;
	void logMsg(const QString& msg) const;
	void serialNumber(const QString& value) const;
	void snT028(const QString& value) const;
	void sn3416_6716(const QString& value) const;
	void sn3416_T028(const QString& value) const;
	void subtype(const QString& value) const;
	void testsDone() const;
	void versionT028(const QString& value) const;
	void version3416_6716(const QString& value) const;
	void version3416_T028(const QString& value) const;
	void connectionStatus(const bool status) const;

	//in
	void connectDevices(const QString& ip6100, const QString& ip6716, const QString& fc3416_6716, const QString& fc3416_t028) const;
	void disconnectDevices() const;
	void requestConnectionStatus() const;
	void requestTestEquipmentData() const;
	void requestUnitUnderTestData() const;
	void setActiveUser(const QString& index) const;
	void setChannelMask(const QString& channelMask) const;
	void setFirmwareRevision(const QString& str) const;
	void setMdaTestPassed(const bool value) const;
	void setSerialNumber(const QString& str) const;
	void setStoreCalibrationDataToEeprom(const bool value) const;
	void setSubtype(const QString& str) const;
	void startPreTests() const;
	void startTests() const;
	void stopTests() const;
};