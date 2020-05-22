#include "../../include/Tests/NegativeExcitationCurrentLimitTest.h"

Result NegativeExcitationCurrentLimitTest::test() const {
	device6716->configureVoltageReferanceSwitches(0x60);
	return excitationTest(limit1501, limit1502, false);
}

NegativeExcitationCurrentLimitTest::NegativeExcitationCurrentLimitTest() : AbstractTest6716("Negative Excitation Current Limit") {}
