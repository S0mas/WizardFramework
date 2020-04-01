#pragma once
#include "../../Common/include/AbstractDevice.h"
#include "../../Common/include/DeviceIdentityResourcesIF.h"
#include "../../Common/include/ChannelsIF.h"
#include "HardwareConnectorMock.h"

////////////////////////////////////////////////////////////////////////////////////////////////////
/// @class	DeviceMock
///
/// @brief	Mocks the AbstractDevice class for the testing purpose.
///
/// @author	Krzysztof Sommerfeld
/// @date	06.02.2020
////////////////////////////////////////////////////////////////////////////////////////////////////

class DeviceMock : public AbstractDevice, public DeviceIdentityResourcesIF, public ChannelsIF {
	Q_OBJECT
	void saveSerialnumber(const QString& str) const override;
	void saveSubtype(const QString& str) const override;
	void saveFirmwareRevision(const QString& str) const override;
	void saveDriverRevision(const QString& str) const override;

	QString loadSerialnumber() const override;
	QString loadSubtype() const override;
	QString loadFirmwareRevision() const override;
	QString loadDriverRevision() const override;
public:

	////////////////////////////////////////////////////////////////////////////////////////////////////
	/// @fn	DeviceMock::DeviceMock(const QString& nameId, QObject* parent = nullptr) noexcept;
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

	DeviceMock(const QString& nameId, QObject* parent = nullptr) noexcept;
	~DeviceMock() override = default;
};
