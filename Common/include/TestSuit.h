#pragma once
#include <QCoreApplication>
#include <QObject>
#include "AbstractTest.h"
#include <memory>

class TestSuit : public QObject {
	Q_OBJECT
	bool stopped = false;
	std::vector<std::unique_ptr<AbstractTest>> testsList;
public:
	const std::vector<std::unique_ptr<AbstractTest>>& getTestsList() const noexcept;
	std::vector<std::unique_ptr<AbstractTest>>& getTestsList() noexcept;
public slots:
	void runTests() const;
	void stopTests();
signals:
	void testsDone() const;
	void testRunned(QString name) const;
	void testFinished(QString name, bool result) const;
};