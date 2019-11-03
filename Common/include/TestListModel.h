#pragma once
#include <QAbstractListModel>
#include <QGuiApplication>
#include <QDir>
#include <QPalette>
#include "TestSuit.h"

class TestListModel : public QAbstractListModel {
	Q_OBJECT
	TestSuit tests;
public:
	TestListModel(QObject* parent = nullptr);
	int rowCount(const QModelIndex& parent = QModelIndex()) const override;
	Q_INVOKABLE QString getTestName(const int index) const noexcept;
	Q_INVOKABLE bool getTestResult(const int index) const noexcept;
	Q_INVOKABLE bool getTestRunned(const int index) const noexcept;
	Q_INVOKABLE AbstractTest* getTest(const int index) const noexcept;
	Q_INVOKABLE bool checkIfAllRunnedTestsPassed() const noexcept;
	Q_INVOKABLE bool getTestShouldBeRun(const int index) const noexcept;
	Q_INVOKABLE void setTestShouldBeRun(const int index, const bool value) noexcept;
	Q_INVOKABLE void setAllTestsShouldBeRun(const bool value) noexcept;
	const std::vector<std::unique_ptr<AbstractTest>>& getTestsList() const noexcept;
	std::vector<std::unique_ptr<AbstractTest>>& getTestsList() noexcept;
	const TestSuit& getTestsSuit() const noexcept;
	TestSuit& getTestsSuit() noexcept;
	/*DUMMY*/QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;/*DUMMY*/
signals:
	void testRunned(QString name) const;
	void testFinished(QString name, bool result) const;
};