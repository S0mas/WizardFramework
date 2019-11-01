#pragma once
#include "../../Common/include/AbstractTest.h"
#include "bu6716_revision.h"
#define INSTR_LANGUAGE_SPECIFIC
#define bu6716_FPGA_ID							0x00  // and 0x01   (16 bit register, big endian)
#include <visa.h>
#include <bu6716.h>
#include <t028.h>
#include <bu3416.h>
#include "Communication_6716.h"
#include <memory>
#include <vector>

inline constexpr int hexFormat = 16;
inline constexpr int decFormat = 10;
inline constexpr int binFormat = 2;

class Abstract6716Test : public AbstractTest {
	void closeAll() const noexcept {}
	void setAutoDAC(const ViUInt16 channelMask, const ViReal64 voltage, const ViInt16 select) const;
	std::vector<ViReal64> getAutoDAC(const ViUInt16 channelMask, const ViInt16 select) const;
protected:
	std::shared_ptr<Communication_6716> connection;
	void preTestSetUp() const override;
	void postTestCleanUp() const override;
public:
	bool checkValue(const unsigned short expected, const unsigned short actual, const int form = decFormat, const int width = 0, const char fill = ' ') const noexcept;
	bool checkId() const;
	bool testWriteRead(const unsigned char expected, const unsigned char address) const;
	std::vector<ViReal64> readValues(const ViInt16 gain, const ViUInt16 channelMask, const ViReal64 warnMargin, const ViReal64 scanRate = 5000, const uint64_t scanCount = 5000) const;
	ViReal64 readValue_oneChannel(const ViInt16 gain, const ViUInt16 channel, const ViReal64 warnMargin, const ViReal64 scanRate = 5000, const uint64_t scanCount = 5000) const;
	unsigned char readDSR() const;
	std::vector<ViReal64> getAutoDACPositive(const ViUInt16 channelMask) const;
	std::vector<ViReal64> getAutoDACNegative(const ViUInt16 channelMask) const;
	void setAutoDACPositive(const ViUInt16 channelMask, const ViReal64 voltage) const;
	void setAutoDACNegative(const ViUInt16 channelMask, const ViReal64 voltage) const;
	ViUInt16 checkValues(const ViUInt16 channelMask, const std::vector<ViReal64>& values, const std::string& limitName, const ViReal64 expected, const ViReal64 marginNeg, const ViReal64 marginPos, ViBoolean verbose = true, ViBoolean checkGains = false) const noexcept;
	ViUInt16 checkValue_oneChannel(const ViUInt16 channel, const ViReal64 value, const std::string& limitName, const ViReal64 expected, const ViReal64 marginNeg, const ViReal64 marginPos, ViBoolean verbose = true, ViBoolean checkGains = false) const noexcept;
	bool excitCalibration(ViInt16 channel, ViReal64* offsetPos, ViReal64* offsetNeg, ViReal64* gainPos, ViReal64* gainNeg, ViBoolean store) const;
	bool excitationTest(const bool positivePolarization) const;
	bool signalPathCalibration(ViInt16 channel, ViReal64 offsets[4], ViReal64 gainCoeffs[4], ViBoolean store) const;
public:
	Abstract6716Test(const QString&& name, const std::shared_ptr<Communication_6716>& connection);
};

//void Abstract6716Test::closeAll() const noexcept {
//	if (env.viBu6716)
//		bu6716_close(env.viBu6716);
//	if (env.viBu3416)
//		bu3416_close(env.viBu3416);
//	if (env.viT028)
//		t028_close(env.viT028);
//	if (env.viT028Master)
//		bu3416_close(env.viT028Master);
//}



