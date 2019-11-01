#pragma once
#include <QString>

enum class StatusType {
	OK,
	WRN,
	ERR
};

class ErrorStatus {
	QString lastStatusMsg = "";
	StatusType type = StatusType::OK;
	QString statusTypeToString(const StatusType type) const noexcept {
		if (type == StatusType::WRN)
			return "WRN:";
		else if (type == StatusType::ERR)
			return "ERR:";
		return "SUCCESS";
	}
public:
	ErrorStatus() noexcept {}
	ErrorStatus(const QString& str, const int status) : lastStatusMsg(str) {
		if (status > 0)
			type = StatusType::WRN;
		else if (status < 0)
			type = StatusType::ERR;
	}

	QString toString() const noexcept {
		return statusTypeToString(type) + lastStatusMsg;
	}

	StatusType statusType() const noexcept {
		return type;
	}
};

class ErrorChecker {
	mutable ErrorStatus lastStatus;
public:
	template<typename ErrorFunction, typename FunctionToTest, typename ...Args>
	void checkError(const unsigned long vi, ErrorFunction&& errorFunc, FunctionToTest&& func, Args&& ...args) const {
		lastStatus = ErrorStatus();
		int status = func(std::forward<Args>(args)...);
		if (status != 0) {
			char errBuf[1024];
			errorFunc(vi, status, errBuf);
			lastStatus = ErrorStatus(errBuf, status);
		}
	}

	ErrorStatus getLastStatus() const noexcept {
		return lastStatus;
	}
};