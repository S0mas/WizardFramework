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

class I2CAccessTo1WireControllersTest : public AbstractTest6716 {
protected:
	Result test() const override;
public:
	I2CAccessTo1WireControllersTest() : AbstractTest6716("I2C Access to one wire controllers") {}
};