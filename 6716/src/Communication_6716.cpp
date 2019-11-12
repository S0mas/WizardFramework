#include "../include/Communication_6716.h"
#include <bu6716.h>
#include <t028.h>
#include <bu3416.h>
#include <bu6100.h>

QString Communication_6716::readResponse() const {
	char data[1024];
	ViUInt32 retSize = 0;
	errorChecker.checkErrorVisa(connectionDetails.viBu6716, viRead, connectionDetails.viBu6716, reinterpret_cast<ViPByte>(data), sizeof(data) - 1, &retSize);
	data[retSize] = '\0';
	checkErrorStatus(QString("Data read: %1").arg(data));
	return data;
}

bool Communication_6716::writeFPGAreg(const unsigned char address, const unsigned char data) const {
	return sendCmd("BINT:MAIN:FPGA #h%02hhx, #h%02hhx\n", address, data);
}

unsigned char Communication_6716::readFPGAreg(const unsigned char address) const {
	sendCmd("BINT:MAIN:FPGA? #h%hhx\n", address);
	return readResponse().toUInt(nullptr, 16);
}

bool Communication_6716::writePortExpander(const unsigned char channel, const unsigned short data) const {
	return sendCmd("input%d:smax7318aug #h%04hx\n", channel, data);
}

unsigned short Communication_6716::readPortExpander(const unsigned char channel) const {
	sendCmd("input%d:smax7318aug?\n", channel);
	return readResponse().toUInt(nullptr, 16);
}

bool Communication_6716::writeTeds2(const unsigned char data) const {
	ViReal64 startTime = bu3100_getStartTime();
	unsigned char statusByte;
	writeI2C(0x19, bu6716_TEDS_CMD_1WWB, data);
	do {
		if (bu3100_getStartTime() - startTime > 1000.0) {
			log(QString("%1: Timeout\n").arg(__FUNCTION__));
			return false;
		}
		statusByte = readI2C_no_addr(0x19);
	} while (statusByte & bu6716_TEDS_STATUS_1WB);
	return true;
}

unsigned char Communication_6716::readTeds2() const {
	ViReal64 startTime = bu3100_getStartTime();
	unsigned char statusByte;
	writeI2C_no_addr(0x19, bu6716_TEDS_CMD_1WRB);
	do {
		if (bu3100_getStartTime() - startTime > 1000.0) {
			log(QString("%1: Timeout\n").arg(__FUNCTION__));
			throw std::runtime_error("Error teds class 2 timeout!");
		}
		statusByte = readI2C_no_addr(0x19);
	} while (statusByte & bu6716_TEDS_STATUS_1WB);
	writeI2C(0x19, bu6716_TEDS_CMD_SRP, bu6716_TEDS_REG_DATA);
	return readI2C_no_addr(0x19);
}

ViStatus Communication_6716::_1wire_commander(ViUInt16* cmds) const {
	ViStatus status = VI_SUCCESS;
	ViUInt32 i = 0;
	while (cmds[i]) {
		if (cmds[i] & 0x8000) {
			// command pause
			bu3100_sleep(cmds[i] & 0x00FF);
		}
		else {
			// send cmd
			writeFPGAreg(bu6716_FPGA_TEDS_DATA, cmds[i] & 0xff);
			writeFPGAreg(bu6716_FPGA_TEDS_ACC, (cmds[i] & 0xff00) >> 8);
			bu3100_sleep(10);
			ViUInt8 data_l = readFPGAreg(bu6716_FPGA_TEDS_DATA);
			ViUInt8 data_h = readFPGAreg(bu6716_FPGA_TEDS_ACC);
			ViInt16 val16 = (data_h << 8) | data_l;

			// ready ?
			if (!(val16 & bu3416_TEDS_READY))
				return bu3416_TEDS_NOTREADY;
			// after reset check the 'present' bit
			if ((cmds[i] & bu3416_TEDS_CMD_MASK) == bu3416_TEDS_CMD_RESET) {
				if (!(val16 & bu3416_TEDS_PRESENT))
					return bu3416_TEDS_NOTPRESENT;
			}
			// read
			if ((cmds[i] & bu3416_TEDS_CMD_MASK) == bu3416_TEDS_CMD_READ)
				cmds[i] = val16;
		}
		i++;
	}
	return status;
}

void Communication_6716::initialize(const QString& ip6100, const QString& ip6716, const int fcNo3416_6716, const int fcNo3416_t028) {
	if (connectionDetails.viBu6100 == 0) {
		errorChecker.checkError6100(0, bu6100_init, ip6100.toStdString().data(), VI_TRUE, VI_FALSE, &connectionDetails.viBu6100);
		checkErrorStatus(QString("CMD: %1").arg("bu6100_init"));
	}
	if (connectionDetails.viBu6716 == 0) {
		errorChecker.checkError6716(0, bu6716_init, ip6716.toStdString().data(), true, true, &connectionDetails.viBu6716);
		checkErrorStatus(QString("CMD: %1").arg("bu6716_init"));
	}
	if (connectionDetails.viBu3416 == 0) {
		errorChecker.checkError3416(0, bu3416_paramInit, ip6100.toStdString().data(), fcNo3416_6716, true, false, &connectionDetails.viBu3416);
		checkErrorStatus(QString("CMD: %1").arg("bu3416_paramInit"));
	}	
	if (connectionDetails.viT028Master == 0) {
		errorChecker.checkError3416(0, bu3416_paramInit, ip6100.toStdString().data(), fcNo3416_t028, VI_TRUE, VI_FALSE, &connectionDetails.viT028Master);
		checkErrorStatus(QString("CMD: %1").arg("bu3416_paramInit"));
	}
	if (connectionDetails.viT028 == 0) {
		errorChecker.checkErrorT028(0, t028_init, connectionDetails.viT028Master, VI_TRUE, VI_TRUE, &connectionDetails.viT028);
		checkErrorStatus(QString("CMD: %1").arg("t028_init"));
	}
	log(connectionDetails.toString());
}

void Communication_6716::initializePrimitive(const QString& ip6716) {
	//errorChecker.checkErrorVisa(0, viOpenDefaultRM, &connectionDetails.primitiveRmVi6716);
	//checkErrorStatus(QString("CMD: %1").arg("viOpenDefaultRM"));
	//errorChecker.checkErrorVisa(0, viOpen, connectionDetails.primitiveRmVi6716, ip6716.toStdString().data(), BU_NULL, BU_NULL, &connectionDetails.primitiveVi6716);
	//checkErrorStatus(QString("CMD: %1").arg("viOpen"));
}

bool Communication_6716::checkErrorStatus(const QString& content) const {
	if (errorChecker.getLastStatus().statusType() == StatusType::ERR) {
		log(QString("%1 ---> status msg: %2\n").arg(content).arg(errorChecker.getLastStatus().toString()));
		throw std::runtime_error("Error occured! Exception thrown.\n");
	}

	return true;
}

ViSession Communication_6716::getPrimitiveRmVi6716() const noexcept {
	return connectionDetails.primitiveRmVi6716;
}

ViSession Communication_6716::getPrimitiveVi6716() const noexcept {
	return connectionDetails.primitiveVi6716;
}

ViSession Communication_6716::getVi6716() const noexcept {
	return connectionDetails.viBu6716;
}

ViSession Communication_6716::getVi3416() const noexcept {
	return connectionDetails.viBu3416;
}

ViSession Communication_6716::getViT028() const noexcept {
	return connectionDetails.viT028;
}

ViSession Communication_6716::getViT028Master() const noexcept {
	return connectionDetails.viT028Master;
}

ViSession Communication_6716::getVi6100() const noexcept {
	return connectionDetails.viBu6100;
}

void Communication_6716::setAllVisToInvalid() noexcept {
	connectionDetails.viBu6716 = 0;
	connectionDetails.viT028 = 0;
	connectionDetails.viT028Master = 0;
	connectionDetails.viBu3416 = 0;
	connectionDetails.primitiveRmVi6716 = 0;
	connectionDetails.primitiveVi6716 = 0;
	connectionDetails.viBu6100 = 0;
}

void Communication_6716::writeI2C_no_addr(const unsigned char i2c_address, const unsigned char data) const {
	sendCmd("BINT:MAIN:ICNA #h%02hhx, #h%02hhx\n", i2c_address, data);
}

unsigned char Communication_6716::readI2C_no_addr(const unsigned char i2c_address) const {
	sendCmd("BINT:MAIN:ICNA? #h%hhx\n", i2c_address);
	return readResponse().toUInt(nullptr, 16);
}

void Communication_6716::writeI2C(const unsigned char i2c_address, const unsigned char address, const unsigned char data) const {
	sendCmd("BINT:MAIN:IC #h%02hhx, #h%02hhx, #h%02hhx\n", i2c_address, address, data);
}

unsigned char Communication_6716::readI2C(const unsigned char i2c_address, const unsigned char address) const {
	sendCmd("BINT:MAIN:ICNA? #h%hhx, #h%02hhx\n", i2c_address, address);
	return readResponse().toUInt(nullptr, 16);
}