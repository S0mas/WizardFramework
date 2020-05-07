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
		auto fcId = dlTestsTargetFecId(dl0, dl1);
		if (auto fecType = devIF_->readFecType(fcId); fecType) {
			return devIF_->isFecIdle(*fecType, fcId) &&
				devIF_->checkIfDlReceiverStateIsIdle() &&
				devIF_->DL_SPI_CSR2_reg_.setDlFrameLength(*fecType, fcId) &&
				devIF_->DL_SPI_CSR1_reg_.enableTestMode(fcId) &&
				devIF_->DL_CSR_reg_.setDlSclkFreqType(fcId, freq) &&
				devIF_->CMD_reg_.setCmd(fcId, *fecType == FecType::_6111 ? FecCmdsType6111::DL_TEST_START : FecCmdsType6132::DL_TEST_START);
		}
	}
	return false;
}

bool DlTests::stopTest() noexcept {
	if (isRunning()) {
		auto fcId = dlTestsTargetFecId(isRunning(TestTypeEnum::DL0), isRunning(TestTypeEnum::DL1));
		if (auto fecType = devIF_->readFecType(fcId); fecType) {
			return fcId != FecIdType::INVALID &&
				devIF_->CMD_reg_.setCmd(fcId, *fecType == FecType::_6111 ? FecCmdsType6111::DL_TEST_STOP : FecCmdsType6132::DL_TEST_STOP) &&
				devIF_->testWithTimeout([this, type = *fecType, fcId]() { return devIF_->isFecIdle(type, fcId); }, 100, 25) &&
				devIF_->DL_SPI_CSR1_reg_.disableTestMode() &&
				devIF_->testWithTimeout([this]() { return devIF_->checkIfDlReceiverStateIsIdle(); }, 100, 25);
		}
	}
	return false;
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
