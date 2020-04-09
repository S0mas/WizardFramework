#include "../../include/Device6991/Registers6991.h"
#include "../../include/Device6991/Device6991.h"

bool Register::readHw() noexcept {
	if (auto CL_SPI_CSR_reg_ = deviceIF->readFpgaRegister(address_); CL_SPI_CSR_reg_) {
		data_ = *CL_SPI_CSR_reg_;
		return true;
	}
	return false;
}

bool Register::writeHw() const noexcept {
	return deviceIF->writeFpgaRegister(address_, data_.to_ulong());
}

Register::Register(int const address, Device6991* deviceIF) : address_(address), deviceIF(deviceIF) {}

std::optional<int> Register::value() noexcept {
	return readHw() ? std::optional{ data_.to_ulong() } : std::nullopt;
}

bool CL_SPI_CSR_reg::isBusy() noexcept {
	return data_.test(CL_START_CL_BUSY);
}

void CL_SPI_CSR_reg::setCommand(Command const cmd) noexcept {
	data_ &= 0xFFFFFF00;
	data_ |= cmd;
}

CL_SPI_CSR_reg::CL_SPI_CSR_reg(Device6991* deviceIF) : Register(RegistersEnum::CL_SPI_CSR_reg, deviceIF) {}
bool CL_SPI_CSR_reg::startTests(bool const CL0, bool const CL1) noexcept {
	if (CL0 || CL1) {
		readHw();
		data_.set(CL0_SEL, CL0);
		data_.set(CL1_SEL, CL1);
		setCommand(WRITE_LOOPBACK);
		data_.set(CL_TEST_LOOP);

		if (!isBusy()) {
			data_.set(CL_START_CL_BUSY);
			return writeHw();
		}
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

std::optional<bool> CL_SPI_CSR_reg::isTestRunning(TestTypeEnum::Type const type) noexcept {
	if(type == TestTypeEnum::CL0)
		return readHw() ? std::optional{ data_.test(CL_TEST_LOOP) && data_.test(CL0_SEL) } : std::nullopt;
	else if(type == TestTypeEnum::CL1)
		return readHw() ? std::optional{ data_.test(CL_TEST_LOOP) && data_.test(CL1_SEL) } : std::nullopt;
	else
		return readHw() ? std::optional{ data_.test(CL_TEST_LOOP) && (data_.test(CL0_SEL) || data_.test(CL1_SEL)) } : std::nullopt;
}

DL_SPI_CSR1_reg::DL_SPI_CSR1_reg(Device6991* deviceIF) : Register(RegistersEnum::DL_SPI_CSR1_reg, deviceIF) {}

bool DL_SPI_CSR1_reg::startTests(bool const DL0, bool const DL1) noexcept {
	if (DL0 || DL1) {
		readHw();
		data_.set(DL0_TEST_MODE, DL0);
		data_.set(DL1_TEST_MODE, DL1);
		data_.set(DL_EN);
		return writeHw();
	}
	return false;
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

DFIFO_CSR_reg::DFIFO_CSR_reg(Device6991* deviceIF) : Register(RegistersEnum::DFIFO_CSR_reg, deviceIF) {}

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

CL_SPI_TLCNT_reg::CL_SPI_TLCNT_reg(Device6991* deviceIF) : Register(RegistersEnum::CL_SPI_TLCNT_reg, deviceIF) {}

CL0_SPI_TLERR_reg::CL0_SPI_TLERR_reg(Device6991* deviceIF) : Register(RegistersEnum::CL0_SPI_TLERR_reg, deviceIF) {}

CL1_SPI_TLERR_reg::CL1_SPI_TLERR_reg(Device6991* deviceIF) : Register(RegistersEnum::CL1_SPI_TLERR_reg, deviceIF) {}

DL0_SPI_TMCNT_reg::DL0_SPI_TMCNT_reg(Device6991* deviceIF) : Register(RegistersEnum::DL0_SPI_TMCNT_reg, deviceIF) {}

DL0_SPI_TMERR_reg::DL0_SPI_TMERR_reg(Device6991* deviceIF) : Register(RegistersEnum::DL0_SPI_TMERR_reg, deviceIF) {}

DL1_SPI_TMCNT_reg::DL1_SPI_TMCNT_reg(Device6991* deviceIF) : Register(RegistersEnum::DL1_SPI_TMCNT_reg, deviceIF) {}

DL1_SPI_TMERR_reg::DL1_SPI_TMERR_reg(Device6991* deviceIF) : Register(RegistersEnum::DL1_SPI_TMERR_reg, deviceIF) {}
