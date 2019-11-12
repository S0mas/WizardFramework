#include "../include/ProductionTestWizardData_6716.h"
#include "../include/Tests/ExcitationCalibrationTest.h"
#include "../include/Tests/ACCouplingTest.h"
#include "../include/Tests/AutobalanceDACTest.h"
#include "../include/Tests/ChannelGainTest.h"
#include "../include/Tests/HalfBridgeTest.h"
#include "../include/Tests/I2CAccessTo1WireControllersTest.h"
#include "../include/Tests/I2CAccessToChannelConfigurationControlPortExpandersTest.h"
#include "../include/Tests/I2CAccessToFPGATest.h"
#include "../include/Tests/I2CSegmentsSwitchingAndTemperatureSensorAccessTest.h"
#include "../include/Tests/IEPETest.h"
#include "../include/Tests/InitialExcitationTest.h"
#include "../include/Tests/LEDsTest.h"
#include "../include/Tests/NegativeExcitationCurrentLimitTest.h"
#include "../include/Tests/PositiveExcitationCurrentLimitTest.h"
#include "../include/Tests/QuarterBridgeTest.h"
#include "../include/Tests/SCUErrorTest.h"
#include "../include/Tests/ShuntTest.h"
#include "../include/Tests/SignalPathCalibrationTest.h"
#include "../include/Tests/SignalPathTest.h"
#include "../include/Tests/TedsClass1Test.h"
#include "../include/Tests/TedsClass2Test.h"
#include "../include/Tests/HalfBridgeTest.h"
#include <QDebug>
void ProductionTestWizardData_6716::loadIDN() {
	connection->sendCmd("*IDN?\0");
	auto const& list = connection->readResponse().split(',', QString::SplitBehavior::SkipEmptyParts);
	if (list.size() == 4) {
		serialNumber_ = list[2];
		subtype_ = list[1].right(2);
		firmwareRevision_ = list[3];
	}
}

void ProductionTestWizardData_6716::saveSubtype(const QString& str) const {
	connection->sendCmd("BINT:MAIN:EEPROM:SUBTYPE '%s'\0", str.toStdString().c_str());
}

void ProductionTestWizardData_6716::saveSerialNumber(const QString& str) const {
	connection->sendCmd("BINT:MAIN:EEPROM:SERIAL #h%s\0", str.toStdString().c_str());
}

void ProductionTestWizardData_6716::saveFirmwareRevision(const QString& str) const {
	connection->sendCmd("BINT:MAIN:EEPROM:PREVISION #h%s\0", str.toStdString().c_str());
}

void ProductionTestWizardData_6716::loadSubtype() {
	loadIDN();
}

void ProductionTestWizardData_6716::loadSerialNumber() {
	loadIDN();
}

void ProductionTestWizardData_6716::loadFirmwareRevision() {
	loadIDN();
}

void ProductionTestWizardData_6716::testsInit() {
	addPreTest(std::make_unique<I2CAccessToFPGATest>(connection));
	addPreTest(std::make_unique<I2CSegmentsSwitchingAndTemperatureSensorAccessTest>(connection));
	addPreTest(std::make_unique<I2CAccessToChannelConfigurationControlPortExpandersTest>(connection));
	addPreTest(std::make_unique<I2CAccessTo1WireControllersTest>(connection));

	addTest(std::make_unique<TedsClass1Test>(connection));
	addTest(std::make_unique<TedsClass2Test>(connection));
	addTest(std::make_unique<LEDsTest>(connection));
	addTest(std::make_unique<SCUErrorTest>(connection));


	addTest(std::make_unique<InitialExcitationTest>(connection));
	addTest(std::make_unique<HalfBridgeTest>(connection));
	addTest(std::make_unique<QuarterBridgeTest>(connection));
	addTest(std::make_unique<PositiveExcitationCurrentLimitTest>(connection));
	addTest(std::make_unique<NegativeExcitationCurrentLimitTest>(connection));
	addTest(std::make_unique<AutobalanceDACTest>(connection));
	addTest(std::make_unique<ShuntTest>(connection));
	addTest(std::make_unique<SignalPathTest>(connection));
	addTest(std::make_unique<ChannelGainTest>(connection));
	addTest(std::make_unique<ACCouplingTest>(connection));
	addTest(std::make_unique<IEPETest>(connection));

	addTest(std::make_unique<ExcitationCalibrationTest>(connection));
	addTest(std::make_unique<SignalPathCalibrationTest>(connection));
}

void ProductionTestWizardData_6716::loadTestEqData() {
	ViInt32 serial = 0xFFFFFFFF;
	connection->callAndThrowOnError3416(bu3416_getSerNum, "bu3416_getSerNum", &serial);
	sn3416_6716_ = QString::number(serial);

	std::string driverRev;
	driverRev.resize(256);
	std::string firmwareRev;
	firmwareRev.resize(256);

	connection->callAndThrowOnError3416(bu3416_revision_query, "bu3416_getSerNum", driverRev.data(), firmwareRev.data());
	version3416_6716_ = QString("%1 %2").arg(driverRev.data()).arg(firmwareRev.data());

	serial = 0xFFFFFFFF;
	connection->callAndThrowOnErrorMaster(bu3416_getSerNum, "bu3416_getSerNum", &serial);
	sn3416_T028_ = QString::number(serial);

	connection->callAndThrowOnErrorMaster(bu3416_revision_query, "bu3416_getSerNum", driverRev.data(), firmwareRev.data());
	version3416_T028_ = QString("%1 %2").arg(driverRev.data()).arg(firmwareRev.data());
}

ProductionTestWizardData_6716::ProductionTestWizardData_6716() {
	driverRevision_ = "test=TODO";//DRIVER_BU6716_REVISION_STR;
	this->moveToThread(&dataThread);
	dataThread.start();
}

void ProductionTestWizardData_6716::initialize() {
	connection = std::make_shared<Communication_6716>();
	connect(connection.get(), &Communication_6716::log, getPrinter(), &PrintInterface::addLog);
	testsInit();
}

void ProductionTestWizardData_6716::setChannelMask(const QString& channelMask) const {
	Abstract6716Test::CHANNEL_MASK = channelMask.toInt(nullptr, 2);
}

void ProductionTestWizardData_6716::sendTestEquipmentData() const {
	emit snT028(snT028_);
	emit sn3416_T028(sn3416_T028_);
	emit sn3416_6716(sn3416_6716_);
	emit versionT028(versionT028_);
	emit version3416_6716(version3416_6716_);
	emit version3416_T028(version3416_T028_);
}

void ProductionTestWizardData_6716::sendUnitUnderTestData() const {
	emit driverRevision(driverRevision_);
	emit firmwareRevision(firmwareRevision_);
	emit serialNumber(serialNumber_);
	emit subtype(subtype_);
}

void ProductionTestWizardData_6716::checkConnectionStatus() {
	qDebug() << "checking started";
	ViUInt16 status = 0;
	try {
		std::string str = "";
		connection->callAndThrowOnError6100(viPrintf, "viPrintf", str.data());
		connection->callAndThrowOnError6716(viPrintf, "viPrintf", str.data());
		connection->callAndThrowOnError3416(viPrintf, "viPrintf", str.data());
		connection->callAndThrowOnErrorMaster(viPrintf, "viPrintf", str.data());
		connection->callAndThrowOnErrorT028(viPrintf, "viPrintf", str.data());
		emit connectionStatus(true);
	}
	catch (...) {
		emit connectionStatus(false);
	}
}

void ProductionTestWizardData_6716::connectDevices(const QString& ip6100, const QString& ip6716, const QString& fc3416_6716, const QString& fc3416_t028) {
	try {
		connection->initializePrimitive(QString("TCPIP0::%1::5025::SOCKET").arg(ip6716));
		connection->initialize(QString("TCPIP0::%1::INSTR").arg(ip6100), QString("TCPIP0::%1::5025::SOCKET").arg(ip6716), fc3416_6716.toInt(), fc3416_t028.toInt());		
		emit connectionStatus(connection->getVi6716() && connection->getVi6100() && connection->getVi3416() && connection->getViT028Master() && connection->getViT028());
		loadIDN();
		loadTestEqData();		
	}
	catch (...) {}
}

void ProductionTestWizardData_6716::disconnectDevices() const {
	//if (connection->getPrimitiveVi6716())
	//	viClose(connection->getPrimitiveVi6716());
	//if (connection->getPrimitiveRmVi6716())
	//	viClose(connection->getPrimitiveRmVi6716());
	if (connection->getVi6716())
		bu6716_close(connection->getVi6716());
	if (connection->getVi3416())
		bu3416_close(connection->getVi3416());
	if (connection->getViT028())
		t028_close(connection->getViT028());
	if (connection->getViT028Master())
		bu3416_close(connection->getViT028Master());
	if (connection->getVi6100())
		bu6100_close(connection->getVi6100());
	connection->setAllVisToInvalid();
	emit connectionStatus(false);
}
