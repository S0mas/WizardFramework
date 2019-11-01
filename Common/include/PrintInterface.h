#pragma once
#include <QString>
#include <QObject>
#include <QDebug>
#include <QThread>
#include <QTimer>
#include <QMutex>
#include <QMutexLocker>

class PrintInterface : public QObject {
	Q_OBJECT
	mutable QString block;
	mutable QMutex m;
public:
	PrintInterface() {
		QTimer* timer = new QTimer(this);
		connect(timer, &QTimer::timeout, this, &PrintInterface::sendBlock);
		timer->start(10);
	}

	template<typename ...Args>
	void printf(const char* str, Args&& ...args) const noexcept {
		char buffer[1024];
		sprintf(buffer, str, args...);
		addToBlock(buffer);
	}

	void printf(const QString& str) const noexcept {
		addToBlock(str);
	}

	void sendBlock() {
		QMutexLocker locker(&m);
		if (!block.isEmpty()) {
			emit printLog(block);
			block.clear();
		}
	}

	void addToBlock(const QString& msg) const noexcept {
		QMutexLocker locker(&m);
		block += msg;
	}
signals:
	void printLog(const QString& msg) const;
};