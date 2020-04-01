#include "../include/UserActionRequestSender.h"
#include "../include/Exceptions.h"

UserActionRequestSender::UserActionRequestSender(QObject* parent) noexcept : QObject(parent) {}

void UserActionRequestSender::waitForErrorHandlingType(const QString& errorMsg) {
	MyPromiseInt promise;
	auto future = promise.future();
	emit requestErrorHandlingActionType(errorMsg, &promise);

	auto data = future.get();
	if (data == 1)
		throw SkipTestException(errorMsg);
	if (data == 2)
		throw StopTestRunException(errorMsg);
}

void UserActionRequestSender::waitForConfirmation(const QString& msg) noexcept {
	MyPromiseVoid promise;
	auto future = promise.future();
	emit requestConfirmation(msg, &promise);
	future.get();
}

bool UserActionRequestSender::waitForAcceptOrDecline(const QString& msg) noexcept {
	MyPromiseBool promise;
	auto future = promise.future();
	emit requestAcceptOrDecline(msg, &promise);
	return future.get();
}

std::vector<bool> UserActionRequestSender::waitForChannelsSelection(const QString& msg, const std::vector<bool> channelsStatuses) noexcept {
	MyPromiseBoolVec promise;
	auto future = promise.future();
	emit requestChannelsSelection(msg, channelsStatuses, &promise);
	return future.get();
}