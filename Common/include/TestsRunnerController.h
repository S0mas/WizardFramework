#pragma once
#include "TestsRunner.h"
#include "LoggingObject.h"

////////////////////////////////////////////////////////////////////////////////////////////////////
/// @class	TestsRunnerController
///
/// @brief	A controller for handling tests runner and to communicate with it.
///
/// @author	Krzysztof Sommerfeld
/// @date	03.02.2020
////////////////////////////////////////////////////////////////////////////////////////////////////

class TestsRunnerController : public LoggingObject {
	Q_OBJECT

public slots:
	////////////////////////////////////////////////////////////////////////////////////////////////////
	/// @fn	Q_INVOKABLE void TestsRunnerController::startTests() const noexcept;
	///
	/// @brief	Sends start testrun command.
	///
	/// @author	Krzysztof Sommerfeld
	/// @date	03.02.2020
	////////////////////////////////////////////////////////////////////////////////////////////////////

	Q_INVOKABLE void startTests() const noexcept;

	////////////////////////////////////////////////////////////////////////////////////////////////////
	/// @fn	Q_INVOKABLE void TestsRunnerController::pauseTests() const noexcept;
	///
	/// @brief	Sends pause testrun command.
	///
	/// @author	Krzysztof Sommerfeld
	/// @date	03.02.2020
	////////////////////////////////////////////////////////////////////////////////////////////////////

	Q_INVOKABLE void pauseTests() const noexcept;

	////////////////////////////////////////////////////////////////////////////////////////////////////
	/// @fn	Q_INVOKABLE void TestsRunnerController::abortTests() const noexcept;
	///
	/// @brief	Sends abort testrun command.
	///
	/// @author	Krzysztof Sommerfeld
	/// @date	03.02.2020
	////////////////////////////////////////////////////////////////////////////////////////////////////

	Q_INVOKABLE void abortTests() const noexcept;

	////////////////////////////////////////////////////////////////////////////////////////////////////
	/// @fn	Q_INVOKABLE void TestsRunnerController::resumeTests() const noexcept;
	///
	/// @brief	Sends resume testrun command.
	///
	/// @author	Krzysztof Sommerfeld
	/// @date	03.02.2020
	////////////////////////////////////////////////////////////////////////////////////////////////////

	Q_INVOKABLE void resumeTests() const noexcept;

	////////////////////////////////////////////////////////////////////////////////////////////////////
	/// @fn	Q_INVOKABLE void TestsRunnerController::setChannelsStateOption(const std::vector<bool>& value) const noexcept;
	///
	/// @brief	Sends channels state selection.
	///
	/// @author	Krzysztof Sommerfeld
	/// @date	03.02.2020
	///
	/// @param 	value	Holds information about the number of channels(size) and
	/// 								which of them should be enabled(true) or disabled(false),
	/// 								the position + 1 is the index of the channel.
	////////////////////////////////////////////////////////////////////////////////////////////////////

	Q_INVOKABLE void setChannelsStateOption(const std::vector<bool>& value) const noexcept;
public:
	////////////////////////////////////////////////////////////////////////////////////////////////////
	/// @fn	TestsRunnerController::TestsRunnerController(QObject* parent = nullptr) noexcept;
	///
	/// @brief	Constructor
	///
	/// @author	Krzysztof Sommerfeld
	/// @date	03.02.2020
	///
	/// @param [in]	parent	(Optional) If not null - will be set as constructed object parent. In the
	/// 					QObject tree.
	////////////////////////////////////////////////////////////////////////////////////////////////////

	TestsRunnerController(QObject* parent = nullptr) noexcept;

	////////////////////////////////////////////////////////////////////////////////////////////////////
	/// @fn	TestsRunnerController::~TestsRunnerController() override;
	///
	/// @brief	Default destructor.
	///
	/// @author	Krzysztof Sommerfeld
	/// @date	03.02.2020
	////////////////////////////////////////////////////////////////////////////////////////////////////

	~TestsRunnerController() override = default;

	////////////////////////////////////////////////////////////////////////////////////////////////////
	/// @fn	void TestsRunnerController::connectTestsRunner(const TestsRunner* runner) noexcept;
	///
	/// @brief	Connects the TestRunner object signals and slots.
	///
	/// @author	Krzysztof Sommerfeld
	/// @date	03.02.2020
	///
	/// @param 	runner	Pointer to the runner object that will be connected with this controller.
	////////////////////////////////////////////////////////////////////////////////////////////////////

	void connectTestsRunner(const TestsRunner* runner) noexcept;
signals:
	////////////////////////////////////////////////////////////////////////////////////////////////////
	/// @fn	void TestsRunnerController::start() const;
	///
	/// @brief	Input signal, indicates the test run should be started.
	///
	/// @author	Krzysztof Sommerfeld
	/// @date	03.02.2020
	////////////////////////////////////////////////////////////////////////////////////////////////////

	void start() const;

	////////////////////////////////////////////////////////////////////////////////////////////////////
	/// @fn	void TestsRunnerController::pause() const;
	///
	/// @brief	Input signal, indicates the test run should be paused.
	///
	/// @author	Krzysztof Sommerfeld
	/// @date	03.02.2020
	////////////////////////////////////////////////////////////////////////////////////////////////////

	void pause() const;

	////////////////////////////////////////////////////////////////////////////////////////////////////
	/// @fn	void TestsRunnerController::abort() const;
	///
	/// @brief	Input signal, indicates the test run should be aborted.
	///
	/// @author	Krzysztof Sommerfeld
	/// @date	03.02.2020
	////////////////////////////////////////////////////////////////////////////////////////////////////

	void abort() const;

	////////////////////////////////////////////////////////////////////////////////////////////////////
	/// @fn	void TestsRunnerController::resume() const;
	///
	/// @brief	Input signal, indicates the test run should be resumed.
	///
	/// @author	Krzysztof Sommerfeld
	/// @date	03.02.2020
	////////////////////////////////////////////////////////////////////////////////////////////////////

	void resume() const;

	////////////////////////////////////////////////////////////////////////////////////////////////////
	/// @fn	void TestsRunnerController::setChannelsStates(const std::vector<bool>&) const;
	///
	/// @brief	Input signal, forwards the selected channels state.
	///
	/// @author	Krzysztof Sommerfeld
	/// @date	03.02.2020
	///
	/// @param 	states	Holds information about the number of channels(size) and
	/// 								which of them should be enabled(true) or disabled(false),
	/// 								the position + 1 is the index of the channel.
	////////////////////////////////////////////////////////////////////////////////////////////////////

	void setChannelsStates(const std::vector<bool>& states) const;

	////////////////////////////////////////////////////////////////////////////////////////////////////
	/// @fn	void TestsRunnerController::reportError(const QString& errorMsg) const;
	///
	/// @brief	Reports an error.
	///
	/// @author	Krzysztof Sommerfeld
	/// @date	03.02.2020
	///
	/// @param 	errorMsg	Information describing the error.
	////////////////////////////////////////////////////////////////////////////////////////////////////

	void reportError(const QString& errorMsg) const;

	////////////////////////////////////////////////////////////////////////////////////////////////////
	/// @fn	void TestsRunnerController::aborted() const;
	///
	/// @brief	Output signal, indicates that testrun was aborted.
	///
	/// @author	Krzysztof Sommerfeld
	/// @date	03.02.2020
	////////////////////////////////////////////////////////////////////////////////////////////////////

	void aborted() const;

	////////////////////////////////////////////////////////////////////////////////////////////////////
	/// @fn	void TestsRunnerController::completed(const bool result) const;
	///
	/// @brief	Output signal, indicates that testrun was completed. Forwards the testrun result.
	///
	/// @author	Krzysztof Sommerfeld
	/// @date	03.02.2020
	///
	/// @param 	result	True to result.
	////////////////////////////////////////////////////////////////////////////////////////////////////

	void completed(const bool result) const;

	////////////////////////////////////////////////////////////////////////////////////////////////////
	/// @fn	void TestsRunnerController::paused() const;
	///
	/// @brief	Output signal, indicates that testrun was paused.
	///
	/// @author	Krzysztof Sommerfeld
	/// @date	03.02.2020
	////////////////////////////////////////////////////////////////////////////////////////////////////

	void paused() const;

	////////////////////////////////////////////////////////////////////////////////////////////////////
	/// @fn	void TestsRunnerController::resumed() const;
	///
	/// @brief	Output signal, indicates that testrun execution was resumed.
	///
	/// @author	Krzysztof Sommerfeld
	/// @date	03.02.2020
	////////////////////////////////////////////////////////////////////////////////////////////////////

	void resumed() const;

	////////////////////////////////////////////////////////////////////////////////////////////////////
	/// @fn	void TestsRunnerController::oneFinished(QString name, int result) const;
	///
	/// @brief	Output signal, indicates that test has been finished. Forwards the name and result of the finished test.
	///
	/// @author	Krzysztof Sommerfeld
	/// @date	03.02.2020
	///
	/// @param 	name  	The name of finished test.
	/// @param 	result	The result of the test.
	////////////////////////////////////////////////////////////////////////////////////////////////////

	void oneFinished(QString name, int result) const;

	////////////////////////////////////////////////////////////////////////////////////////////////////
	/// @fn	void TestsRunnerController::oneRunned(QString name) const;
	///
	/// @brief	Output signal, indicates that test has been runned. Forwards the name of the runned test.
	///
	/// @author	Krzysztof Sommerfeld
	/// @date	03.02.2020
	///
	/// @param 	name	The name of the test.
	////////////////////////////////////////////////////////////////////////////////////////////////////

	void oneRunned(QString name) const;

	////////////////////////////////////////////////////////////////////////////////////////////////////
	/// @fn	void TestsRunnerController::oneSummary(const QString& msg) const;
	///
	/// @brief	Output signal, forwards the summary of the completed test
	///
	/// @author	Krzysztof Sommerfeld
	/// @date	03.02.2020
	///
	/// @param 	msg	The summary.
	////////////////////////////////////////////////////////////////////////////////////////////////////

	void oneSummary(const QString& msg) const;
};