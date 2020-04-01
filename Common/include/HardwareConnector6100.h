#pragma once
#include "AbstractHardwareConnector.h"
#include <string>

class HardwareConnector6100 : public AbstractHardwareConnector {
	std::shared_ptr<AbstractDataResource> ipResource;
	std::string resource() const noexcept;
	bool isConnectionPossible() const override;
	void connectImpl() noexcept override;
	void disconnectImpl() noexcept override;
public:
	////////////////////////////////////////////////////////////////////////////////////////////////////
	/// @fn	HardwareConnector6100::HardwareConnector6100(const QString& nameId, QObject* parent = nullptr) noexcept;
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

	HardwareConnector6100(const QString& nameId, QObject* parent = nullptr) noexcept;
	~HardwareConnector6100() override = default;
	std::vector<std::shared_ptr<AbstractDataResource>> resources() const noexcept override;
};