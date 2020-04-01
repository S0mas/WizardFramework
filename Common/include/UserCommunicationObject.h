#pragma once
#include "UserActionRequestSender.h"

////////////////////////////////////////////////////////////////////////////////////////////////////
/// @class	UserCommunicationObject
///
/// @brief	Class that should be inherited by objects that needs to communicate with the user.
///
/// @author	Krzysztof Sommerfeld
/// @date	03.02.2020
////////////////////////////////////////////////////////////////////////////////////////////////////

class UserCommunicationObject : public QObject {
protected:
	UserActionRequestSender* sender_ = nullptr;
public:

	////////////////////////////////////////////////////////////////////////////////////////////////////
	/// @fn	UserCommunicationObject::UserCommunicationObject(QObject* parent = nullptr)
	///
	/// @brief	Constructor
	///
	/// @author	Krzysztof Sommerfeld
	/// @date	03.02.2020
	///
	/// @param [in]	parent	(Optional) If not null - will be set as constructed object parent. In the
	/// 					QObject tree.
	////////////////////////////////////////////////////////////////////////////////////////////////////

	UserCommunicationObject(QObject* parent = nullptr) : QObject(parent) {}

	////////////////////////////////////////////////////////////////////////////////////////////////////
	/// @fn	void UserCommunicationObject::setSender(UserActionRequestSender* sender) noexcept
	///
	/// @brief	Sets a sender
	///
	/// @author	Krzysztof Sommerfeld
	/// @date	03.02.2020
	///
	/// @param [in]	sender	Asigns the sender object and takes the ownership of it.
	////////////////////////////////////////////////////////////////////////////////////////////////////

	void setSender(UserActionRequestSender* sender) noexcept {
		sender_ = sender;
		sender_->setParent(this);
	}
};