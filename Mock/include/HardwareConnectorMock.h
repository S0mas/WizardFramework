#pragma once
#include "../../Common/include/AbstractHardwareConnector.h"

////////////////////////////////////////////////////////////////////////////////////////////////////
/// @class	HardwareConnectorMock
///
/// @brief	Mocks the AbstractCHardwareConnector class for testing purpose.
///
/// @author	Krzysztof Sommerfeld
/// @date	06.02.2020
////////////////////////////////////////////////////////////////////////////////////////////////////

class HardwareConnectorMock : public AbstractHardwareConnector {
	std::shared_ptr<AbstractDataResource> ipResource;
	inline static int index = 0;
protected:
	bool isConnectionPossible() const override;
	void connectImpl() noexcept override;
	void disconnectImpl() noexcept override;
public:

	////////////////////////////////////////////////////////////////////////////////////////////////////
	/// @fn	HardwareConnectorMock::HardwareConnectorMock(const QString& nameId, QObject* parent = nullptr) noexcept;
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

	HardwareConnectorMock(const QString& nameId, QObject* parent = nullptr) noexcept;
	~HardwareConnectorMock() override = default;
	std::vector<std::shared_ptr<AbstractDataResource>> resources() const noexcept override;
};
