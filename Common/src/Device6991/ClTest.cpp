#include "../../include/Device6991/ClTest.h"
#include "../../include/Device6991/Device6991.h"

ClTests::ClTests(Device6991 * devIF) : devIF_(devIF) {}

bool ClTests::startTest(bool const cl0, bool const cl1) noexcept {
	if (cl0 || cl1)
		return devIF_->CL_SPI_CSR_reg_.startTests(cl0, cl1);
	return false;
}

bool ClTests::stopTest() noexcept {
	return devIF_->CL_SPI_CSR_reg_.stopTests();
}

bool ClTests::isRunning(TestTypeEnum::Type const type) noexcept {
	auto isRunning = devIF_->CL_SPI_CSR_reg_.isTestRunning(type);
	return isRunning ? *isRunning : false;
}

std::optional<uint32_t> ClTests::errors(TestTypeEnum::Type const type) const noexcept {
	return type == TestTypeEnum::CL0 ? devIF_->CL0_SPI_TLERR_reg_.value() : devIF_->CL1_SPI_TLERR_reg_.value();
}

std::optional<uint32_t> ClTests::count() const noexcept {
	return devIF_->CL_SPI_TLCNT_reg_.value();
}
