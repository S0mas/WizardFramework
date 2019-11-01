#pragma once
#include <QString>
#include <QObject>
#include <cstdio>
#include "PrintInterface.h"

class AbstractTest : public QObject {
	Q_OBJECT
	Q_PROPERTY(bool shouldBeRun READ getShouldBeRun WRITE setShouldBeRun NOTIFY shouldBeRunChanged)
	QString name;
	mutable bool result = false;
	bool m_shouldBeRun = true;
	mutable bool wasRunned = false;
	PrintInterface* printer;
protected:
	virtual bool test() const = 0;
	virtual void preTestSetUp() const {}
	virtual void postTestCleanUp() const {}
public:
	AbstractTest(const QString& name);
	QString getName() const noexcept;
	void run() const;
	bool getShouldBeRun() const noexcept;
	bool getResult() const noexcept;
	bool getRunned() const noexcept;
	void setShouldBeRun(const bool value) noexcept;

	template<typename ...Args>
	void printf(Args&& ...args) const noexcept {
		if(printer)
			printer->printf(std::forward<Args>(args)...);
	}

	void setPrinter(PrintInterface* newPrinter) noexcept {
		printer = newPrinter;
	}

signals:
	void shouldBeRunChanged(bool);
};




