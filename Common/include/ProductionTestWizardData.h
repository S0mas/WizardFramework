#pragma once
#include <QStringListModel>
#include <QThread>

#include "TestListModel.h"
#include "DataBasseConnector.h"

class ProductionTestWizardData : public QObject {
    Q_OBJECT
    int activeUser = -1;
    bool mdaTestPassed = true;
	QThread testingThread;
	QThread preTestingThread;
	std::unique_ptr<TestListModel> testsListModel = nullptr;
	std::unique_ptr<TestListModel> preTestsListModel = nullptr;
	std::unique_ptr<QStringListModel> usersNamesModel = nullptr;
	std::unique_ptr<PrintInterface> printer = nullptr;
	void loadUsersFromDataBase() noexcept;
protected:
	QString subtype = "error while reading data from device";
	QString serialNumber = "error while reading data from device";
	QString driverRevision = "empty";
	QString firmwareRevision = "error while reading data from device";
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
	~ProductionTestWizardData();
	Q_INVOKABLE int getActiveUser() const noexcept;
	Q_INVOKABLE QString getSubtype() const noexcept;
	Q_INVOKABLE QString getSerialNumber() const noexcept;
	Q_INVOKABLE QString getDriverRevision() const noexcept;
	Q_INVOKABLE QString getFirmwareRevision() const noexcept;
	Q_INVOKABLE bool getMdaTestPassed() const noexcept;
	Q_INVOKABLE void setActiveUser(const int index) noexcept;
	Q_INVOKABLE void setSubtype(const QString& str) noexcept;
	Q_INVOKABLE void setSerialNumber(const QString& str) noexcept;
	Q_INVOKABLE void setFirmwareRevision(const QString& str) noexcept;
	Q_INVOKABLE void setMdaTestPassed(const bool value) noexcept;
	Q_INVOKABLE TestListModel* getTestsModel() const noexcept;
	Q_INVOKABLE TestListModel* getPreTestsModel() const noexcept;
	Q_INVOKABLE QStringListModel* getUserListModel() const noexcept;
signals:
	void logMsg(const QString& msg) const;
	void startPreTests() const;
	void startTests() const;
	void stopTests() const;
	void testsDone() const;
};
