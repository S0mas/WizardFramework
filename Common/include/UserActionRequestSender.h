#pragma once
#include "Promises.h"

class UserActionRequestController;

////////////////////////////////////////////////////////////////////////////////////////////////////
/// @class	UserActionRequestSender
///
/// @brief	It can only be created by the UserActionRequestController. Sends requests to the
/// 		UserActionRequestController object(creator). Also, allows to read the feedback data
/// 		using promises objects.
///
/// @author	Krzysztof Sommerfeld
/// @date	31.01.2020
////////////////////////////////////////////////////////////////////////////////////////////////////

class UserActionRequestSender : public QObject {
	Q_OBJECT
	friend UserActionRequestController;

	////////////////////////////////////////////////////////////////////////////////////////////////////
	/// @fn	UserActionRequestSender::UserActionRequestSender(QObject* parent = nullptr) noexcept;
	///
	/// @brief	Constructor - it is private. Only UserActionRequestController class can create it(that
	/// 		is why it is a friend).
	///
	/// @author	Krzysztof Sommerfeld
	/// @date	31.01.2020
	///
	/// @param [in]	parent	(Optional) If not null - will be set as constructed object parent. In the
	/// 					QObject tree.
	////////////////////////////////////////////////////////////////////////////////////////////////////

	UserActionRequestSender(QObject* parent = nullptr) noexcept;
public:
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// @fn	UserActionRequestSender::~UserActionRequestSender() override = default;
    ///
    /// @brief	Default destructor
    ///
    /// @author	Krzysztof Sommerfeld
    /// @date	31.01.2020
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    ~UserActionRequestSender() override = default;

	////////////////////////////////////////////////////////////////////////////////////////////////////
	/// @fn	void UserActionRequestSender::waitForConfirmation(const QString& msg) noexcept;
	///
	/// @brief	Sends requestConfirmation and waits for the waking up trigger from the creator.
	///
	/// @author	Krzysztof Sommerfeld
	/// @date	31.01.2020
	///
	/// @param 	msg	The message describing the request.
	////////////////////////////////////////////////////////////////////////////////////////////////////

	void waitForConfirmation(const QString& msg) noexcept;

	////////////////////////////////////////////////////////////////////////////////////////////////////
	/// @fn	bool UserActionRequestSender::waitForAcceptOrDecline(const QString& msg) noexcept;
	///
	/// @brief	Sends requestAcceptOrDecline and waits for the waking up trigger from the creator.
	///
	/// @author	Krzysztof Sommerfeld
	/// @date	31.01.2020
	///
	/// @param 	msg	The message describing the request.
	///
	/// @returns	The user decision - accepted(true), declined(false).
	////////////////////////////////////////////////////////////////////////////////////////////////////

	bool waitForAcceptOrDecline(const QString& msg) noexcept;

	////////////////////////////////////////////////////////////////////////////////////////////////////
	/// @fn	std::vector<bool> UserActionRequestSender::waitForChannelsSelection(const QString& msg, const std::vector<bool>& channelsStatuses) noexcept;
	///
	/// @brief	Sends requestChannelsSelection and waits for the waking up trigger from the creator.
	/// 		The enabledChannelsMask param will be stored into shared object.
	///
	/// @author	Krzysztof Sommerfeld
	/// @date	31.01.2020
	///
	/// @param 	msg					The message describing the request.
	/// @param 	channelsStatuses	Holds information about the number of channels(size) and
	/// 										which of them should be enabled(true) or disabled(false), the
	/// 										position + 1 is the index of the channel.
	///
	/// @returns	Holds information about the number of channels(size) and which of them are
	/// 			selected(true) or not selected(false).
	////////////////////////////////////////////////////////////////////////////////////////////////////

	std::vector<bool> waitForChannelsSelection(const QString& msg, const std::vector<bool> channelsStatuses) noexcept;

	////////////////////////////////////////////////////////////////////////////////////////////////////
	/// @fn	void UserActionRequestSender::waitForErrorHandlingType(const QString& errorMsg);
	///
	/// @brief	Sends requestErrorHandlingActionType and waits for the waking up trigger from the
	/// 		creator. The options are:
	/// 		-ignore (no action)
	/// 		-skip test (throws exception)
	/// 		-skip testsrun (throws exception)
	///
	/// @author	Krzysztof Sommerfeld
	/// @date	31.01.2020
	///
	/// @param 	errorMsg	The message describing the error.
	////////////////////////////////////////////////////////////////////////////////////////////////////

	void waitForErrorHandlingType(const QString& errorMsg);
signals:

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// @fn	void UserActionRequestSender::requestConfirmation(const QString& msg, MyPromiseVoid* barrierPromise) const;
    ///
    /// @brief	Requests the confirmation action. It doesn't expect any data. Triggers creator's
    /// 		correspondent signal.
    ///
    /// @author	Krzysztof Sommerfeld
    /// @date	31.01.2020
    ///
    /// @param 		   	msg			  	The message describing the request.
    /// @param [in]		barrierPromise	The ptr to the promise object that will be used to command continue of execution by the creator.
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    void requestConfirmation(const QString& msg, MyPromiseVoid* barrierPromise) const;

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// @fn	void UserActionRequestSender::requestAcceptOrDecline(const QString& msg, MyPromiseBool* dataPromise) const;
    ///
    /// @brief	Requests the acceptance or rejection action. Triggers creator's correspondent signal.
    ///
    /// @author	Krzysztof Sommerfeld
    /// @date	31.01.2020
    ///
    /// @param 		   	msg		   	The message describing the request.
    /// @param [in]		dataPromise	The ptr to the promise object that will be used to retrieve data from the creator.
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    void requestAcceptOrDecline(const QString& msg, MyPromiseBool* dataPromise) const;

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// @fn	void UserActionRequestSender::requestChannelsSelection(const QString& msg, const std::vector<bool>& channelsStatuses, MyPromiseBoolVec* dataPromise) const;
    ///
    /// @brief	Requests the selection between the enabled channels. Triggers creator's correspondent
    /// 		signal.
    ///
    /// @author	Krzysztof Sommerfeld
    /// @date	31.01.2020
    ///
    /// @param 	   	msg					The message describing the request.
    /// @param 	   	channelsStatuses	Holds information about the number of channels(size) and
    /// 											which of them should be enabled(true) or disabled(false), the
    /// 											position + 1 is the index of the channel.
    /// @param [in]	dataPromise			The ptr to the promise object that will be used to retrieve
    /// 								data from the creator.
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    void requestChannelsSelection(const QString& msg, const std::vector<bool> channelsStatuses, MyPromiseBoolVec* dataPromise) const;

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// @fn	void UserActionRequestSender::requestErrorHandlingActionType(const QString& errorMsg, MyPromiseInt* dataPromise) const;
    ///
    /// @brief	Requests the information, how the active error should be handled. Triggers creator's
    /// 		correspondent signal.
    ///
    /// @author	Krzysztof Sommerfeld
    /// @date	31.01.2020
    ///
    /// @param 		   	errorMsg   	The message describing the error.
    /// @param [in]		dataPromise	The ptr to the promise object that will be used to retrieve data from the creator.
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    void requestErrorHandlingActionType(const QString& errorMsg, MyPromiseInt* dataPromise) const;
};

