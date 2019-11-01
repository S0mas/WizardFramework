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
#include "../AbstractTest_6716.h"

class NegativeExcitationCurrentLimitTest : public Abstract6716Test {
protected:
	bool test() const override;
public:
	NegativeExcitationCurrentLimitTest(const std::shared_ptr<Communication_6716>& connection);
};
