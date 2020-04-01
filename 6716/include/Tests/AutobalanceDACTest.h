#pragma once
#define INSTR_LANGUAGE_SPECIFIC
#include <visa.h>
#include <bu3100.h>
#include <bu6716.h>
#include <t028.h>
#include <bu3416.h>
#include <bu6100.h>
#include <stdio.h>
#include <string>
#include <QString>
#include "../AbstractTest6716.h"

class AutobalanceDACTest : public AbstractTest6716 {
	Limit limit1601 = { "L1601", 0.025, 0.035 };
	Limit limit1602 = { "L1602", -0.035, -0.025 };
protected:
	Result test() const override;
public:
	AutobalanceDACTest();
};
