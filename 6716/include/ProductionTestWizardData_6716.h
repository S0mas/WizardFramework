#pragma once
#include "../../Common/include/ProductionTestWizardData.h"
#include "bu6716_revision.h"
#include "Communication_6716.h"
#include <memory>

class ProductionTestWizardData_6716 : public ProductionTestWizardData {
	Q_OBJECT
	std::shared_ptr<Communication_6716> connection;
	QString snT028 = "no diriver method available";
	QString sn3416_6716 = "error while reading data from device";
	QString sn3416_T028 = "error while reading data from device";
	QString versionT028 = "no diriver method available";
	QString version3416_6716 = "error while reading data from device";
	QString version3416_T028 = "error while reading data from device";

	void loadIDN();
	void saveSubtype(const QString& str) const override;
	void saveSerialNumber(const QString& str) const override;
	void saveFirmwareRevision(const QString& str) const override;
	void loadSubtype() override;
	void loadSerialNumber() override;
	void loadFirmwareRevision() override;
	void testsInit();
	void loadTestEqData();
public:
	ProductionTestWizardData_6716();
	void initialize() override;

	Q_INVOKABLE QString getSnT028() const noexcept;
	Q_INVOKABLE QString getSn3416_6716() const noexcept;
	Q_INVOKABLE QString getSn3416_T028() const noexcept;
	Q_INVOKABLE QString getVersionT028() const noexcept;
	Q_INVOKABLE QString getVersion3416_6716() const noexcept;
	Q_INVOKABLE QString getVersion3416_T028() const noexcept;
};