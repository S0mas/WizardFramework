#pragma once
#include "AbstractHardwareConnector.h"
#include <visa.h>

class HardwareConnectorT028 : public AbstractHardwareConnector {
	const ViSession* masterViSession = nullptr;
	bool isConnectionPossible() const override;
	void connectImpl() noexcept override;
	void disconnectImpl() noexcept override;
public:
	////////////////////////////////////////////////////////////////////////////////////////////////////
	/// @fn	HardwareConnectorT028::HardwareConnectorT028(const ViSession* masterViSession, QObject* parent = nullptr) noexcept;
	///
	/// @brief	Constructor
	///
	/// @author	Krzysztof Sommerfeld
	/// @date	06.02.2020
	///
	/// @param 		   	masterViSession	The master vi session.
	/// @param [in]	parent	(Optional) If not null - will be set as constructed object parent. In the
	/// 					QObject tree.
	////////////////////////////////////////////////////////////////////////////////////////////////////

	HardwareConnectorT028(const ViSession* masterViSession, QObject* parent = nullptr) noexcept;
	~HardwareConnectorT028() override = default;
	std::vector<std::shared_ptr<AbstractDataResource>> resources() const noexcept override;
};