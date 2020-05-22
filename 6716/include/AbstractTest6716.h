#pragma once
#define INSTR_LANGUAGE_SPECIFIC
#include "../../Common/include/AbstractTest.h"
#include "../../../visa/include/visa.h"
#include "../../../bu6716/include/bu6716.h"
#include "../../../bu5716/t028/include/t028.h"
#include "../../../bu3416/include/bu3416.h"
#include "../../../bu6100/include/bu6100.h"
#include "../../Common/include/Device6716.h"
#include "../../Common/include/Device6100.h"
#include "../../Common/include/Device3416.h"
#include "../../Common/include/DeviceT028.h"
#include <memory>
#include <vector>

////////////////////////////////////////////////////////////////////////////////////////////////////
/// @struct	ExcitationCalibrationCoefficient
///
/// @brief	The excitation calibration coefficients.
///
/// @author	Krzysztof Sommerfeld
/// @date	05.02.2020
////////////////////////////////////////////////////////////////////////////////////////////////////

struct ExcitationCalibrationCoefficient {
	double positiveOffset = 0;
	double negativeOffset = 0;
	double positiveGain = 1;
	double negativeGain = 1;

	////////////////////////////////////////////////////////////////////////////////////////////////////
	/// @fn	QString toString() const noexcept
	///
	/// @brief	Convert this object into a string representation
	///
	/// @author	Krzysztof Sommerfeld
	/// @date	05.02.2020
	///
	/// @returns	A QString that represents this object.
	////////////////////////////////////////////////////////////////////////////////////////////////////

	QString toString() const noexcept {
		return QString("positiveOffset=%1, negativeOffset=%2, positiveGain=%3, negativeGain=%4").arg(positiveOffset).arg(negativeOffset).arg(positiveGain).arg(negativeGain);
	}
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// @struct	SignalPathCalibrationCoefficient
///
/// @brief	The signal path calibration coefficients. One for each of the four 6716 gains.
///
/// @author	Krzysztof Sommerfeld
/// @date	05.02.2020
////////////////////////////////////////////////////////////////////////////////////////////////////

struct SignalPathCalibrationCoefficient {
	std::vector<double> offsets = { 0, 0 ,0 ,0 };
	std::vector<double> gainCoefficients = { 1, 1, 1, 1 };
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// @class	AbstractTest6716
///
/// @brief	The abstract test for the 6716 device production test.
///
/// @author	Krzysztof Sommerfeld
/// @date	05.02.2020
////////////////////////////////////////////////////////////////////////////////////////////////////

class AbstractTest6716 : public AbstractChannelsTest {
	Q_OBJECT
protected:
	inline static Device6716* device6716 = nullptr;
	inline static Device6100* device6100 = nullptr;
	inline static Device3416* device3416_6716 = nullptr;
	inline static Device3416* device3416_t028 = nullptr;
	inline static DeviceT028* deviceT028 = nullptr;
	inline static std::vector<ExcitationCalibrationCoefficient> tempExcitationCalibrationData_;
	inline static std::vector<SignalPathCalibrationCoefficient> tempSignalPathCalibrationData_;
	inline static bool needToReloadExcitationCalibDataFromTestApplication = false;
	inline static bool needToReloadSignalPathCalibDataFromTestApplication = false;
	void preTestSetUp() const override;
	void postTestCleanUp() const override;

	////////////////////////////////////////////////////////////////////////////////////////////////////
	/// @fn	double AbstractTest6716::setAndGetVrefVoltage(const double voltage) const;
	///
	/// @brief	Sets the voltage to the closest value that is supported by the device and returns it.
 	///
	/// @author	Krzysztof Sommerfeld
	/// @date	05.02.2020
	///
	/// @param 	voltage	The voltage.
	///
	/// @returns	The seted voltage.
	////////////////////////////////////////////////////////////////////////////////////////////////////

	double setAndGetVrefVoltage(const double voltage) const;

	////////////////////////////////////////////////////////////////////////////////////////////////////
	/// @fn	bool AbstractTest6716::checkValue(const unsigned short expected, const unsigned short actual, const int form = 10, const int width = 0, const char fill = ' ') const noexcept;
	///
	/// @brief	Checks the value if the expected value is same as actual and logs it.
	///
	/// @author	Krzysztof Sommerfeld
	/// @date	05.02.2020
	///
	/// @param 	expected	The expected.
	/// @param 	actual  	The actual.
	/// @param 	form		(Optional) The form.
	/// @param 	width   	(Optional) The width.
	/// @param 	fill		(Optional) The fill.
	///
	/// @returns	True if it it, false if it is not.
	////////////////////////////////////////////////////////////////////////////////////////////////////

	bool checkValue(const unsigned short expected, const unsigned short actual, const int form = 10, const int width = 0, const char fill = ' ') const noexcept;

	////////////////////////////////////////////////////////////////////////////////////////////////////
	/// @fn	bool AbstractTest6716::checkId() const;
	///
	/// @brief	Checks if the id of the device is valid.
	///
	/// @author	Krzysztof Sommerfeld
	/// @date	05.02.2020
	///
	/// @returns	True if it is, false if it is not.
	////////////////////////////////////////////////////////////////////////////////////////////////////

	bool checkId() const;

	////////////////////////////////////////////////////////////////////////////////////////////////////
	/// @fn	bool AbstractTest6716::testWriteRead(const unsigned char expected, const unsigned char address) const;
	///
	/// @brief	Tests write and read for the selected address.
	///
	/// @author	Krzysztof Sommerfeld
	/// @date	05.02.2020
	///
	/// @param 	expected	The expected value.
	/// @param 	address 	The address.
	///
	/// @returns	True if the test passes, false if the test fails.
	////////////////////////////////////////////////////////////////////////////////////////////////////

	bool testWriteRead(const unsigned char expected, const unsigned char address) const;

	////////////////////////////////////////////////////////////////////////////////////////////////////
	/// @fn	Result AbstractTest6716::excitationTest(const Limit& limit1, const Limit& limi2, const bool positive) const;
	///
	/// @brief	Tests the excitation. Positive and negative.
	///
	/// @author	Krzysztof Sommerfeld
	/// @date	05.02.2020
	///
	/// @param 	limit1  	The first limit.
	/// @param 	limi2   	The second limi.
	/// @param 	positive	True, if positive excitation should be tested.
	///
	/// @returns	The result of the test.
	////////////////////////////////////////////////////////////////////////////////////////////////////

	Result excitationTest(const Limit& limit1, const Limit& limi2, const bool positive) const;
public:
	////////////////////////////////////////////////////////////////////////////////////////////////////
	/// @fn	void AbstractTest6716::setChannelsStates(const std::vector<bool>& states) noexcept override;
	///
	/// @brief	Sets channels states. Enables or disables them.
	///
	/// @author	Krzysztof Sommerfeld
	/// @date	05.02.2020
	///
	/// @param 	states	The states.
	////////////////////////////////////////////////////////////////////////////////////////////////////

	void setChannelsStates(const std::vector<bool>& states) noexcept override;

	////////////////////////////////////////////////////////////////////////////////////////////////////
	/// @fn	AbstractTest6716::AbstractTest6716(const QString&& name);
	///
	/// @brief	Constructor
	///
	/// @author	Krzysztof Sommerfeld
	/// @date	05.02.2020
	///
	/// @param 	name	The name.
	////////////////////////////////////////////////////////////////////////////////////////////////////

	AbstractTest6716(const QString&& name);

	////////////////////////////////////////////////////////////////////////////////////////////////////
	/// @fn	inline static void AbstractTest6716::setDevices(AbstractDevice* const c6716, AbstractDevice* const c6100, AbstractDevice* const c3416_6716, AbstractDevice* const c3416_t028, AbstractDevice* const cT028) noexcept
	///
	/// @brief	Sets the devices ptrs that will be used in the test.
	///
	/// @author	Krzysztof Sommerfeld
	/// @date	05.02.2020
	///
	/// @param [in]	c6716	  	The ptr to the 6716 device.
	/// @param [in]	c6100	  	The ptr to the 6100 device.
	/// @param [in]	c3416_6716	The ptr to the 3416 connected to the 6716 device.
	/// @param [in]	c3416_t028	The ptr to the 3416 connected to the t028 device.
	/// @param [in]	cT028	  	The ptr to the t028 device.
	////////////////////////////////////////////////////////////////////////////////////////////////////

	inline static void setDevices(AbstractDevice* const c6716, AbstractDevice* const c6100, AbstractDevice* const c3416_6716, AbstractDevice* const c3416_t028, AbstractDevice* const cT028) noexcept {
		device6716 = dynamic_cast<Device6716*>(c6716);
		device6100 = dynamic_cast<Device6100*>(c6100);
		device3416_6716 = dynamic_cast<Device3416*>(c3416_6716);
		device3416_t028 = dynamic_cast<Device3416*>(c3416_t028);
		deviceT028 = dynamic_cast<DeviceT028*>(cT028);
	}
};



