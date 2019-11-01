#pragma once
#include "../../Common/include/AbstractTest.h"
#include "bu6716_revision.h"
#define INSTR_LANGUAGE_SPECIFIC
#define bu6716_FPGA_ID							0x00  // and 0x01   (16 bit register, big endian)
//#include <visa.h>
//#include <bu6716.h>
//#include <t028.h>
//#include <bu3416.h>
#include "Communication_6716.h"
#include <memory>
#include <vector>

inline constexpr int hexFormat = 16;
inline constexpr int decFormat = 10;
inline constexpr int binFormat = 2;

class Abstract6716Test : public AbstractTest {
	void closeAll() const noexcept {}
	void setAutoDAC(const unsigned short channelMask, const double voltage, const short select) const;
	std::vector<double> getAutoDAC(const unsigned short channelMask, const short select) const;
protected:
	std::shared_ptr<Communication_6716> connection;
	void preTestSetUp() const override;
	void postTestCleanUp() const override;
public:
	bool checkValue(const unsigned short expected, const unsigned short actual, const int form = decFormat, const int width = 0, const char fill = ' ') const noexcept;
	bool checkId() const;
	bool testWriteRead(const unsigned char expected, const unsigned char address) const;
	std::vector<double> readValues(const short gain, const unsigned short channelMask, const double warnMargin, const double scanRate = 5000, const uint64_t scanCount = 5000) const;
	double readValue_oneChannel(const short gain, const unsigned short channel, const double warnMargin, const double scanRate = 5000, const uint64_t scanCount = 5000) const;
	unsigned char readDSR() const;
	std::vector<double> getAutoDACPositive(const unsigned short channelMask) const;
	std::vector<double> getAutoDACNegative(const unsigned short channelMask) const;
	void setAutoDACPositive(const unsigned short channelMask, const double voltage) const;
	void setAutoDACNegative(const unsigned short channelMask, const double voltage) const;
	unsigned short checkValues(const unsigned short channelMask, const std::vector<double>& values, const std::string& limitName, const double expected, const double marginNeg, const double marginPos, bool verbose = true, bool checkGains = false) const noexcept;
	unsigned short checkValue_oneChannel(const unsigned short channel, const double value, const std::string& limitName, const double expected, const double marginNeg, const double marginPos, bool verbose = true, bool checkGains = false) const noexcept;
	bool excitCalibration(short channel, double* offsetPos, double* offsetNeg, double* gainPos, double* gainNeg, bool store) const;
	bool excitationTest(const bool positivePolarization) const;
	bool signalPathCalibration(short channel, double offsets[4], double gainCoeffs[4], bool store) const;
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



