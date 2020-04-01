#pragma once
#include "../../Common/include/AbstractProductionTestWizardContent.h"

class ProductionTestWizardContentMock : public AbstractProductionTestWizardContent {
	Q_OBJECT
	void initializeTests() override;
	void createAddressIdentificationPageContent() override;
	void createUutIdentificationPageContent() override;
	void createTestEquipmentIdentificationPageContent() override;
public:
	ProductionTestWizardContentMock(QObject* parent = nullptr);
	~ProductionTestWizardContentMock() override = default;
	int channelsCount() const noexcept override;
};
