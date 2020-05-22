#pragma once
#include <QString>
#include <optional>
#include <functional>

////////////////////////////////////////////////////////////////////////////////////////////////////
/// @enum	StatusType
///
/// @brief	Values that represent status types
////////////////////////////////////////////////////////////////////////////////////////////////////

enum class StatusType {
	OK,
	WRN,
	ERR
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// @class	ErrorStatus
///
/// @brief	An error status.
///
/// @author	Krzysztof Sommerfeld
/// @date	03.02.2020
////////////////////////////////////////////////////////////////////////////////////////////////////

class ErrorStatus {
	QString lastStatusMsg;
	StatusType type;

	////////////////////////////////////////////////////////////////////////////////////////////////////
	/// @fn	QString ErrorStatus::statusTypeToString(const StatusType type) const noexcept
	///
	/// @brief	Status type to string
	///
	/// @author	Krzysztof Sommerfeld
	/// @date	03.02.2020
	///
	/// @param 	type	The type.
	///
	/// @returns	The type as string.
	////////////////////////////////////////////////////////////////////////////////////////////////////

	QString statusTypeToString(const StatusType type) const noexcept {
		if (type == StatusType::WRN)
			return "WRN:";
		else if (type == StatusType::ERR)
			return "ERR:";
		return "SUCCESS";
	}
public:
	////////////////////////////////////////////////////////////////////////////////////////////////////
	/// @fn	ErrorStatus::ErrorStatus() noexcept : lastStatusMsg(""), type(StatusType::OK)
	///
	/// @brief	Default constructor
	///
	/// @author	Krzysztof Sommerfeld
	/// @date	03.02.2020
	////////////////////////////////////////////////////////////////////////////////////////////////////

	ErrorStatus() noexcept : lastStatusMsg(""),  type(StatusType::OK) {}

	////////////////////////////////////////////////////////////////////////////////////////////////////
	/// @fn	ErrorStatus::ErrorStatus(const QString& str, const int status)
	///
	/// @brief	Constructor
	///
	/// @author	Krzysztof Sommerfeld
	/// @date	03.02.2020
	///
	/// @param 	str   	The string.
	/// @param 	status	The status.
	////////////////////////////////////////////////////////////////////////////////////////////////////

	ErrorStatus(const QString& str, const int status) : lastStatusMsg(str + QString(", Status: %1").arg(status)) {
		if (status > 0)
			type = StatusType::WRN;
		else if (status < 0)
			type = StatusType::ERR;
		else
			type = StatusType::OK;
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////
	/// @fn	QString ErrorStatus::toString() const noexcept
	///
	/// @brief	Convert this object into a string representation
	///
	/// @author	Krzysztof Sommerfeld
	/// @date	03.02.2020
	///
	/// @returns	A QString that represents this object.
	////////////////////////////////////////////////////////////////////////////////////////////////////

	QString toString() const noexcept {
		return statusTypeToString(type) + lastStatusMsg;
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////
	/// @fn	StatusType ErrorStatus::statusType() const noexcept
	///
	/// @brief	Gets the status type
	///
	/// @author	Krzysztof Sommerfeld
	/// @date	03.02.2020
	///
	/// @returns	The statusType.
	////////////////////////////////////////////////////////////////////////////////////////////////////

	StatusType statusType() const noexcept {
		return type;
	}
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// @class	ErrorChecker
///
/// @brief	An error checker class. Used to handle the hardware dirvers functions calls.
///
/// @author	Krzysztof Sommerfeld
/// @date	03.02.2020
////////////////////////////////////////////////////////////////////////////////////////////////////

class ErrorChecker {
	ErrorStatus lastStatus;
	std::function<int(unsigned long, int, char*)> readErrorFunc = [](...) { return 0; };
public:
	////////////////////////////////////////////////////////////////////////////////////////////////////
	/// @fn	ErrorChecker::ErrorChecker(const std::function<int(unsigned long, int, char*)>& errorFunction) noexcept
	///
	/// @brief	Constructor
	///
	/// @author	Krzysztof Sommerfeld
	/// @date	03.02.2020
	///
	/// @param 	errorFunction	The error function.
	////////////////////////////////////////////////////////////////////////////////////////////////////

	ErrorChecker(const std::function<int(unsigned long, int, char*)>& errorFunction) noexcept : readErrorFunc(errorFunction) {}

	////////////////////////////////////////////////////////////////////////////////////////////////////
	/// @fn	template<typename FunctionToTest, typename ...Args> int ErrorChecker::checkError(const unsigned long vi, FunctionToTest&& func, Args&& ...args) noexcept
	///
	/// @brief	Invokes the functions and checks if there was any error.
	///
	/// @tparam	FunctionToTest	Type of the function to test.
	/// @tparam	...Args		  	Types of the arguments pack.
	/// @param  vi	   	The vi that needs to be added as argument to the function call.
	/// @param  func   	The function to invoke.
	/// @param  ...args	The arguments pack.
	///
	/// @returns	The status of that invoked function returned.
	////////////////////////////////////////////////////////////////////////////////////////////////////

	template<typename FunctionToTest, typename ...Args>
	int checkError(const unsigned long vi, FunctionToTest&& func, Args&& ...args) noexcept {
		lastStatus = ErrorStatus();
		int status = func(vi, std::forward<Args>(args)...);
		if (status != 0) {
			char errBuf[256];
			readErrorFunc(vi, status, errBuf);
			lastStatus = ErrorStatus(errBuf, status);
		}
		return status;
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////
	/// @fn	template<typename FunctionToTest, typename ...Args> int ErrorChecker::checkError(FunctionToTest&& func, Args&& ...args) noexcept
	///
	/// @brief	Invokes the functions and checks if there was any error.
	///
	/// @tparam	FunctionToTest	Type of the function to test.
	/// @tparam	...Args		  	Types of the arguments pack.
	/// @param  func   	The function to invoke.
	/// @param  ...args	The arguments pack.
	///
	/// @returns	The status of that invoked function returned.
	////////////////////////////////////////////////////////////////////////////////////////////////////

	template<typename FunctionToTest, typename ...Args>
	int checkError(FunctionToTest&& func, Args&& ...args) noexcept {
		lastStatus = ErrorStatus();
		int status = func(std::forward<Args>(args)...);
		if (status != 0) {
			char errBuf[256];
			readErrorFunc(0, status, errBuf);
			lastStatus = ErrorStatus(errBuf, status);
		}
		return status;
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////
	/// @fn	QString ErrorChecker::lastErrorMsg() const noexcept
	///
	/// @brief	Gets the last error message
	///
	/// @author	Krzysztof Sommerfeld
	/// @date	03.02.2020
	///
	/// @returns	The last error msg.
	////////////////////////////////////////////////////////////////////////////////////////////////////

	QString lastErrorMsg() const noexcept {
		return lastStatus.toString();
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////
	/// @fn	bool ErrorChecker::errorOccured() const noexcept
	///
	/// @brief	Determines if the error occured
	///
	/// @author	Krzysztof Sommerfeld
	/// @date	03.02.2020
	///
	/// @returns	True if it ocurred, false if not.
	////////////////////////////////////////////////////////////////////////////////////////////////////

	bool errorOccured() const noexcept {
		if (lastStatus.statusType() == StatusType::ERR)
			return true;
		return false;
	}
};