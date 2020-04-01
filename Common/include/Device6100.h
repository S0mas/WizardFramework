#pragma once
#include "ScpiDevice.h"
#include "DeviceIdentityResourcesIF.h"

////////////////////////////////////////////////////////////////////////////////////////////////////
/// @class	Device6100
///
/// @brief	The virtual representation of the 6100 device. The only implemented functionality is that used in production tests.
///
/// @author	Krzysztof Sommerfeld
/// @date	06.02.2020
////////////////////////////////////////////////////////////////////////////////////////////////////

class Device6100 : public ScpiDevice, public DeviceIdentityResourcesIF {
	Q_OBJECT
public:
	////////////////////////////////////////////////////////////////////////////////////////////////////
	/// @fn	Device6100::Device6100(const QString& nameId, QObject* parent = nullptr) noexcept;
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

	Device6100(const QString& nameId, QObject* parent = nullptr) noexcept;
	~Device6100() override = default;

	void saveSubtype(const QString& str) const override;
	void saveSerialnumber(const QString& str) const override;
	void saveFirmwareRevision(const QString& str) const override;
	void saveDriverRevision(const QString& str) const override;
	QString loadSubtype() const override;
	QString loadSerialnumber() const override;
	QString loadFirmwareRevision() const override;
	QString loadDriverRevision() const override;
};

