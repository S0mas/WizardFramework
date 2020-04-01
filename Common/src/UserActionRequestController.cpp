#include "../include/UserActionRequestController.h"

UserActionRequestController::UserActionRequestController(QObject* parent) : LoggingObject(parent) {}

UserActionRequestSender* UserActionRequestController::createSender() noexcept {
	auto sender = new UserActionRequestSender();
	connect(sender, &UserActionRequestSender::requestConfirmation, this, &UserActionRequestController::requestConfirmationReceived);
	connect(sender, &UserActionRequestSender::requestAcceptOrDecline, this, &UserActionRequestController::requestAcceptOrDeclineReceived);
	connect(sender, &UserActionRequestSender::requestChannelsSelection, this, &UserActionRequestController::requestChannelsSelectionReceived);
	connect(sender, &UserActionRequestSender::requestErrorHandlingActionType, this, &UserActionRequestController::requestErrorHandlingActionTypeReceived);

	//// connect to printer
	//connect(&receiver, &UserActionRequestSender::logMsg, &printer, &PrintInterface::addLog);
	return sender;
}

