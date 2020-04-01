#pragma once
#include "AbstractHardwareConnector.h"
#include "Resource.h"
#include <string>

class HardwareConnector6111 : public AbstractHardwareConnector {
	std::string resource() const noexcept { return ""; }
	bool isConnectionPossible() const override { return true; }
	void connectImpl() noexcept override {}
	void disconnectImpl() noexcept override {}
public:
	////////////////////////////////////////////////////////////////////////////////////////////////////
	/// @fn	HardwareConnector6111::HardwareConnector6111(const QString& nameId, QObject* parent = nullptr) noexcept;
	///
	/// @brief	Constructor
	///
	/// @author	Krzysztof Sommerfeld
	/// @date	05.02.2020
	///
	/// @param 		   	nameId			 	The name identifier.
	/// @param [in]	parent	(Optional) If not null - will be set as constructed object parent. In the
	/// 					QObject tree.
	////////////////////////////////////////////////////////////////////////////////////////////////////

	HardwareConnector6111(const QString& nameId, QObject* parent = nullptr) noexcept {}
	~HardwareConnector6111() override = default;
	std::vector<std::shared_ptr<AbstractDataResource>> resources() const noexcept override { return {}; }
};