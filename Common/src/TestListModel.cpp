#include "..\include\TestListModel.h"

TestListModel::TestListModel(QObject* parent) : QAbstractListModel(parent) {
	connect(&tests, &TestSuit::testRunned, this, &TestListModel::testRunned);
}

int TestListModel::rowCount(const QModelIndex& parent) const {
	return parent.isValid() ? 0 : static_cast<int>(tests.getTestsList().size());
}

Q_INVOKABLE QString TestListModel::getTestName(const int index) const noexcept {
	return tests.getTestsList().at(index)->getName();
}

Q_INVOKABLE bool TestListModel::getTestResult(const int index) const noexcept {
	return tests.getTestsList().at(index)->getResult();
}

Q_INVOKABLE bool TestListModel::getTestRunned(const int index) const noexcept {
	return tests.getTestsList().at(index)->getRunned();
}

Q_INVOKABLE AbstractTest* TestListModel::getTest(const int index) const noexcept {
	return tests.getTestsList()[index].get();
}

Q_INVOKABLE bool TestListModel::checkIfAllRunnedTestsPassed() const noexcept {
	for (auto const& test : tests.getTestsList())
		if (test->getRunned() && !test->getResult())
			return false;
	return true;
}

Q_INVOKABLE bool TestListModel::getTestShouldBeRun(const int index) const noexcept {
	return tests.getTestsList().at(index)->getShouldBeRun();
}

Q_INVOKABLE void TestListModel::setTestShouldBeRun(const int index, const bool value) noexcept {
	return tests.getTestsList().at(index)->setShouldBeRun(value);
}

Q_INVOKABLE void TestListModel::setAllTestsShouldBeRun(const bool value) noexcept {
	for (auto const& test : tests.getTestsList())
		test->setShouldBeRun(value);
}

const std::vector<std::unique_ptr<AbstractTest>>& TestListModel::getTestsList() const noexcept {
	return tests.getTestsList();
}

std::vector<std::unique_ptr<AbstractTest>>& TestListModel::getTestsList() noexcept {
	return tests.getTestsList();
}

const TestSuit& TestListModel::getTestsSuit() const noexcept {
	return tests;
}

TestSuit& TestListModel::getTestsSuit() noexcept {
	return tests;
}

//DUMMY
QVariant TestListModel::data(const QModelIndex& index, int role) const {
	throw std::runtime_error("DONT USE THIS CLASS THAT WAY!");
}
