#include "../include/Tests/PositiveExcitationCurrentLimitTest.h"
#include "../include/defines.h"

bool PositiveExcitationCurrentLimitTest::test() const {
	configureVoltageReferanceSwitches(0x60);
	return excitationTest(true);
}

PositiveExcitationCurrentLimitTest::PositiveExcitationCurrentLimitTest(const std::shared_ptr<Communication_6716>& connection) : Abstract6716Test("Positive Excitation Current Limit", connection) {}
