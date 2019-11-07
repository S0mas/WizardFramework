#pragma once
#include <QStringListModel>
#include <QThread>

#include "TestListModel.h"
#include "DataBasseConnector.h"

class ProductionTestWizardData : public QObject {
    Q_OBJECT
	QThread testingThread;
	QThread preTestingThread;
	QThread printerThread;
	std::unique_ptr<TestListModel> testsListModel_ = nullptr;
	std::unique_ptr<TestListModel> preTestsListModel_ = nullptr;
	std::unique_ptr<QStringListModel> usersNamesModel_ = nullptr;
	std::unique_ptr<PrintInterface> printer = nullptr;
	void loadUsersFromDataBase() noexcept;
protected:
	QThread dataThread;
	QString subtype_ = "error while reading data from device";
	QString serialNumber_ = "error while reading data from device";
	QString driverRevision_ = "empty";
	QString firmwareRevision_ = "error while reading data from device";
	int activeUser_ = -1;
	bool mdaTestPassed_ = true;
	std::unique_ptr<DataBaseConnectorInterface> dbConnection;

	virtual void saveSubtype(const QString& str) const = 0;
	virtual void saveSerialNumber(const QString& str) const = 0;
	virtual void saveFirmwareRevision(const QString& str) const = 0;
	virtual void loadSubtype() = 0;
	virtual void loadSerialNumber() = 0;
	virtual void loadFirmwareRevision() = 0;
public:
	virtual void initialize() = 0;
protected:
	void addTest(std::unique_ptr<AbstractTest>&& test);
	void addPreTest(std::unique_ptr<AbstractTest>&& test);
	PrintInterface* getPrinter() const noexcept {
		return printer.get();
	}
public:
	ProductionTestWizardData();
	virtual ~ProductionTestWizardData();
	TestListModel* testsModel() const noexcept;
	TestListModel* preTestsModel() const noexcept;
	QStringListModel* userListModel() const noexcept;
public slots:
	void setActiveUser(const int index);
	void setFirmwareRevision(const QString& str);
	void setMdaTestPassed(const bool value);
	void setSerialNumber(const QString& str);
	void setShouldStoreCalibrationDataToEeprom(const bool value);
	void setSubtype(const QString& str);
signals:
	void driverRevision(const QString& value) const;
	void firmwareRevision(const QString& value) const;
	bool shouldStoreCalibrationDataToEeprom(const bool value) const;
	void logMsg(const QString& msg) const;
	void serialNumber(const QString& value) const;
	void startPreTests() const;
	void startTests() const;
	void stopTests() const;
	void subtype(const QString& value) const;
	void testsDone() const;
};
