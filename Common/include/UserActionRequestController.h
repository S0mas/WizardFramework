#pragma once
#include "UserActionRequestSender.h"
#include "LoggingObject.h"

////////////////////////////////////////////////////////////////////////////////////////////////////
/// @class	UserActionRequestController
///
/// @brief	A controller for handling user action requests and is responsible for communication
/// 		between the user and inner application objects.
///
/// @author	Krzysztof Sommerfeld
/// @date	03.02.2020
////////////////////////////////////////////////////////////////////////////////////////////////////

class UserActionRequestController : public LoggingObject {
	Q_OBJECT
public:

	////////////////////////////////////////////////////////////////////////////////////////////////////
	/// @fn	UserActionRequestController::UserActionRequestController(QObject* parent = nullptr);
	///
	/// @brief	Constructor
	///
	/// @author	Krzysztof Sommerfeld
	/// @date	03.02.2020
	///
	/// @param [in]	parent	(Optional) If not null - will be set as constructed object parent. In the
	/// 					QObject tree.
	////////////////////////////////////////////////////////////////////////////////////////////////////

	UserActionRequestController(QObject* parent = nullptr);

	////////////////////////////////////////////////////////////////////////////////////////////////////
	/// @fn	UserActionRequestSender* UserActionRequestController::createSender() noexcept;
	///
	/// @brief	Creates the sender that is connected to this object. All communication from the
	/// 		created sender will be handled by this object. The ownership of the sender object is
	/// 		transferred to the caller.
	///
	/// @author	Krzysztof Sommerfeld
	/// @date	03.02.2020
	///
	/// @returns	New sender object - fully connected to this object.
	////////////////////////////////////////////////////////////////////////////////////////////////////

	UserActionRequestSender* createSender() noexcept;
signals:

	////////////////////////////////////////////////////////////////////////////////////////////////////
	/// @fn	void UserActionRequestController::requestConfirmationReceived(const QString& msg, MyPromiseVoid* barrierPromise) const;
	///
	/// @brief	Forwards request from the sender. This signal should be emitted after sender will
	/// 		emit the requestConfirmation signal.
	///
	/// @author	Krzysztof Sommerfeld
	/// @date	31.01.2020
	///
	/// @param 	   	msg			  	The message of the request.
	/// @param [in]	barrierPromise	The ptr to the promise object that will be used to signal
	/// 							feedback event back to the sender.
	////////////////////////////////////////////////////////////////////////////////////////////////////

	void requestConfirmationReceived(const QString& msg, MyPromiseVoid* barrierPromise) const;

	////////////////////////////////////////////////////////////////////////////////////////////////////
	/// @fn	void UserActionRequestController::requestAcceptOrDeclineReceived(const QString& msg, MyPromiseBool* dataPromise) const;
	///
	/// @brief	Forwards request from the sender. This signal should be emitted after sender will
	/// 		emit the requestAcceptOrDecline signal.
	///
	/// @author	Krzysztof Sommerfeld
	/// @date	31.01.2020
	///
	/// @param 	   	msg		   	The message of the request.
	/// @param [in]	dataPromise	The ptr to the promise object that will be used to send the data back
	/// 						to the sender.
	////////////////////////////////////////////////////////////////////////////////////////////////////

	void requestAcceptOrDeclineReceived(const QString& msg, MyPromiseBool* dataPromise) const;

	////////////////////////////////////////////////////////////////////////////////////////////////////
	/// @fn	void UserActionRequestController::requestChannelsSelectionReceived(const QString& msg, const std::vector<bool>& channelsStatuses, MyPromiseBoolVec* dataPromise) const;
	///
	/// @brief	Forwards request from the sender. This signal should be emitted after sender will
	/// 		emit the requestChannelsSelection signal.
	///
	/// @author	Krzysztof Sommerfeld
	/// @date	31.01.2020
	///
	/// @param 	   	msg					The message of the request.
	/// @param 	   	channelsStatuses	Holds information about the number of channels(size) and
	/// 											which of them should be enabled(true) or disabled(false), the
	/// 											position + 1 is the index of the channel.
	/// @param [in]	dataPromise			The ptr to the promise object that will be used to send the
	/// 								data back to the sender.
	////////////////////////////////////////////////////////////////////////////////////////////////////

	void requestChannelsSelectionReceived(const QString& msg, const std::vector<bool> channelsStatuses, MyPromiseBoolVec* dataPromise) const;

	////////////////////////////////////////////////////////////////////////////////////////////////////
	/// @fn	void UserActionRequestController::requestErrorHandlingActionTypeReceived(const QString& errorMsg, MyPromiseInt* dataPromise) const;
	///
	/// @brief	Forwards request from the sender. This signal should be emitted after sender will
	/// 		emit the requestErrorHandlingActionType signal.
	///
	/// @author	Krzysztof Sommerfeld
	/// @date	31.01.2020
	///
	/// @param 	   	errorMsg   	Message describing the error.
	/// @param [in]	dataPromise	The ptr to the promise object that will be used to give data back to
	/// 						the sender.
	////////////////////////////////////////////////////////////////////////////////////////////////////

	void requestErrorHandlingActionTypeReceived(const QString& errorMsg, MyPromiseInt* dataPromise) const;
};
