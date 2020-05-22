#include "../../include/Device6991/DlTest.h"
#include "../../include/Device6991/Device6991.h"

FecIdType::Type DlTests::dlTestsTargetFecId(bool const dl0, bool const dl1) const noexcept {
	if (dl0 && dl1)
		return FecIdType::BOTH;
	if (dl0)
		return FecIdType::_1;
	if (dl1)
		return FecIdType::_2;
	return FecIdType::INVALID;
}

bool DlTests::startTest(bool const dl0, bool const dl1, DlSclkFreqType::Type const freq) noexcept {
	if (dl0 || dl1) {
		auto fecId = dlTestsTargetFecId(dl0, dl1);
		auto fecType = devIF_->readFecType(fecId);
		return fecType && devIF_->isFecIdle(fecId, *fecType) &&
			devIF_->DL_SPI_CSR1_reg_.isDlReciverIdle() &&
			devIF_->setProperDlFrame(fecId, *fecType) &&
			devIF_->DL_SPI_CSR1_reg_.enableTestMode(fecId) &&
			devIF_->fecRegs_[fecId].DL_CSR_reg_.setDlSclkFreqType(freq) &&
			devIF_->fecRegs_[fecId].CMD_reg_.setCmd(*fecType == FecType::_6111 ? FecCmdsType6111::DL_TEST_START : FecCmdsType6132::DL_TEST_START);
	}
	return false;
}

bool DlTests::stopTest() noexcept {
	auto fecId = FecIdType::BOTH;
	auto fecType = devIF_->readFecType(fecId);	
	return fecType &&
		devIF_->fecRegs_[fecId].CMD_reg_.setCmd(*fecType == FecType::_6111 ? FecCmdsType6111::DL_TEST_STOP : FecCmdsType6132::DL_TEST_STOP) &&
		devIF_->testWithTimeout([this, type = *fecType, fecId]() { return devIF_->fecRegs_[fecId].BOARD_CSR_reg_.isIdle(type, false); }, 100, 25) &&
		devIF_->DL_SPI_CSR1_reg_.disableTestMode() &&
		devIF_->testWithTimeout([this]() { return devIF_->DL_SPI_CSR1_reg_.isDlReciverIdle(false); }, 100, 25);	
}

bool DlTests::isRunning(TestTypeEnum::Type const type) noexcept {
	auto isRunning = devIF_->DL_SPI_CSR1_reg_.isTestRunning(type);
	return isRunning ? *isRunning : false;
}

std::optional<uint32_t> DlTests::errors(TestTypeEnum::Type const type) const noexcept {
	return type == TestTypeEnum::DL0 ? devIF_->DL0_SPI_TMERR_reg_.value() : devIF_->DL1_SPI_TMERR_reg_.value();
}

std::optional<uint32_t> DlTests::count(TestTypeEnum::Type const type) const noexcept {
	return type == TestTypeEnum::DL0 ? devIF_->DL0_SPI_TMCNT_reg_.value() : devIF_->DL1_SPI_TMCNT_reg_.value();
}
