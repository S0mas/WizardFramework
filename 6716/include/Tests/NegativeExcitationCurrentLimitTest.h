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

class NegativeExcitationCurrentLimitTest : public AbstractTest6716 {
	Limit limit1501 = {"L1501", -6.01, -5.99};
	Limit limit1502 = {"L1502", -7.2, -6};
protected:
	Result test() const override;
public:
	NegativeExcitationCurrentLimitTest();
};
