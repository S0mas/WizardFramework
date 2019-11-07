#pragma once
#include "../../Common/include/ProductionTestWizardData.h"
#define INSTR_LANGUAGE_SPECIFIC
#include <bu6716.h>
#include <bu3416.h>
#include <t028.h>
#include <bu6100.h>
#include "Communication_6716.h"
#include <memory>

class ProductionTestWizardData_6716 : public ProductionTestWizardData {
	Q_OBJECT
	std::shared_ptr<Communication_6716> connection;
	QString snT028_ = "no diriver method available";
	QString sn3416_6716_ = "error while reading data from device";
	QString sn3416_T028_ = "error while reading data from device";
	QString versionT028_ = "no diriver method available";
	QString version3416_6716_ = "error while reading data from device";
	QString version3416_T028_ = "error while reading data from device";
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
	virtual ~ProductionTestWizardData_6716() {
		disconnectDevices();
	}
	void initialize() override;

signals:
	void driverRevision(const QString& value) const;
	void firmwareRevision(const QString& value) const;
	void snT028(const QString& value) const;
	void sn3416_6716(const QString& value) const;
	void sn3416_T028(const QString& value) const;
	void versionT028(const QString& value) const;
	void version3416_6716(const QString& value) const;
	void version3416_T028(const QString& value) const;
public slots:
	void connectDevices(const QString& ip6100, const QString& ip6716, const QString& fc3416_6716, const QString& fc3416_t028);
	void disconnectDevices() const;
	void setChannelMask(const QString& channelMask) const;
	void sendTestEquipmentData() const;
	void sendUnitUnderTestData() const;
};