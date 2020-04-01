#pragma once
#include "bu6716.h"
#include "ScpiDevice.h"
#include "DeviceIdentityResourcesIF.h"
#include "ChannelsIF.h"

////////////////////////////////////////////////////////////////////////////////////////////////////
/// @class	Device6716
///
/// @brief	The virtual representation of the 6716 device. The only implemented functionality is that used in production tests.
///
/// @author	Krzysztof Sommerfeld
/// @date	06.02.2020
////////////////////////////////////////////////////////////////////////////////////////////////////

class Device6716 : public ScpiDevice, public DeviceIdentityResourcesIF, public ChannelsIF {
	Q_OBJECT
	void setAutoDAC(const double voltage, const short select) const;
	std::vector<double> getAutoDAC(short select) const;
public:

	////////////////////////////////////////////////////////////////////////////////////////////////////
	/// @fn	Device6716::Device6716(const QString& nameId, QObject* parent = nullptr) noexcept;
	///
	/// @brief	Constructor
	///
	/// @author	Krzysztof Sommerfeld
	/// @date	06.02.2020
	///
	/// @param 		   	nameId	Identifier for the name.
	/// @param [in]	parent	(Optional) If not null - will be set as constructed object parent. In the
	/// 					QObject tree.
	////////////////////////////////////////////////////////////////////////////////////////////////////

	Device6716(const QString& nameId, QObject* parent = nullptr) noexcept;
	~Device6716() override = default;

	////////////////////////////////////////////////////////////////////////////////////////////////////
	/// @fn	bool Device6716::writeFPGAreg(const unsigned char address, const unsigned char data) const;
	///
	/// @brief	Writes the fpga reg.
	///
	/// @author	Krzysztof Sommerfeld
	/// @date	06.02.2020
	///
	/// @param 	address	The address of register.
	/// @param 	data   	The data byte.
	///
	/// @returns	True if it succeeds, false if it fails.
	////////////////////////////////////////////////////////////////////////////////////////////////////

	bool writeFPGAreg(const unsigned char address, const unsigned char data) const;

	////////////////////////////////////////////////////////////////////////////////////////////////////
	/// @fn	unsigned char Device6716::readFPGAreg(const unsigned char address) const;
	///
	/// @brief	Reads fpga reg.
	///
	/// @author	Krzysztof Sommerfeld
	/// @date	06.02.2020
	///
	/// @param 	address	The address of register.
	///
	/// @returns	The data byte.
	////////////////////////////////////////////////////////////////////////////////////////////////////

	unsigned char readFPGAreg(const unsigned char address) const;

	////////////////////////////////////////////////////////////////////////////////////////////////////
	/// @fn	bool Device6716::writePortExpander(const int channelIndex, const unsigned short data) const;
	///
	/// @brief	Writes a port expander for selected channel, it contains the channel configuration register.
	///
	/// @author	Krzysztof Sommerfeld
	/// @date	06.02.2020
	///
	/// @param 	channelIndex	The channel index.
	/// @param 	data			The new port expander state.
	///
	/// @returns	True if it succeeds, false if it fails.
	////////////////////////////////////////////////////////////////////////////////////////////////////

	bool writePortExpander(const int channelIndex, const unsigned short data) const;

	////////////////////////////////////////////////////////////////////////////////////////////////////
	/// @fn	unsigned short Device6716::readPortExpander(const int channelIndex) const;
	///
	/// @brief	Reads port expander for selected channel, it contains the channel configuration register.
	///
	/// @author	Krzysztof Sommerfeld
	/// @date	06.02.2020
	///
	/// @param 	channelIndex	The channel index.
	///
	/// @returns	The port expander state.
	////////////////////////////////////////////////////////////////////////////////////////////////////

	unsigned short readPortExpander(const int channelIndex) const;

	////////////////////////////////////////////////////////////////////////////////////////////////////
	/// @fn	void Device6716::writeI2cNoAddress(const unsigned char i2cDeviceAddress, const unsigned char data) const;
	///
	/// @brief	Writes i2c device - without the data address.
	///
	/// @author	Krzysztof Sommerfeld
	/// @date	06.02.2020
	///
	/// @param 	i2cDeviceAddress	Zero-based index of the 2c device address.
	/// @param 	data				The data.
	////////////////////////////////////////////////////////////////////////////////////////////////////

	void writeI2cNoAddress(const unsigned char i2cDeviceAddress, const unsigned char data) const;

	////////////////////////////////////////////////////////////////////////////////////////////////////
	/// @fn	unsigned char Device6716::readI2cNoAddress(const unsigned char i2cDeviceAddress) const;
	///
	/// @brief	Reads i2c device - without the data address.
	///
	/// @author	Krzysztof Sommerfeld
	/// @date	06.02.2020
	///
	/// @param 	i2cDeviceAddress	The i2c device address.
	///
	/// @returns	The data byte.
	////////////////////////////////////////////////////////////////////////////////////////////////////

	unsigned char readI2cNoAddress(const unsigned char i2cDeviceAddress) const;

	////////////////////////////////////////////////////////////////////////////////////////////////////
	/// @fn	void Device6716::writeI2c(const unsigned char i2cDeviceAddress, const unsigned char address, const unsigned char data) const;
	///
	/// @brief	Writes i2c.
	///
	/// @author	Krzysztof Sommerfeld
	/// @date	06.02.2020
	///
	/// @param 	i2cDeviceAddress	The i2c device address.
	/// @param 	address				The memory address.
	/// @param 	data				The data byte.
	////////////////////////////////////////////////////////////////////////////////////////////////////

	void writeI2c(const unsigned char i2cDeviceAddress, const unsigned char address, const unsigned char data) const;

	////////////////////////////////////////////////////////////////////////////////////////////////////
	/// @fn	unsigned char Device6716::readI2c(const unsigned char i2cDeviceAddress, const unsigned char address) const;
	///
	/// @brief	Reads i2c.
	///
	/// @author	Krzysztof Sommerfeld
	/// @date	06.02.2020
	///
	/// @param 	i2cDeviceAddress	The i2c device address.
	/// @param 	address				The memory address.
	///
	/// @returns	The read data byte.
	////////////////////////////////////////////////////////////////////////////////////////////////////

	unsigned char readI2c(const unsigned char i2cDeviceAddress, const unsigned char address) const;

	////////////////////////////////////////////////////////////////////////////////////////////////////
	/// @fn	bool Device6716::writeTeds2(const unsigned char data) const;
	///
	/// @brief	Writes the TEDS Class 2.
	///
	/// @author	Krzysztof Sommerfeld
	/// @date	06.02.2020
	///
	/// @param 	data	The data.
	///
	/// @returns	True if it succeeds, false if it fails.
	////////////////////////////////////////////////////////////////////////////////////////////////////

	bool writeTeds2(const unsigned char data) const;

	////////////////////////////////////////////////////////////////////////////////////////////////////
	/// @fn	unsigned char Device6716::readTeds2() const;
	///
	/// @brief	Reads TEDS Class 2
	///
	/// @author	Krzysztof Sommerfeld
	/// @date	06.02.2020
	///
	/// @returns	True if it succeeds, false if it fails.
	////////////////////////////////////////////////////////////////////////////////////////////////////

	unsigned char readTeds2() const;

	////////////////////////////////////////////////////////////////////////////////////////////////////
	/// @fn	ViStatus Device6716::oneWireCommander(unsigned short* cmds) const;
	///
	/// @brief	Invokes commands for one-wire interface
	///
	/// @author	Krzysztof Sommerfeld
	/// @date	06.02.2020
	///
	/// @param [in,out]	cmds	If non-null, the commands.
	///
	/// @returns	The ViStatus.
	////////////////////////////////////////////////////////////////////////////////////////////////////

	ViStatus oneWireCommander(unsigned short* cmds) const;

	////////////////////////////////////////////////////////////////////////////////////////////////////
	/// @fn	unsigned char Device6716::readDSR() const;
	///
	/// @brief	Reads the DSR(Device Status Register).
	///
	/// @author	Krzysztof Sommerfeld
	/// @date	06.02.2020
	///
	/// @returns	The DSR.
	////////////////////////////////////////////////////////////////////////////////////////////////////

	unsigned char Device6716::readDSR() const;

	////////////////////////////////////////////////////////////////////////////////////////////////////
	/// @fn	void Device6716::setAutoDACPositive(const double voltage) const;
	///
	/// @brief	Sets automatic DACs - positive, for all enabled channels.
	///
	/// @author	Krzysztof Sommerfeld
	/// @date	06.02.2020
	///
	/// @param 	voltage	The voltage.
	////////////////////////////////////////////////////////////////////////////////////////////////////

	void setAutoDACPositive(const double voltage) const;

	////////////////////////////////////////////////////////////////////////////////////////////////////
	/// @fn	void Device6716::setAutoDACNegative(const double voltage) const;
	///
	/// @brief	Sets automatic DACs - negative, for all enabled channels.
	///
	/// @author	Krzysztof Sommerfeld
	/// @date	06.02.2020
	///
	/// @param 	voltage	The voltage.
	////////////////////////////////////////////////////////////////////////////////////////////////////

	void setAutoDACNegative(const double voltage) const;

	////////////////////////////////////////////////////////////////////////////////////////////////////
	/// @fn	std::vector<ViReal64> Device6716::getAutoDACPositive() const;
	///
	/// @brief	Gets automatic DACs - positive, for all enabled channels.
	///
	/// @author	Krzysztof Sommerfeld
	/// @date	06.02.2020
	///
	/// @returns	The automatic DAC positive.
	////////////////////////////////////////////////////////////////////////////////////////////////////

	std::vector<ViReal64> getAutoDACPositive() const;

	////////////////////////////////////////////////////////////////////////////////////////////////////
	/// @fn	std::vector<ViReal64> Device6716::getAutoDACNegative() const;
	///
	/// @brief	Gets automatic DACs - negative, for all enabled channels.
	///
	/// @author	Krzysztof Sommerfeld
	/// @date	06.02.2020
	///
	/// @returns	The automatic DACs - negative.
	////////////////////////////////////////////////////////////////////////////////////////////////////

	std::vector<ViReal64> Device6716::getAutoDACNegative() const;

	////////////////////////////////////////////////////////////////////////////////////////////////////
	/// @fn	bool Device6716::configureVoltageReferanceSwitches(const unsigned char newSegConfRegValue) const;
	///
	/// @brief	Configure voltage referance switches
	///
	/// @author	Krzysztof Sommerfeld
	/// @date	06.02.2020
	///
	/// @param 	newSegConfRegValue	The new segment conf register value.
	///
	/// @returns	True if it succeeds, false if it fails.
	////////////////////////////////////////////////////////////////////////////////////////////////////

	bool configureVoltageReferanceSwitches(const unsigned char newSegConfRegValue) const;

	////////////////////////////////////////////////////////////////////////////////////////////////////
	/// @fn	bool Device6716::isSegment1(const int channelIndex) const noexcept;
	///
	/// @brief	Query if channel with 'channelIndex' is part of segment 1
	///
	/// @author	Krzysztof Sommerfeld
	/// @date	06.02.2020
	///
	/// @param 	channelIndex	Zero-based index of the channel.
	///
	/// @returns	True if segment 1, false if not.
	////////////////////////////////////////////////////////////////////////////////////////////////////

	bool isSegment1(const int channelIndex) const noexcept;

	void saveSubtype(const QString& str) const override;
	void saveSerialnumber(const QString& str) const override;
	void saveFirmwareRevision(const QString& str) const override;
	void saveDriverRevision(const QString& str) const override;
	QString loadSubtype() const override;
	QString loadSerialnumber() const override;
	QString loadFirmwareRevision() const override;
	QString loadDriverRevision() const override;
};
