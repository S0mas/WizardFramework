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

class PositiveExcitationCurrentLimitTest : public AbstractTest6716 {
	Limit limit1401 = { "L1401", 5.99, 6.01 };
	Limit limit1402 = { "L1402", 6, 7.2 };
protected:
	Result test() const override;
public:
	PositiveExcitationCurrentLimitTest();
};
