#include "../../include/Device6991/Registers6991.h"
#include "../../include/Device6991/Device6991.h"


bool Register::readHw() noexcept {
	if (auto clSpiCsrReg_ = deviceIF->readFpgaRegister(address_); clSpiCsrReg_) {
		data_ = *clSpiCsrReg_;
		return true;
	}
	return false;
}

bool Register::writeHw() const noexcept {
	return deviceIF->writeFpgaRegister(address_, data_.to_ulong());
}

Register::Register(int const address, Device6991* deviceIF) : address_(address), deviceIF(deviceIF) {}

bool CL_SPI_CSR_reg::isBusy() noexcept {
	return data_.test(CL_START_CL_BUSY);
}

void CL_SPI_CSR_reg::setCommand(Command const cmd) noexcept {
	data_ &= 0xFFFFFF00;
	data_ |= cmd;
}

CL_SPI_CSR_reg::CL_SPI_CSR_reg(Device6991* deviceIF) : Register(0x4488, deviceIF) {}
bool CL_SPI_CSR_reg::startTests(TargetSelection const target) noexcept {
	readHw();
	if (target == BOTH) {
		data_.set(CL0_SEL);
		data_.set(CL1_SEL);
	}
	else if (target == FE1) {
		data_.set(CL0_SEL);
		data_.reset(CL1_SEL);
	}
	else {
		data_.reset(CL0_SEL);
		data_.set(CL1_SEL);
	}
	setCommand(WRITE_LOOPBACK);
	data_.set(CL_TEST_LOOP);

	if (!isBusy()) {
		data_.set(CL_START_CL_BUSY);
		return writeHw();
	}
	return false;
}

bool CL_SPI_CSR_reg::stopTests() noexcept {
	if (readHw()) {
		data_.reset(CL_TEST_LOOP);
		return writeHw();
	}
	return false;
}

std::optional<bool> CL_SPI_CSR_reg::isTestRunning() noexcept {
	return readHw() ? std::optional{ data_.test(CL_TEST_LOOP) } : std::nullopt;
}

DL_SPI_CSR1_reg::DL_SPI_CSR1_reg(Device6991* deviceIF) : Register(0x4500, deviceIF) {}

bool DL_SPI_CSR1_reg::startTests(TargetSelection const target) noexcept {
	readHw();
	if (target == BOTH) {
		data_.set(DL0_TEST_MODE);
		data_.set(DL1_TEST_MODE);
	}
	else if (target == FE1) {
		data_.set(DL0_TEST_MODE);
		data_.reset(DL1_TEST_MODE);
	}
	else {
		data_.reset(DL0_TEST_MODE);
		data_.set(DL1_TEST_MODE);
	}
	data_.set(DL_EN);
	return writeHw();
}

bool DL_SPI_CSR1_reg::stopTests() noexcept {
	if (readHw()) {
		data_.reset(DL0_TEST_MODE);
		data_.reset(DL1_TEST_MODE);
		data_.reset(DL_EN);
		return writeHw();
	}
	return false;
}

std::optional<bool> DL_SPI_CSR1_reg::isTestRunning() noexcept {
	return readHw() ? std::optional{ data_.test(DL_EN) && (data_.test(DL0_TEST_MODE) || data_.test(DL1_TEST_MODE)) } : std::nullopt;
}

DFIFO_CSR_reg::DFIFO_CSR_reg(Device6991* deviceIF) : Register(0x4700, deviceIF) {}

bool DFIFO_CSR_reg::startTests() noexcept {
	readHw();
	data_.set(DFIFO_TEST_MODE);
	return writeHw();
}

bool DFIFO_CSR_reg::stopTests() noexcept {
	if (readHw()) {
		data_.reset(DFIFO_TEST_MODE);
		return writeHw();
	}
	return false;
}

std::optional<bool> DFIFO_CSR_reg::isTestRunning() noexcept {
	return readHw() ? std::optional{ data_.test(DFIFO_TEST_MODE) } : std::nullopt;
}