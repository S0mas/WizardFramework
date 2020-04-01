#include "../include/ProductionTestWizardContentMock.h"
#include "../include/MockTest.h"
#include "../include/DeviceMock.h"

void ProductionTestWizardContentMock::initializeTests() {
	preTestsRunner_.addTest(new MockTest(true));
	preTestsRunner_.addTest(new MockTest(true));

	mainTestsRunner_.addTest(new MockTestWithErrorHandlingSelection(false));
	mainTestsRunner_.addTest(new MockTestWithConfirmationAction(true));
	mainTestsRunner_.addTest(new MockTestWithDecisionAction(true));
	mainTestsRunner_.addTest(new MockTestWithChannelsErrorSelection(true));
}

void ProductionTestWizardContentMock::createAddressIdentificationPageContent() {
	devicesManager_.addDevice(new DeviceMock("Mock1"));
	devicesManager_.addDevice(new DeviceMock("Mock2"));
	MockTest::setDevices(devicesManager_.getDevice("Mock1"), devicesManager_.getDevice("Mock2"));
	addressesIdentificationPageContent.addResources(devicesManager_.devices());
}

void ProductionTestWizardContentMock::createUutIdentificationPageContent() {
	std::vector<std::shared_ptr<AbstractDataResource>> resources;
	auto deviceResourcesMock = dynamic_cast<DeviceIdentityResourcesIF*>(devicesManager_.getDevice("Mock1"))->allIdentityResources(false);
	resources.insert(resources.end(), deviceResourcesMock.begin(), deviceResourcesMock.end());
	uutIdentificationPageContent.addResources(resources);
}

void ProductionTestWizardContentMock::createTestEquipmentIdentificationPageContent() {
	std::vector<std::shared_ptr<AbstractDataResource>> resources;

	auto deviceResourcesMock2 = dynamic_cast<DeviceIdentityResourcesIF*>(devicesManager_.getDevice("Mock2"))->allIdentityResources();
	resources.insert(resources.end(), deviceResourcesMock2.begin(), deviceResourcesMock2.end());

	resources.push_back(std::make_shared<ConstantResource>("ProductionTestsMockVersion", "Mock Production tests version", Enums::GUI_Type::TYPE_1, "1.0"));
	testEquipmentIdentificationPageContent.addResources(resources);
}

ProductionTestWizardContentMock::ProductionTestWizardContentMock(QObject* parent) : AbstractProductionTestWizardContent(parent) {
	AbstractProductionTestWizardContent::initialize();
	QStringList devicesNamesIds;
	for (auto const& device : devicesManager_.devices())
		devicesNamesIds << device->nameId();
	devicesManagerController_.registerDevices(devicesNamesIds);
}

int ProductionTestWizardContentMock::channelsCount() const noexcept {
	return 32;
}
