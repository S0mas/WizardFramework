#pragma once
#include "ScpiDevice.h"
#include "ChannelsIF.h"
#include "DeviceIdentityResourcesIF.h"
#include "HardwareConnector6111.h"

////////////////////////////////////////////////////////////////////////////////////////////////////
/// @class	Device6111
///
/// @brief	The virtual representation of the Device6111 device. The only implemented functionality is that used in production tests.
///
/// @author	Krzysztof Sommerfeld
/// @date	06.02.2020
////////////////////////////////////////////////////////////////////////////////////////////////////

class Device6111 : public ScpiDevice, public DeviceIdentityResourcesIF, public ChannelsIF {
	Q_OBJECT
public:
	using DataType = QVector<bool>;
	////////////////////////////////////////////////////////////////////////////////////////////////////
	/// @fn	Device6111::Device6111(const QString& nameId, QObject* parent = nullptr) noexcept;
	///
	/// @brief	Constructor
	///
	/// @author	Krzysztof Sommerfeld
	/// @date	06.02.2020
	///
	/// @param 		   	nameId	The name identifier.
	/// @param [in]	parent	(Optional) If not null - will be set as constructed object parent. In the
	/// 					QObject tree.
	////////////////////////////////////////////////////////////////////////////////////////////////////

	Device6111(const QString& nameId, QObject* parent = nullptr) noexcept : ScpiDevice(nameId, new HardwareConnector6111(nameId), parent), DeviceIdentityResourcesIF(nameId), ChannelsIF(128) {}
	~Device6111() override = default;

	void saveSubtype(const QString& str) const override {}
	void saveSerialnumber(const QString& str) const override {}
	void saveFirmwareRevision(const QString& str) const override {}
	void saveDriverRevision(const QString& str) const override {}
	QString loadSubtype() const override { return ""; }
	QString loadSerialnumber() const override { return ""; }
	QString loadFirmwareRevision() const override { return ""; }
	QString loadDriverRevision() const override { return ""; }


	DataType data(const int channelId) const noexcept {
		return {};
	}
};

