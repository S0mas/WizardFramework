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

class ShuntTest : public AbstractTest6716 {
	void quickAutobalance(ViUInt16 channelMask, ViInt16 adcGain = bu3416_GAIN_1) const;
	Limit limit1701 = {"L1701", -0.001, 0.001};
	Limit limit1702 = {"L1702", -0.007 , -0.005};
protected:
	Result test() const override;
public:
	ShuntTest();
};
