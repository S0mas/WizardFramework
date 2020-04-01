#pragma once
#define INSTR_LANGUAGE_SPECIFIC
#include <visa.h>
#include <bu3100.h>
#include <bu6716.h>
#include <t028.h>
#include <bu3416.h>
#include <stdio.h>
#include <string>
#include <QString>
#include "../AbstractTest6716.h"

class ExcitationCalibrationTest : public AbstractTest6716 {
	Limit limit1101 = { "L1101", -0.05, 0.05 };
	Limit limit1102 = { "L1102", 8.8, 9.2 };
	Limit limit1103 = { "L1103", -9.2, -8.8 };
	Limit limit2201 = { "L2201", -0.001, 0.001 };
	Limit limit2202 = { "L2202", 9.997, 10.003 };
	Limit limit2203 = { "L2203", -10.003, -9.997 };
	bool excitCalibration(ViInt16 channel) const;
protected:
	Result test() const override;
public:
	ExcitationCalibrationTest();
};