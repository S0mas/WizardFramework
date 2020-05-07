#pragma once
#include "Defines6991.h"

class Device6991;

class ClTests {
	Device6991* devIF_;
public:
	ClTests(Device6991* devIF);
	bool startTest(bool const cl0, bool const cl1) noexcept;
	bool stopTest() noexcept;
	bool isRunning(TestTypeEnum::Type const type = TestTypeEnum::INVALID) noexcept;
	std::optional<uint32_t> errors(TestTypeEnum::Type const type) const noexcept;
	std::optional<uint32_t> count() const noexcept;
};
