#pragma once
#define INSTR_LANGUAGE_SPECIFIC
#include "../../../visa/include/visa.h"
#include "../../../bu3100/include/bu3100.h"
#include "../../../bu6716/include/bu6716.h"
#include "../../../bu5716/t028/include/t028.h"
#include "../../../bu3416/include/bu3416.h"
#include "../../../bu6100/include/bu6100.h"
#include <stdio.h>
#include <string>
#include <QString>
#include "../AbstractTest6716.h"

class SignalPathCalibrationTest : public AbstractTest6716 {
	Limit limit2301 = { "L2301", -0.01, 0.01 };
	Limit limit2302 = { "L2302", -0.01, 0.01 };
	Limit limit2303 = { "L2303", -0.05, 0.05 };
	Limit limit2304 = { "L2304", -0.25, 0.25 };
	Limit limit2305 = { "L2305", 8.99, 9.01 };
	Limit limit2306 = { "L2306", 8.99, 9.01 };
	Limit limit2307 = { "L2307", 8.95, 9.05 };
	Limit limit2308 = { "L2308", 8.75, 9.25 };
	bool signalPathCalibration(ViInt16 channel) const;
protected:
	Result test() const override;
public:
	SignalPathCalibrationTest();
};