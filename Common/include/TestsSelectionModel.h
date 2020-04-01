#pragma once
#include "AbstractTest.h"
#include <vector>

////////////////////////////////////////////////////////////////////////////////////////////////////
/// @class	TestsSelectionModel
///
/// @brief	A data Model for the tests selection.
///
/// @author	Krzysztof Sommerfeld
/// @date	03.02.2020
////////////////////////////////////////////////////////////////////////////////////////////////////

class TestsSelectionModel : public QObject {
	Q_OBJECT

	/// @brief	The tests container to select from - non owning.
	const std::vector<AbstractTest*>* tests;
public:

	////////////////////////////////////////////////////////////////////////////////////////////////////
	/// @fn	TestsSelectionModel::TestsSelectionModel(const std::vector<AbstractTest*>& tests, QObject* parent = nullptr);
	///
	/// @brief	Constructor
	///
	/// @author	Krzysztof Sommerfeld
	/// @date	03.02.2020
	///
	/// @param 		   	tests 	The tests to select from.
	/// @param [in]	parent	(Optional) If not null - will be set as constructed object parent. In the
	/// 					QObject tree.
	////////////////////////////////////////////////////////////////////////////////////////////////////

	TestsSelectionModel(const std::vector<AbstractTest*>& tests, QObject* parent = nullptr);

	////////////////////////////////////////////////////////////////////////////////////////////////////
	/// @fn	TestsSelectionModel::~TestsSelectionModel() override = default;
	///
	/// @brief	Default destructor
	///
	/// @author	Krzysztof Sommerfeld
	/// @date	03.02.2020
	////////////////////////////////////////////////////////////////////////////////////////////////////

	~TestsSelectionModel() override = default;

	////////////////////////////////////////////////////////////////////////////////////////////////////
	/// @fn	Q_INVOKABLE QString TestsSelectionModel::getTestName(const int index) const noexcept;
	///
	/// @brief	Gets test name
	///
	/// @author	Krzysztof Sommerfeld
	/// @date	03.02.2020
	///
	/// @param 	index	The test identifier.
	///
	/// @returns	The test name.
	////////////////////////////////////////////////////////////////////////////////////////////////////

	Q_INVOKABLE QString getTestName(const int index) const noexcept;

	////////////////////////////////////////////////////////////////////////////////////////////////////
	/// @fn	Q_INVOKABLE bool TestsSelectionModel::getTestShouldBeRun(const int index) const noexcept;
	///
	/// @brief	Gets information if the tests should be run or not.
	///
	/// @author	Krzysztof Sommerfeld
	/// @date	03.02.2020
	///
	/// @param 	index	The test identifier.
	///
	/// @returns	True if the test should be run.
	////////////////////////////////////////////////////////////////////////////////////////////////////

	Q_INVOKABLE bool getTestShouldBeRun(const int index) const noexcept;

	////////////////////////////////////////////////////////////////////////////////////////////////////
	/// @fn	Q_INVOKABLE void TestsSelectionModel::setTestShouldBeRun(const int index, const bool value) noexcept;
	///
	/// @brief	Sets if the test should be run or not.
	///
	/// @author	Krzysztof Sommerfeld
	/// @date	03.02.2020
	///
	/// @param 	index	The test identifier.
	/// @param 	value	The new value.
	////////////////////////////////////////////////////////////////////////////////////////////////////

	Q_INVOKABLE void setTestShouldBeRun(const int index, const bool value) noexcept;

	////////////////////////////////////////////////////////////////////////////////////////////////////
	/// @fn	Q_INVOKABLE void TestsSelectionModel::setAllTestsShouldBeRun(const bool value) noexcept;
	///
	/// @brief	Sets the if all the tests should be run or not.
	///
	/// @author	Krzysztof Sommerfeld
	/// @date	03.02.2020
	///
	/// @param 	value	The new value.
	////////////////////////////////////////////////////////////////////////////////////////////////////

	Q_INVOKABLE void setAllTestsShouldBeRun(const bool value) noexcept;

	////////////////////////////////////////////////////////////////////////////////////////////////////
	/// @fn	int TestsSelectionModel::count() const noexcept;
	///
	/// @brief	Gets the tests count
	///
	/// @author	Krzysztof Sommerfeld
	/// @date	03.02.2020
	///
	/// @returns	The number of tests to select from.
	////////////////////////////////////////////////////////////////////////////////////////////////////

	Q_INVOKABLE int count() const noexcept;
signals:
	////////////////////////////////////////////////////////////////////////////////////////////////////
	/// @fn	signals: void TestsSelectionModel::testShouldBeRunChanged(bool value, int id) const;
	///
	/// @brief	Output signal, indicates that the value of shouldBeRun for some test has been changed.
	///
	/// @author	Krzysztof Sommerfeld
	/// @date	03.02.2020
	///
	/// @param 	value	The new value.
	/// @param 	id   	The test identifier.
	////////////////////////////////////////////////////////////////////////////////////////////////////
	/// 
	void testShouldBeRunChanged(bool value, int id) const;
};