#pragma once
#include "AbstractHardwareConnector.h"
#include "Resource.h"
#include <visa.h>
#include <string>

class HardwareConnector3416 : public AbstractHardwareConnector {
	std::shared_ptr<AbstractDataResource> ipResource;
	std::shared_ptr<AbstractDataResource> fcIdResource;
	std::string resource() const noexcept;

	////////////////////////////////////////////////////////////////////////////////////////////////////
	/// @fn	int HardwareConnector3416::cardId() const noexcept;
	///
	/// @brief	Gets the 3416 function card identifier.
	///
	/// @author	Krzysztof Sommerfeld
	/// @date	05.02.2020
	///
	/// @returns	The 3416 function card identifier.
	////////////////////////////////////////////////////////////////////////////////////////////////////

	int cardId() const noexcept;
	bool isConnectionPossible() const override;
	void connectImpl() noexcept override;
	void disconnectImpl() noexcept override;
public:
	////////////////////////////////////////////////////////////////////////////////////////////////////
	/// @fn	HardwareConnector3416::HardwareConnector3416(const QString& nameId, const QString& motherBoardNameId, QObject* parent = nullptr) noexcept;
	///
	/// @brief	Constructor
	///
	/// @author	Krzysztof Sommerfeld
	/// @date	05.02.2020
	///
	/// @param 		   	nameId			 	The name identifier.
	/// @param 		   	motherBoardNameId	The name identifier for the mother board.
	/// @param [in]	parent	(Optional) If not null - will be set as constructed object parent. In the
	/// 					QObject tree.
	////////////////////////////////////////////////////////////////////////////////////////////////////

	HardwareConnector3416(const QString& nameId, const QString& motherBoardNameId, QObject* parent = nullptr) noexcept;
	~HardwareConnector3416() override = default;

	////////////////////////////////////////////////////////////////////////////////////////////////////
	/// @fn	const ViSession& HardwareConnector3416::refereceToViSession() const noexcept;
	///
	/// @brief	Referece to vi session
	///
	/// @author	Krzysztof Sommerfeld
	/// @date	05.02.2020
	///
	/// @returns	A reference to a const ViSession.
	////////////////////////////////////////////////////////////////////////////////////////////////////

	const ViSession& refereceToViSession() const noexcept;
	std::vector<std::shared_ptr<AbstractDataResource>> resources() const noexcept override;
};