#pragma once
#include <QString>

////////////////////////////////////////////////////////////////////////////////////////////////////
/// @class	Limit
///
/// @brief	Represents the voltage limit.
///
/// @author	Krzysztof Sommerfeld
/// @date	05.02.2020
////////////////////////////////////////////////////////////////////////////////////////////////////

class Limit {
	QString name_;
	double min_;
	double max_;
	mutable QString lastStatusMsg_;

	////////////////////////////////////////////////////////////////////////////////////////////////////
	/// @fn	void Limit::setLogMsg(const double min, const double max, const double measured, const bool status) const noexcept
	///
	/// @brief	Sets log message
	///
	/// @author	Krzysztof Sommerfeld
	/// @date	05.02.2020
	///
	/// @param 	min			The minimum.
	/// @param 	max			The maximum.
	/// @param 	measured	The measured.
	/// @param 	status  	The status, true if measred is in the range of <min, max>.
	////////////////////////////////////////////////////////////////////////////////////////////////////

	void setLogMsg(const double min, const double max, const double measured, const bool status) const noexcept {
		lastStatusMsg_ = status ? QString("OK: Measured voltage %1 is within the correct range <%2, %3>.").arg(measured).arg(min).arg(max) : QString("ERROR: Measured voltage %1 is not within the correct range <%2, %3>.").arg(measured).arg(min).arg(max);
	}
public:
	////////////////////////////////////////////////////////////////////////////////////////////////////
	/// @fn	Limit::Limit(const QString& name, const double min, const double max)
	///
	/// @brief	Constructor
	///
	/// @author	Krzysztof Sommerfeld
	/// @date	05.02.2020
	///
	/// @param 	name	The name.
	/// @param 	min 	The minimum.
	/// @param 	max 	The maximum.
	////////////////////////////////////////////////////////////////////////////////////////////////////

	Limit(const QString& name, const double min, const double max) :
		name_(name),
		min_(min),
		max_(max) {}

	////////////////////////////////////////////////////////////////////////////////////////////////////
	/// @fn	QString Limit::lastStatusMsg() const noexcept
	///
	/// @brief	Last status message
	///
	/// @author	Krzysztof Sommerfeld
	/// @date	05.02.2020
	///
	/// @returns	A QString.
	////////////////////////////////////////////////////////////////////////////////////////////////////

	QString lastStatusMsg() const noexcept {
		return lastStatusMsg_;
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////
	/// @fn	bool Limit::checkValue(const double measured, const int gain = 1) const noexcept
	///
	/// @brief	Checks the value if it is in the range of the limit.
	///
	/// @author	Krzysztof Sommerfeld
	/// @date	05.02.2020
	///
	/// @param 	measured	The measured.
	/// @param 	gain		(Optional) The gain.
	///
	/// @returns	True if it is, false if it is not.
	////////////////////////////////////////////////////////////////////////////////////////////////////

	bool checkValue(const double measured, const int gain = 1) const noexcept {
		auto const status = measured >= (min_ * gain) && measured <= (max_ * gain);
		setLogMsg(min_ * gain, max_ * gain, measured, status);
		return status;
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////
	/// @fn	bool Limit::checkValueSigned(const double measured, const int gain = 1) const noexcept
	///
	/// @brief	Checks the value if it is in the range of the limit.(opposite sign)
	///
	/// @author	Krzysztof Sommerfeld
	/// @date	05.02.2020
	///
	/// @param 	measured	The measured.
	/// @param 	gain		(Optional) The gain.
	///
	/// @returns	True if it is, false if it is not.
	////////////////////////////////////////////////////////////////////////////////////////////////////

	bool checkValueSigned(const double measured, const int gain = 1) const noexcept {
		auto const signedMin = max_ * -1;
		auto const signedMax = min_ * -1;
		auto const status = measured >= (signedMin * gain) && measured <= (signedMax * gain);
		setLogMsg(signedMin * gain, signedMax * gain, measured, status);
		return status;
	}
};