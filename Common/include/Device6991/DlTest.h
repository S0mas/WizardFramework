#pragma once
#include "Defines6991.h"

class Device6991;

class DlTests {
	Device6991* devIF_;
	FecIdType::Type dlTestsTargetFecId(bool const dl0, bool const dl1) const noexcept;
public:
	DlTests(Device6991* devIF) : devIF_(devIF) {}
	bool startTest(bool const dl0, bool const dl1, DlSclkFreqType::Type const freq) noexcept;
	bool stopTest() noexcept;
	bool isRunning(TestTypeEnum::Type const type = TestTypeEnum::INVALID) noexcept;
	std::optional<uint32_t> errors(TestTypeEnum::Type const type) const noexcept;
	std::optional<uint32_t> count(TestTypeEnum::Type const type) const noexcept;
};
