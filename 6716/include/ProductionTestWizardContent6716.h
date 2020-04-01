#pragma once
#include "../../Common/include/AbstractProductionTestWizardContent.h"
#include "../../Common/include/Device6716.h"
#include "../../Common/include/Device6100.h"
#include "../../Common/include/Device3416.h"
#include "../../Common/include/DeviceT028.h"

class ProductionTestWizardContent6716 : public AbstractProductionTestWizardContent {
	Q_OBJECT
	void initializeTests() override;
	void createAddressIdentificationPageContent() override;
	void createUutIdentificationPageContent() override;
	void createTestEquipmentIdentificationPageContent() override;
	QString version = "1.1";
public:
	ProductionTestWizardContent6716(QObject* parent = nullptr);
	~ProductionTestWizardContent6716() override = default;
	int channelsCount() const noexcept override;
};