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
signals:
	void shouldBeRunChanged(bool) const;
	void log(QString) const;
};




