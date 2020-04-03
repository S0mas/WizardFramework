#pragma once
#include "Device6991.h"

////////////////////////////////////////////////////////////////////////////////////////////////////
/// @class	Device6111
///
/// @brief	The virtual representation of the Device6111 device. The only implemented functionality is that used in production tests.
///
/// @author	Krzysztof Sommerfeld
/// @date	06.02.2020
////////////////////////////////////////////////////////////////////////////////////////////////////

class Device6111 : public Device6991 {
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

	Device6111(const QString& nameId, ScpiIF* scpiIF, QObject* parent = nullptr) noexcept : Device6991(nameId, nullptr, scpiIF, 256, parent) {}
	~Device6111() override = default;
};

