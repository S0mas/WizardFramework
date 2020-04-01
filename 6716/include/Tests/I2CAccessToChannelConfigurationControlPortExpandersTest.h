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

class I2CAccessToChannelConfigurationControlPortExpandersTest : public AbstractTest6716 {
	bool checkPortExpander() const noexcept;
protected:
	Result test() const override;
public:
	I2CAccessToChannelConfigurationControlPortExpandersTest();
};
