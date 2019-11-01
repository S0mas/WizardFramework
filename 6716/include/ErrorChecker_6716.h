#pragma once
#include "../../Common/include/ErrorChecker.h"
#include <visa.h>

class ErrorChecker_6716 : public ErrorChecker {
	ErrorStatus lastStatus;
public:
	template<typename Function, typename ...Args>
	void checkError6716(const ViSession vi, Function&& func, Args&& ...args) const;
	template<typename Function, typename ...Args>
	void checkError3416(const ViSession vi, Function&& func, Args&& ...args) const;
	template<typename Function, typename ...Args>
	void checkErrorT028(const ViSession vi, Function&& func, Args&& ...args) const;
	template<typename Function, typename ...Args>
	void checkErrorVisa(const ViSession vi, Function&& func, Args&& ...args) const;
};

template<typename Function, typename ...Args>
void ErrorChecker_6716::checkError6716(const ViSession vi, Function&& func, Args&& ...args) const {
	checkError(vi, &bu6716_error_message, std::forward<Function>(func), std::forward<Args>(args)...);
}

template<typename Function, typename ...Args>
void ErrorChecker_6716::checkError3416(const ViSession vi, Function&& func, Args&& ...args) const {
	checkError(vi, &bu3416_error_message, std::forward<Function>(func), std::forward<Args>(args)...);
}

template<typename Function, typename ...Args>
void ErrorChecker_6716::checkErrorT028(const ViSession vi, Function&& func, Args&& ...args) const {
	checkError(vi, &t028_error_message, std::forward<Function>(func), std::forward<Args>(args)...);
}

template<typename Function, typename ...Args>
void ErrorChecker_6716::checkErrorVisa(const ViSession vi, Function&& func, Args&& ...args) const {
	checkError(vi, &viStatusDesc, std::forward<Function>(func), std::forward<Args>(args)...);
}
