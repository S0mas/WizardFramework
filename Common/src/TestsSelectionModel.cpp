#include "..\include\TestsSelectionModel.h"

TestsSelectionModel::TestsSelectionModel(const std::vector<AbstractTest*>& tests, QObject* parent) : QObject(parent), tests(&tests) {}

QString TestsSelectionModel::getTestName(const int index) const noexcept {
	return tests->at(index)->name();
}

bool TestsSelectionModel::getTestShouldBeRun(const int index) const noexcept {
	return tests->at(index)->shouldBeRun();
}

void TestsSelectionModel::setTestShouldBeRun(const int index, const bool value) noexcept {
	tests->at(index)->setShouldBeRun(value);
	emit testShouldBeRunChanged(tests->at(index)->shouldBeRun(), index);
}

void TestsSelectionModel::setAllTestsShouldBeRun(const bool value) noexcept {
	for (int index = 0; index < tests->size(); ++index) {
		tests->at(index)->setShouldBeRun(value);
		emit testShouldBeRunChanged(tests->at(index)->shouldBeRun(), index);
	}
}

int TestsSelectionModel::count() const noexcept {
	return tests->size();
}
