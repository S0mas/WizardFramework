#pragma once
#include <QString>
#include <QObject>
#include <optional>
#include <string>
#include <visa.h>
#include "ErrorChecker_6716.h"
#include "defines.h"

class ConnectionDetails {
public:
	ViSession viBu6716 = 0;
	ViSession viT028 = 0;
	ViSession viT028Master = 0;
	ViSession viBu3416 = 0;
	ViSession primitiveRmVi6716 = 0;
	ViSession primitiveVi6716 = 0;
	ViSession viBu6100 = 0;

	QString toString() const noexcept {
		QString str = "ConnectionDetails:\n";
		str += "\tbu6716 vi: " + QString::number(viBu6716) + "\n";
		str += "\tt028 vi: " + QString::number(viT028) + "\n";
		str += "\tt028Master vi: " + QString::number(viT028Master) + "\n";
		str += "\tbu3416 vi: " + QString::number(viBu3416) + "\n";
		str += "<------------------------------------------->\n";
		return str;
	}
};

class Communication_6716 : public QObject {
	Q_OBJECT
	ConnectionDetails connectionDetails;
	ErrorChecker_6716 errorChecker;
	bool checkErrorStatus(const QString& content) const;
public:
	template<typename ...Args>
	bool sendCmd(Args&& ...format) const;

	QString readResponse() const;
	bool writeFPGAreg(const unsigned char address, const unsigned char data) const;
	unsigned char readFPGAreg(const unsigned char address) const;
	bool writePortExpander(const unsigned char channel, const unsigned short data) const;
	unsigned short readPortExpander(const unsigned char channel) const;
	void writeI2C_no_addr(const unsigned char i2c_address, const unsigned char data) const;
	unsigned char readI2C_no_addr(const unsigned char i2c_address) const;
	void writeI2C(const unsigned char i2c_address, const unsigned char address, const unsigned char data) const;
	unsigned char readI2C(const unsigned char i2c_address, const unsigned char address) const;
	bool writeTeds2(const unsigned char data) const;
	unsigned char readTeds2() const;
	ViStatus _1wire_commanderXX(ViUInt16* cmds) const;
	ViStatus _1wire_commander(ViUInt16* cmds) const;

	template<typename Function, typename ...Args>
	void callAndThrowOnErrorVisa(Function&& function, const QString& functionName, Args&& ...format) const;
	template<typename Function, typename ...Args>
	void callAndThrowOnError3416(Function&& function, const QString& functionName, Args&& ...format) const;
	template<typename Function, typename ...Args>
	void callAndThrowOnError6716(Function&& function, const QString& functionName, Args&& ...format) const;
	template<typename Function, typename ...Args>
	void callAndThrowOnErrorT028(Function&& function, const QString& functionName, Args&& ...format) const;
	template<typename Function, typename ...Args>
	void callAndThrowOnErrorMaster(Function&& function, const QString& functionName, Args&& ...format) const;
	template<typename Function, typename ...Args>
	void callAndThrowOnError6100(Function&& function, const QString& functionName, Args&& ...format) const;
	void initialize(const QString& ip6100, const QString& ip6716, const int fcNo3416_6716, const int fcNo3416_t028);
	void initializePrimitive(const QString& ip6716);

	ViSession getPrimitiveRmVi6716() const noexcept;
	ViSession getPrimitiveVi6716() const noexcept;
	ViSession getVi6716() const noexcept;
	ViSession getVi3416() const noexcept;
	ViSession getViT028() const noexcept;
	ViSession getViT028Master() const noexcept;
	ViSession getVi6100() const noexcept;

	void setAllVisToInvalid() noexcept;

signals:
	void log(QString msg) const;
};

template<typename ...Args>
bool Communication_6716::sendCmd(Args&& ...format) const {
	char data[1024];
	sprintf(data, format...);
	errorChecker.checkErrorVisa(connectionDetails.viBu6716, viWrite, connectionDetails.viBu6716, reinterpret_cast<ViPByte>(data), strlen(data) + 1, nullptr);
	return checkErrorStatus(QString("Raw SCPI cmd sended: %1").arg(data));
}

template<typename Function, typename ...Args>
void Communication_6716::callAndThrowOnErrorT028(Function&& function, const QString& functionName, Args&& ...format) const {
	errorChecker.checkErrorT028(getViT028(), std::forward<Function>(function), getViT028(), std::forward<Args>(format)...);
	checkErrorStatus(QString("T028 driver function called: %1").arg(functionName));
}

template<typename Function, typename ...Args>
void Communication_6716::callAndThrowOnErrorVisa(Function&& function, const QString& functionName, Args&& ...format) const {
	errorChecker.checkErrorVisa(getVi6716(), std::forward<Function>(function), getVi6716(), std::forward<Args>(format)...);
	checkErrorStatus(QString("Visa driver function called: %1").arg(functionName));
}

template<typename Function, typename ...Args>
void Communication_6716::callAndThrowOnError6716(Function&& function, const QString& functionName, Args&& ...format) const {
	errorChecker.checkError6716(getVi6716(), std::forward<Function>(function), getVi6716(), std::forward<Args>(format)...);
	checkErrorStatus(QString("6716 driver function called: %1").arg(functionName));
}

template<typename Function, typename ...Args>
void Communication_6716::callAndThrowOnError3416(Function&& function, const QString& functionName, Args&& ...format) const {
	errorChecker.checkError3416(getVi3416(), std::forward<Function>(function), getVi3416(), std::forward<Args>(format)...);
	checkErrorStatus(QString("3416 driver function called: %1").arg(functionName));
}

template<typename Function, typename ...Args>
void Communication_6716::callAndThrowOnErrorMaster(Function&& function, const QString& functionName, Args&& ...format) const {
	errorChecker.checkError3416(getViT028Master(), std::forward<Function>(function), getViT028Master(), std::forward<Args>(format)...);
	checkErrorStatus(QString("T028 Master 3416 driver function called: %1").arg(functionName));
}

template<typename Function, typename ...Args>
void Communication_6716::callAndThrowOnError6100(Function&& function, const QString& functionName, Args&& ...format) const {
	errorChecker.checkError6100(getVi6100(), std::forward<Function>(function), getVi6100(), std::forward<Args>(format)...);
	checkErrorStatus(QString("6100 driver function called: %1").arg(functionName));
}
