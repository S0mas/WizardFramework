#pragma once
#include <QString>
#include <QObject>
#include <QThread>
#include <thread>
#include <chrono>
#include <QCoreApplication>

class PrintInterface : public QObject {
	Q_OBJECT
	mutable QString logs;
public:
	PrintInterface() {}

	void printingProcess() const noexcept {
		while (!QThread::currentThread()->isInterruptionRequested()) {
			std::this_thread::sleep_for(std::chrono::milliseconds(100));
			if (!logs.isEmpty()) {
				emit printLog(logs);
				logs.clear();
			}
			QCoreApplication::processEvents(QEventLoop::AllEvents, 50);
		}
	}
public slots:
	void addLog(const QString& msg) const noexcept {
		logs += msg;
	}
signals:
	void printLog(const QString& msg) const;
};