#pragma once
#define INSTR_LANGUAGE_SPECIFIC
#include "../../../visa/include/visa.h"
#include "../../../bu3100/include/bu3100.h"
#include "../../../bu6716/include/bu6716.h"
#include "../../../bu5716/t028/include/t028.h"
#include "../../../bu3416/include/bu3416.h"
#include <stdio.h>
#include <string>
#include <QString>
#include "../AbstractTest6716.h"

class I2CAccessToFPGATest : public AbstractTest6716 {
	bool testRWAndCheckId(const unsigned char expectedValue) const noexcept;
protected:
	Result test() const override;
public:
	I2CAccessToFPGATest();
};