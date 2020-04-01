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

class IEPETest : public AbstractTest6716 {
	Limit limit2101 = { "L2101", 0.04, 0.067 };
protected:
	Result test() const override;
public:
	IEPETest();
};
