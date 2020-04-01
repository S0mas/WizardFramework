#include "../include/Tests/PositiveExcitationCurrentLimitTest.h"

Result PositiveExcitationCurrentLimitTest::test() const {
	device6716->configureVoltageReferanceSwitches(0x60);
	return excitationTest(limit1401, limit1402, true);
}

PositiveExcitationCurrentLimitTest::PositiveExcitationCurrentLimitTest() : AbstractTest6716("Positive Excitation Current Limit") {}
