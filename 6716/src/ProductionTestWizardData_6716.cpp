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

void ProductionTestWizardData_6716::loadIDN() {
	connection->sendCmd("*IDN?\0");
	auto const& list = connection->readResponse().split(',', QString::SplitBehavior::SkipEmptyParts);
	if (list.size() == 4) {
		serialNumber = list[2];
		subtype = list[1].right(2);
		firmwareRevision = list[3];
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

	addTest(std::make_unique<ExcitationCalibrationTest>(connection));
	addTest(std::make_unique<SignalPathCalibrationTest>(connection));
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
}

void ProductionTestWizardData_6716::loadTestEqData() {
	ViInt32 serial = 0xFFFFFFFF;
	connection->callAndThrowOnError3416(bu3416_getSerNum, "bu3416_getSerNum", &serial);
	sn3416_6716 = QString::number(serial);

	std::string driverRev;
	driverRev.resize(256);
	std::string firmwareRev;
	firmwareRev.resize(256);

	connection->callAndThrowOnError3416(bu3416_revision_query, "bu3416_getSerNum", driverRev.data(), firmwareRev.data());
	version3416_6716 = QString("%1 %2").arg(driverRev.data()).arg(firmwareRev.data());

	serial = 0xFFFFFFFF;
	connection->callAndThrowOnErrorMaster(bu3416_getSerNum, "bu3416_getSerNum", &serial);
	sn3416_T028 = QString::number(serial);

	connection->callAndThrowOnErrorMaster(bu3416_revision_query, "bu3416_getSerNum", driverRev.data(), firmwareRev.data());
	version3416_T028 = QString("%1 %2").arg(driverRev.data()).arg(firmwareRev.data());
}

ProductionTestWizardData_6716::ProductionTestWizardData_6716() {
	driverRevision = DRIVER_BU6716_REVISION_STR;
}

void ProductionTestWizardData_6716::initialize() {
	connection = std::make_shared<Communication_6716>();
	connect(connection.get(), &Communication_6716::log, getPrinter(), &PrintInterface::addLog);
	connection->initializePrimitive();
	connection->initialize();
	loadIDN();
	testsInit();
	loadTestEqData();
}

QString ProductionTestWizardData_6716::getSnT028() const noexcept {
	return snT028;
}

QString ProductionTestWizardData_6716::getSn3416_6716() const noexcept {
	return sn3416_6716;
}

QString ProductionTestWizardData_6716::getSn3416_T028() const noexcept {
	return sn3416_T028;
}

QString ProductionTestWizardData_6716::getVersionT028() const noexcept {
	return versionT028;
}

QString ProductionTestWizardData_6716::getVersion3416_6716() const noexcept {
	return version3416_6716;
}

QString ProductionTestWizardData_6716::getVersion3416_T028() const noexcept {
	return version3416_T028;
}