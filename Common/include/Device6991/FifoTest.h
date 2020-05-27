#pragma once
#include "Defines6991.h"
#include <QFile>

class Device6991;

class FifoTest {
	Device6991* devIF_;
	uint64_t expected_ = 0;
	uint64_t count_ = 0;
	uint64_t errors_ = 0;
	uint32_t lastSample_;
	QFile* dataFile_;
public:
	FifoTest(Device6991* devIF);
	void validateData(std::vector<uint32_t> const& data);
	FifoTestModel model() noexcept;
	bool configure(FifoTestModel::Configuration const& config) noexcept;
	bool startTest(FifoTestModel::Configuration const& config) noexcept;
	bool stopTest() noexcept;
	uint64_t errors() const noexcept;
	uint64_t count() const noexcept;
	bool isRunning() const noexcept;
};