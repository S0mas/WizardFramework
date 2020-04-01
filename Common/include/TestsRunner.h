#pragma once
#include "AbstractTest.h"
#include "PrintInterface.h"

#include <QThread>
#include <memory>

////////////////////////////////////////////////////////////////////////////////////////////////////
/// @class	TestsRunner
///
/// @brief	Runs the contained tests that are selected. Also, is responsible for controling the testrun and for tests objects.
///
/// @author	Krzysztof Sommerfeld
/// @date	03.02.2020
////////////////////////////////////////////////////////////////////////////////////////////////////

class TestsRunner : public QObject {
	Q_OBJECT
	bool paused = false;
	bool aborted = false;
	std::vector<AbstractTest*> tests_;

	////////////////////////////////////////////////////////////////////////////////////////////////////
	/// @fn	void TestsRunner::connectTest(AbstractTest& test) const noexcept;
	///
	/// @brief	Connects a test
	///
	/// @author	Krzysztof Sommerfeld
	/// @date	03.02.2020
	///
	/// @param [in]	test	The test that will be connected to this runner.
	////////////////////////////////////////////////////////////////////////////////////////////////////

	void connectTest(AbstractTest& test) const noexcept;
public:
	////////////////////////////////////////////////////////////////////////////////////////////////////
	/// @fn	TestsRunner::TestsRunner(QObject* parent = nullptr) noexcept
	///
	/// @brief	Constructor
	///
	/// @author	Krzysztof Sommerfeld
	/// @date	03.02.2020
	///
	/// @param [in]	parent	(Optional) If not null - will be set as constructed object parent. In the
	/// 					QObject tree.
	////////////////////////////////////////////////////////////////////////////////////////////////////

	TestsRunner(QObject* parent = nullptr) noexcept : QObject(parent) {}

	////////////////////////////////////////////////////////////////////////////////////////////////////
	/// @fn	TestsRunner::~TestsRunner() override = default;
	///
	/// @brief	Default destructor
	///
	/// @author	Krzysztof Sommerfeld
	/// @date	03.02.2020
	////////////////////////////////////////////////////////////////////////////////////////////////////

	~TestsRunner() override = default;

	////////////////////////////////////////////////////////////////////////////////////////////////////
	/// @fn	void TestsRunner::addTest(AbstractTest* test);
	///
	/// @brief	Adds a test
	///
	/// @author	Krzysztof Sommerfeld
	/// @date	03.02.2020
	///
	/// @param [in]	test	Pointer to the test object - it also takes the ownership of it.
	////////////////////////////////////////////////////////////////////////////////////////////////////

	void addTest(AbstractTest* test);

	////////////////////////////////////////////////////////////////////////////////////////////////////
	/// @fn	std::vector<AbstractTest*>& TestsRunner::tests() noexcept;
	///
	/// @brief	Gets the tests
	///
	/// @author	Krzysztof Sommerfeld
	/// @date	03.02.2020
	///
	/// @returns	Vector of test pointers
	////////////////////////////////////////////////////////////////////////////////////////////////////

	std::vector<AbstractTest*>& tests() noexcept;
public slots:
	////////////////////////////////////////////////////////////////////////////////////////////////////
	/// @fn	public slots: void TestsRunner::startTests() noexcept;
	///
	/// @brief	Starts the testrun
	///
	/// @author	Krzysztof Sommerfeld
	/// @date	03.02.2020
	////////////////////////////////////////////////////////////////////////////////////////////////////

	void startTests() noexcept;

	////////////////////////////////////////////////////////////////////////////////////////////////////
	/// @fn	void TestsRunner::pauseTests() noexcept;
	///
	/// @brief	Pauses the testrun
	///
	/// @author	Krzysztof Sommerfeld
	/// @date	03.02.2020
	////////////////////////////////////////////////////////////////////////////////////////////////////

	void pauseTests() noexcept;

	////////////////////////////////////////////////////////////////////////////////////////////////////
	/// @fn	void TestsRunner::resumeTests() noexcept;
	///
	/// @brief	Resumes the testrun
	///
	/// @author	Krzysztof Sommerfeld
	/// @date	03.02.2020
	////////////////////////////////////////////////////////////////////////////////////////////////////

	void resumeTests() noexcept;

	////////////////////////////////////////////////////////////////////////////////////////////////////
	/// @fn	void TestsRunner::abortTests() noexcept;
	///
	/// @brief	Aborts the testrun
	///
	/// @author	Krzysztof Sommerfeld
	/// @date	03.02.2020
	////////////////////////////////////////////////////////////////////////////////////////////////////

	void abortTests() noexcept;

	////////////////////////////////////////////////////////////////////////////////////////////////////
	/// @fn	void TestsRunner::setChannelsStates(const std::vector<bool>& value) noexcept;
	///
	/// @brief	Input signal, forwards the selected channels state.
	///
	/// @author	Krzysztof Sommerfeld
	/// @date	03.02.2020
	///
	/// @param 	value	Holds information about the number of channels(size) and
	/// 								which of them should be enabled(true) or disabled(false),
	/// 								 the position + 1 is the index of the channel.
	////////////////////////////////////////////////////////////////////////////////////////////////////

	void setChannelsStates(const std::vector<bool>& value) noexcept;
signals:
	////////////////////////////////////////////////////////////////////////////////////////////////////
	/// @fn	void TestsRunner::reportError(const QString& errorMsg) const;
	///
	/// @brief	Reports an error.
	///
	/// @author	Krzysztof Sommerfeld
	/// @date	03.02.2020
	///
	/// @param 	errorMsg	Message describing the error.
	////////////////////////////////////////////////////////////////////////////////////////////////////
	/// 
	void reportError(const QString& errorMsg) const;

	////////////////////////////////////////////////////////////////////////////////////////////////////
	/// @fn	void TestsRunner::testsCompleted(const bool result) const;
	///
	/// @brief	The output signal, indicates the testrun has been completed. Forwards the result.
	///
	/// @author	Krzysztof Sommerfeld
	/// @date	03.02.2020
	///
	/// @param 	result	True to result.
	////////////////////////////////////////////////////////////////////////////////////////////////////

	void testsCompleted(const bool result) const;

	////////////////////////////////////////////////////////////////////////////////////////////////////
	/// @fn	void TestsRunner::testRunned(QString name) const;
	///
	/// @brief	The output signal, indicates the test has been runned.
	///
	/// @author	Krzysztof Sommerfeld
	/// @date	03.02.2020
	///
	/// @param 	name	The name.
	////////////////////////////////////////////////////////////////////////////////////////////////////

	void testRunned(QString name) const;

	////////////////////////////////////////////////////////////////////////////////////////////////////
	/// @fn	void TestsRunner::testFinished(QString name, int result) const;
	///
	/// @brief	The output signal, indicates the test has been finished.
	///
	/// @author	Krzysztof Sommerfeld
	/// @date	03.02.2020
	///
	/// @param 	name  	The name.
	/// @param 	result	The result.
	////////////////////////////////////////////////////////////////////////////////////////////////////

	void testFinished(QString name, int result) const;

	////////////////////////////////////////////////////////////////////////////////////////////////////
	/// @fn	void TestsRunner::testsAborted() const;
	///
	/// @brief	The output signal, indicates the testrun has been aborted.
	///
	/// @author	Krzysztof Sommerfeld
	/// @date	03.02.2020
	////////////////////////////////////////////////////////////////////////////////////////////////////

	void testsAborted() const;

	////////////////////////////////////////////////////////////////////////////////////////////////////
	/// @fn	void TestsRunner::testsPaused() const;
	///
	/// @brief	The output signal, indicates the testrun has been paused.
	///
	/// @author	Krzysztof Sommerfeld
	/// @date	03.02.2020
	////////////////////////////////////////////////////////////////////////////////////////////////////

	void testsPaused() const;

	////////////////////////////////////////////////////////////////////////////////////////////////////
	/// @fn	void TestsRunner::testsResumed() const;
	///
	/// @brief	The output signal, indicates the testrun has been resumed.
	///
	/// @author	Krzysztof Sommerfeld
	/// @date	03.02.2020
	////////////////////////////////////////////////////////////////////////////////////////////////////

	void testsResumed() const;

	////////////////////////////////////////////////////////////////////////////////////////////////////
	/// @fn	void TestsRunner::logMsg(const QString& msg) const;
	///
	/// @brief	The output signal, forwards a message.
	///
	/// @author	Krzysztof Sommerfeld
	/// @date	03.02.2020
	///
	/// @param 	msg	The message.
	////////////////////////////////////////////////////////////////////////////////////////////////////

	void logMsg(const QString& msg) const;

	////////////////////////////////////////////////////////////////////////////////////////////////////
	/// @fn	void TestsRunner::testSummary(const QString& msg) const;
	///
	/// @brief	The output signal, sends test summary.
	///
	/// @author	Krzysztof Sommerfeld
	/// @date	03.02.2020
	///
	/// @param 	msg	The summary.
	////////////////////////////////////////////////////////////////////////////////////////////////////

	void testSummary(const QString& msg) const;
};