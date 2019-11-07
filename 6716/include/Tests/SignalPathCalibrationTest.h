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
#include "../AbstractTest_6716.h"

class SignalPathCalibrationTest : public Abstract6716Test {
protected:
	bool test() const override;
public:
	SignalPathCalibrationTest(const std::shared_ptr<Communication_6716>& connection);
};