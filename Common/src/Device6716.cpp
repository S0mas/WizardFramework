#include "../include/Device6716.h"
#include "bu3100.h"
#include "bu3416.h"
#include "../include/HardwareConnector6716.h"
#include "../include/ScpiIFVisa.h"

Device6716::Device6716(const QString& nameId, QObject* parent) noexcept : ScpiDevice(nameId, new HardwareConnector6716(nameId), new ScpiIFVisa, parent), DeviceIdentityResourcesIF(nameId), ChannelsIF(16) {
}

bool Device6716::writeFPGAreg(const unsigned char address, const unsigned char data) const {
	return invokeCmd(QString("BINT:MAIN:FPGA #h%1, #h%2").arg(QString::number(address, 16), QString::number(data, 16)));
}

unsigned char Device6716::readFPGAreg(const unsigned char address) const {
	auto resp = invokeQuery(QString("BINT:MAIN:FPGA? #h%1").arg(QString::number(address, 16)));
	if(resp)
		return resp->toUInt(nullptr, 16);
	emit reportError("Couldnt read register! 0xFF returned");
	return 0xFF;
}

bool Device6716::writePortExpander(const int channelIndex, const unsigned short data) const {
	return invokeCmd(QString("input%1:smax7318aug #h%2").arg(QString::number(channelIndex), QString::number(data, 16)));
}

unsigned short Device6716::readPortExpander(const int channelIndex) const {
	auto resp = invokeQuery(QString("input%1:smax7318aug?").arg(QString::number(channelIndex)));
	if (resp)
		return resp->toUInt(nullptr, 16);
	emit reportError("Couldnt read register! 0xFFFF returned");
	return 0xFFFF;
}

bool Device6716::writeTeds2(const unsigned char data) const {
	ViReal64 startTime = bu3100_getStartTime();
	unsigned char statusByte;
	writeI2c(0x19, bu6716_TEDS_CMD_1WWB, data);
	bu3100_sleep(50);
	do {
		if (bu3100_getStartTime() - startTime > 1000.0) {
			throw std::runtime_error("Error teds class 2 write timeout!");
		}
		statusByte = readI2cNoAddress(0x19);
	} while (statusByte & bu6716_TEDS_STATUS_1WB);
	bu3100_sleep(50);
	return true;
}

unsigned char Device6716::readTeds2() const {
	ViReal64 startTime = bu3100_getStartTime();
	unsigned char statusByte;
	writeI2cNoAddress(0x19, bu6716_TEDS_CMD_1WRB);
	bu3100_sleep(50);
	do {
		if (bu3100_getStartTime() - startTime > 1000.0) {
			throw std::runtime_error("Error teds class 2 read timeout!");
		}
		statusByte = readI2cNoAddress(0x19);
	} while (statusByte & bu6716_TEDS_STATUS_1WB);
	bu3100_sleep(50);
	writeI2c(0x19, bu6716_TEDS_CMD_SRP, bu6716_TEDS_REG_DATA);
	bu3100_sleep(50);
	return readI2cNoAddress(0x19);
}

ViStatus Device6716::oneWireCommander(unsigned short* cmds) const {
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

void Device6716::writeI2cNoAddress(const unsigned char i2c_address, const unsigned char data) const {
	invokeCmd(QString("BINT:MAIN:ICNA #h%1, #h%2").arg(QString::number(i2c_address, 16), QString::number(data, 16)));
}

unsigned char Device6716::readI2cNoAddress(const unsigned char i2c_address) const {
	auto i2c = invokeQuery(QString("BINT:MAIN:ICNA? #h%1").arg(i2c_address));
	if (i2c)
		return i2c->toUInt(nullptr, 16);
	emit reportError("Couldnt read I2C, 0xFF returned.");
	return 0xFF;
}

void Device6716::writeI2c(const unsigned char i2c_address, const unsigned char address, const unsigned char data) const {
	invokeCmd(QString("BINT:MAIN:IC #h%1, #h%2, #h%3").arg(QString::number(i2c_address, 16), QString::number(address, 16), QString::number(data, 16)));
}

unsigned char Device6716::readI2c(const unsigned char i2c_address, const unsigned char address) const {
	auto i2c = invokeQuery(QString("BINT:MAIN:ICNA? #h%1, #h%2").arg(QString::number(i2c_address, 16), QString::number(address, 16)));
	if (i2c)
		return i2c->toUInt(nullptr, 16);
	emit reportError("Couldnt read I2C, 0xFF returned.");
	return 0xFF;
}

unsigned char Device6716::readDSR() const {
	auto dsr = invokeQuery(QString("*DSR?"));
	if(dsr)
		return dsr->toUInt(nullptr, 16);
	emit reportError("Couldnt read DSR, 0xFF returned.");
	return 0xFF;
}

void Device6716::setAutoDAC(const double voltage, const short select) const {
	for (auto& channel : channels())
		if (channel.enabled())
			select == AUTO_DAC_POSITIVE ? invokeCmd(QString("input%1:autodac:positive %2").arg(QString::number(channel.index()), QString::number(voltage, 'f', 2))) : invokeCmd(QString("input%1:autodac:negative %2").arg(QString::number(channel.index()), QString::number(voltage, 'f', 2)));
	bu3100_sleep(50);
}

void Device6716::setAutoDACPositive(const double voltage) const {
	setAutoDAC(voltage, AUTO_DAC_POSITIVE);
	logMsg(QString("AutoDAC - Positive set to %1V").arg(voltage));
}

void Device6716::setAutoDACNegative(const double voltage) const {
	setAutoDAC(voltage, AUTO_DAC_NEGATIVE);
	logMsg(QString("AutoDAC - Negative set to %1V").arg(voltage));
}

std::vector<double> Device6716::getAutoDAC(short select) const {
	std::vector<double> voltages;
	for (auto& channel : channels()) {
		if (channel.enabled()) {
			if (select == AUTO_DAC_POSITIVE) {
				auto resp = invokeQuery(QString("input%1:autodac:positive?").arg(channel.index()));
				if (resp)
					voltages.push_back(resp->toUInt());
				else {
					voltages.push_back(0);
					emit reportError(QString("Couldnt read voltage for channel! 0 returned").arg(channel.index()));
				}
			}
			else {
				auto resp = invokeQuery(QString("input%1:autodac:negative?").arg(channel.index()));
				if (resp)
					voltages.push_back(resp->toUInt());
				else {
					voltages.push_back(0);
					emit reportError(QString("Couldnt read voltage for channel! 0 returned").arg(channel.index()));
				}
			}
		}
	}
	return voltages;
}

std::vector<double> Device6716::getAutoDACPositive() const {
	return getAutoDAC(AUTO_DAC_POSITIVE);
}

std::vector<double> Device6716::getAutoDACNegative() const {
	return getAutoDAC(AUTO_DAC_NEGATIVE);
}

bool Device6716::configureVoltageReferanceSwitches(const unsigned char newSegConfRegValue) const {
	auto result = writeFPGAreg(bu6716_FPGA_SEGCONF, newSegConfRegValue);
	bu3100_sleep(50);
	return result;
}

bool Device6716::isSegment1(const int channelIndex) const noexcept {
	return channelIndex < 9;
}

void Device6716::saveSubtype(const QString& str) const {
	invokeCmd(QString("BINT:MAIN:EEPROM:SUBTYPE '%1'\0").arg(str));
}

void Device6716::saveSerialnumber(const QString& str) const {
	invokeCmd(QString("BINT:MAIN:EEPROM:SERIAL #h%1\0").arg(str));
}

void Device6716::saveFirmwareRevision(const QString& str) const {
	invokeCmd(QString("BINT:MAIN:EEPROM:PREVISION #h%1\0").arg(str));
}

void Device6716::saveDriverRevision(const QString& str) const {
	//todo
}

QString Device6716::loadSubtype() const {
	if (auto resp = invokeQuery("*IDN?\0"); resp) {
		auto const& list = resp->split(',', QString::SplitBehavior::SkipEmptyParts);
		if (list.size() == 4)
			return list[1].right(2);
	}
	return "ER";
}

QString Device6716::loadSerialnumber() const {
	if (auto resp = invokeQuery("*IDN?\0"); resp) {
		auto const& list = resp->split(',', QString::SplitBehavior::SkipEmptyParts);
		if (list.size() == 4)
			return list[2];
	}
	return "error";
}

QString Device6716::loadFirmwareRevision() const {
	if (auto resp = invokeQuery("*IDN?\0"); resp) {
		auto const& list = resp->split(',', QString::SplitBehavior::SkipEmptyParts);
		if (list.size() == 4)
			return list[3];
	}
	return "error";
}

QString Device6716::loadDriverRevision() const {
	return DRIVER_BU6716_REVISION_STR;
}
