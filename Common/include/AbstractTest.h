#pragma once
#include <QString>
#include <QObject>
#include <cstdio>
#include "PrintInterface.h"
#include "UserActionTypes.h"

class AbstractTest : public QObject {
	Q_OBJECT
	const QString name;
	mutable bool result = false;
	mutable bool wasRunned = false;
	bool shouldBeRun = true;
	static inline bool storeCalibrationDataToEeprom = true;
protected:
	mutable std::atomic<bool> continueTestClicked = { false };
	mutable bool problemReportedByUser = false;
	virtual bool test() const = 0;
	virtual void preTestSetUp() const {}
	virtual void postTestCleanUp() const {}
public:
	AbstractTest(const QString& name);
	const QString& getName() const noexcept;
	void run() const;
	bool getShouldBeRun() const noexcept;
	bool getResult() const noexcept;
	bool getRunned() const noexcept;
	void setShouldBeRun(const bool value) noexcept;
	static void setStoreCalibrationDataToEeprom(const bool) noexcept;
	static bool getStoreCalibrationDataToEeprom() noexcept;
	void waitForUserAction(const QString& msg, const UserActionType actionType) const noexcept;
signals:
	void shouldBeRunChanged(bool) const;
	void log(QString) const;
	void askUserAction(const QString& msg, const int actionType) const;
public slots:
	void continueTest();
	void reportProblem();
};




