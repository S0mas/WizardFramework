#include "../include/Tests/NegativeExcitationCurrentLimitTest.h"
#include "../include/defines.h"

bool NegativeExcitationCurrentLimitTest::test() const {
	return excitationTest(false);
}

NegativeExcitationCurrentLimitTest::NegativeExcitationCurrentLimitTest(const std::shared_ptr<Communication_6716>& connection) : Abstract6716Test("Negative Excitation Current Limit", connection) {}