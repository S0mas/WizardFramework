#pragma once
#include "AbstractDevice.h"
#include "DeviceIdentityResourcesIF.h"
#include "ChannelsIF.h"
#include "../../../visa/include/visa.h"

////////////////////////////////////////////////////////////////////////////////////////////////////
/// @class	Device3416
///
/// @brief	The virtual representation of 3416 device. The only implemented functionality is that used in production tests.
///
/// @author	Krzysztof Sommerfeld
/// @date	05.02.2020
////////////////////////////////////////////////////////////////////////////////////////////////////

class Device3416 : public AbstractDevice, public DeviceIdentityResourcesIF, public ChannelsIF {
	Q_OBJECT
public:
	////////////////////////////////////////////////////////////////////////////////////////////////////
	/// @fn	Device3416::Device3416(const QString& nameId, const QString& motherBoardNameId, QObject* parent = nullptr) noexcept;
	///
	/// @brief	Constructor
	///
	/// @author	Krzysztof Sommerfeld
	/// @date	05.02.2020
	///
	/// @param 		   	nameId			 	The name identifier.
	/// @param 		   	motherBoardNameId	Identifier for the mother board name.
	/// @param [in]	parent	(Optional) If not null - will be set as constructed object parent. In the
	/// 					QObject tree.
	////////////////////////////////////////////////////////////////////////////////////////////////////

	Device3416(const QString& nameId, const QString& motherBoardNameId, QObject* parent = nullptr) noexcept;
	~Device3416() override = default;

	void saveSubtype(const QString& str) const override;
	void saveSerialnumber(const QString& str) const override;
	void saveFirmwareRevision(const QString& str) const override;
	void saveDriverRevision(const QString& str) const override;
	QString loadSubtype() const override;
	QString loadSerialnumber() const override;
	QString loadFirmwareRevision() const override;
	QString loadDriverRevision() const override;

	////////////////////////////////////////////////////////////////////////////////////////////////////
	/// @fn	const ViSession& Device3416::refereceToViSession() const noexcept;
	///
	/// @brief	Referece to vi session - it used for devices that consider this object as ther master.
	///
	/// @author	Krzysztof Sommerfeld
	/// @date	05.02.2020
	///
	/// @returns	The const reference to a the vi session.
	////////////////////////////////////////////////////////////////////////////////////////////////////

	const ViSession& refereceToViSession() const noexcept;

	////////////////////////////////////////////////////////////////////////////////////////////////////
	/// @fn	std::vector<double> Device3416::measureChannels(const unsigned channelMask, const int gain = 1, const double scanRate = 5000, const uint64_t scanCount = 5000) const;
	///
	/// @brief	Measures all enabled channels.
	///
	/// @author	Krzysztof Sommerfeld
	/// @date	05.02.2020
	///
	/// @param 	channelMask	The channel mask.
	/// @param 	gain	   	(Optional) The gain.
	/// @param 	scanRate   	(Optional) The scan rate.
	/// @param 	scanCount  	(Optional) Number of scans.
	///
	/// @returns	The measured values.
	////////////////////////////////////////////////////////////////////////////////////////////////////

	std::vector<double> measureChannels(const unsigned channelMask, const int gain = 1, const double scanRate = 5000, const uint64_t scanCount = 5000) const;

	////////////////////////////////////////////////////////////////////////////////////////////////////
	/// @fn	double Device3416::measureChannel(const int channelIndex, const int gain = 1, const uint64_t scanRate = 5000, const uint64_t scanCount = 5000) const;
	///
	/// @brief	Measures selected channel.
	///
	/// @author	Krzysztof Sommerfeld
	/// @date	05.02.2020
	///
	/// @param 	channelIndex	The index of the channel.
	/// @param 	gain			(Optional) The gain.
	/// @param 	scanRate		(Optional) The scan rate.
	/// @param 	scanCount   	(Optional) Number of scans.
	///
	/// @returns	The measured value.
	////////////////////////////////////////////////////////////////////////////////////////////////////

	double measureChannel(const int channelIndex, const int gain = 1, const uint64_t scanRate = 5000, const uint64_t scanCount = 5000) const;
};

