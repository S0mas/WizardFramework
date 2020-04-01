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

class ACCouplingTest : public AbstractTest6716 {
	Limit limit2001 = { "L2001", 2, 4 };
	Limit limit2002 = { "L2002", -4, -2 };
	Limit limit2003 = { "L2003", 8.99, 9.01 };
protected:
	Result test() const override;
public:
	ACCouplingTest();
};