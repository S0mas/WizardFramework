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

class HalfBridgeTest : public AbstractTest6716 {
	Limit limit1201 = {"L1201", -0.05, 0.05};
	Limit limit1202 = {"L1202", -3.5, -1.5};
	Limit limit1203 = {"L1203", 0.0015, 0.0035};
protected:
	Result test() const override;
public:
	HalfBridgeTest();
};
