#pragma once
#include "AbstractDevice.h"
#include "ChannelsIF.h"
#include "visa.h"

////////////////////////////////////////////////////////////////////////////////////////////////////
/// @class	DeviceT028
///
/// @brief	The virtual representation of the T028 device. The only implemented functionality is that used in production tests.
///
/// @author	Krzysztof Sommerfeld
/// @date	06.02.2020
////////////////////////////////////////////////////////////////////////////////////////////////////

class DeviceT028 : public AbstractDevice, public ChannelsIF {
	Q_OBJECT
public:
	////////////////////////////////////////////////////////////////////////////////////////////////////
	/// @fn	DeviceT028::DeviceT028(const QString& nameId, const ViSession* masterViSession, QObject* parent = nullptr) noexcept;
	///
	/// @brief	Constructor
	///
	/// @author	Krzysztof Sommerfeld
	/// @date	06.02.2020
	///
	/// @param 		   	nameId		   	The name identifier.
	/// @param 		   	masterViSession	The 3416 master vi session.
	/// @param [in]	parent	(Optional) If not null - will be set as constructed object parent. In the
	/// 					QObject tree.
	////////////////////////////////////////////////////////////////////////////////////////////////////

	DeviceT028(const QString& nameId, const ViSession* masterViSession, QObject* parent = nullptr) noexcept;
	~DeviceT028() override = default;
};

