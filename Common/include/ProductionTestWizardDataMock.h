#pragma once
#include "ProductionTestWizardData.h"
#include "MockTest.h"

class ProductionTestWizardDataMock : public ProductionTestWizardData {
    Q_OBJECT
protected:
	void saveSubtype(const QString& str) const override {}
	void saveSerialNumber(const QString& str) const override {}
	void saveFirmwareRevision(const QString& str) const override {}
	void loadSubtype() override {}
	void loadSerialNumber() override {}
	void loadFirmwareRevision() override  {}
public:
	void initialize()  {}
public:
	ProductionTestWizardDataMock() {
		subtype = "TEST";
		serialNumber = "12345678";
		driverRevision = "0.69";
		firmwareRevision = "0.99";

		addPreTest(std::make_unique<MockTest>(true));
		addPreTest(std::make_unique<MockTest>(true));
		addPreTest(std::make_unique<MockTest>(true));
		addPreTest(std::make_unique<MockTest>(true));

		addTest(std::make_unique<MockTest>(true));
		addTest(std::make_unique<MockTest>(true));
		addTest(std::make_unique<MockTest>(false));
		addTest(std::make_unique<MockTest>(true));
		addTest(std::make_unique<MockTest>(false));
	}
	~ProductionTestWizardDataMock() {}
};
