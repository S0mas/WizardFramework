#include "../include/ProductionTestWizardContent6716.h"
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
#include "../../Common/include/HardwareConnector6716.h"
#include "../../Common/include/HardwareConnector6100.h"
#include "../../Common/include/HardwareConnector3416.h"
#include "../../Common/include/HardwareConnectorT028.h"
#include "../../Common/include/AppicationSettings.h"


void ProductionTestWizardContent6716::initializeTests() {
	preTestsRunner_.addTest(new I2CAccessToFPGATest());
	preTestsRunner_.addTest(new I2CSegmentsSwitchingAndTemperatureSensorAccessTest());
	preTestsRunner_.addTest(new I2CAccessToChannelConfigurationControlPortExpandersTest());
	preTestsRunner_.addTest(new I2CAccessTo1WireControllersTest());

	//user action needed
	mainTestsRunner_.addTest(new LEDsTest());
	mainTestsRunner_.addTest(new TedsClass2Test());
	mainTestsRunner_.addTest(new TedsClass1Test());
	mainTestsRunner_.addTest(new ExcitationCalibrationTest());
	//////////////////////////////////////////////////////////

	mainTestsRunner_.addTest(new SCUErrorTest());
	mainTestsRunner_.addTest(new HalfBridgeTest());
	mainTestsRunner_.addTest(new QuarterBridgeTest());
	mainTestsRunner_.addTest(new PositiveExcitationCurrentLimitTest());
	mainTestsRunner_.addTest(new NegativeExcitationCurrentLimitTest());
	mainTestsRunner_.addTest(new AutobalanceDACTest());
	mainTestsRunner_.addTest(new ShuntTest());
	mainTestsRunner_.addTest(new SignalPathTest());
	mainTestsRunner_.addTest(new ChannelGainTest());
	mainTestsRunner_.addTest(new ACCouplingTest());
	mainTestsRunner_.addTest(new IEPETest());

	mainTestsRunner_.addTest(new SignalPathCalibrationTest());
}

void ProductionTestWizardContent6716::createAddressIdentificationPageContent() {
	devicesManager_.addDevice(new Device6716("6716"));
	devicesManager_.addDevice(new Device6100("6100"));
	devicesManager_.addDevice(new Device3416("3416 connected to 6716", "6100"));
	devicesManager_.addDevice(new Device3416("3416 connected to t028", "6100"));
	auto masterViRef = &dynamic_cast<Device3416*>(devicesManager_.getDevice("3416 connected to t028"))->refereceToViSession();
	devicesManager_.addDevice(new DeviceT028("t028", masterViRef));
	AbstractTest6716::setDevices(devicesManager_.getDevice("6716"), devicesManager_.getDevice("6100"), devicesManager_.getDevice("3416 connected to 6716"), devicesManager_.getDevice("3416 connected to t028"), devicesManager_.getDevice("t028"));

	addressesIdentificationPageContent.addResources(devicesManager_.devices());
}

void ProductionTestWizardContent6716::createUutIdentificationPageContent() {
	std::vector<std::shared_ptr<AbstractDataResource>> resources;
	auto deviceResources6716 = dynamic_cast<DeviceIdentityResourcesIF*>(devicesManager_.getDevice("6716"))->allIdentityResources(false);
	resources.insert(resources.end(), deviceResources6716.begin(), deviceResources6716.end());
	uutIdentificationPageContent.addResources(resources);
}

void ProductionTestWizardContent6716::createTestEquipmentIdentificationPageContent() {
	std::vector<std::shared_ptr<AbstractDataResource>> resources;
	resources.push_back(std::make_shared<ConstantResource>("Driver6716", "6716 driver revision", Enums::GUI_Type::TYPE_1, dynamic_cast<Device6716*>(devicesManager_.getDevice("6716"))->loadDriverRevision()));

	auto deviceResources3416_6716 = dynamic_cast<DeviceIdentityResourcesIF*>(devicesManager_.getDevice("3416 connected to 6716"))->allIdentityResources();
	resources.insert(resources.end(), deviceResources3416_6716.begin(), deviceResources3416_6716.end());

	auto deviceResources3416_T028 = dynamic_cast<DeviceIdentityResourcesIF*>(devicesManager_.getDevice("3416 connected to t028"))->allIdentityResources();
	resources.insert(resources.end(), deviceResources3416_T028.begin(), deviceResources3416_T028.end());

	resources.push_back(std::make_shared<ConstantResource>("ProductionTests6716Version", "6716 Production test version", Enums::GUI_Type::TYPE_1, version));
	testEquipmentIdentificationPageContent.addResources(resources);
}

ProductionTestWizardContent6716::ProductionTestWizardContent6716(QObject* parent) : AbstractProductionTestWizardContent(parent) {
	AbstractProductionTestWizardContent::initialize();
	QStringList devicesNamesIds;
	for (auto const& device : devicesManager_.devices())
		devicesNamesIds << device->nameId();
	devicesManagerController_.registerDevices(devicesNamesIds);
}

int ProductionTestWizardContent6716::channelsCount() const noexcept {
	return 16;
}
