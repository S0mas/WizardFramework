#include "..\include\AbstractProductionTestWizardContent.h"

void AbstractProductionTestWizardContent::runObjectOnSeparetedThread(QObject& obj) {
	threads_.push_back(new QThread(this));
	obj.moveToThread(threads_.back());
	threads_.back()->start();
}

AbstractProductionTestWizardContent::AbstractProductionTestWizardContent(QObject* parent) : QObject(parent) {}

AbstractProductionTestWizardContent::~AbstractProductionTestWizardContent() {
	for (auto& thread : threads_) {
		thread->requestInterruption();
		thread->quit();
		thread->wait();
	}
}

const std::vector<AbstractTest*>& AbstractProductionTestWizardContent::mainTests() noexcept {
	return mainTestsRunner_.tests();
}

void AbstractProductionTestWizardContent::initialize() {
	initializeTests();
	createAddressIdentificationPageContent();
	createUutIdentificationPageContent();
	createTestEquipmentIdentificationPageContent();

	for (auto const device : devicesManager_.devices())
		device->setSender(userActionRequestController_.createSender());
	devicesManagerController_.connectDevicesManager(&devicesManager_);
	runObjectOnSeparetedThread(devicesManager_);

	for (auto const test : preTestsRunner_.tests())
		test->setSender(userActionRequestController_.createSender());
	preTestsRunnerController_.connectTestsRunner(&preTestsRunner_);
	runObjectOnSeparetedThread(preTestsRunner_);

	for (auto const test : mainTestsRunner_.tests())
		test->setSender(userActionRequestController_.createSender());
	mainTestsRunnerController_.connectTestsRunner(&mainTestsRunner_);
	runObjectOnSeparetedThread(mainTestsRunner_);
}
