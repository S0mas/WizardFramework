#pragma once
#include <QString>
#include <QObject>
#include <cstdio>
#include "PrintInterface.h"

class AbstractTest : public QObject {
	Q_OBJECT
	const QString name;
	mutable bool result = false;
	bool shouldBeRun = true;
	mutable bool wasRunned = false;
	static inline bool storeCalibrationDataToEeprom = true;
protected:
	static inline std::atomic<bool> continueTestClicked = { false };
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
signals:
	void shouldBeRunChanged(bool) const;
	void log(QString) const;
	void askUserAction(const QString& str) const;
public slots:
	void continueTest() {
		continueTestClicked = true;
	}
};




