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

class TedsClass2Test : public Abstract6716Test {
	bool _initTeds2() const;
	bool _resetTeds2(char channel, char step) const;
protected:
	bool test() const override;
public:
	TedsClass2Test(const std::shared_ptr<Communication_6716>& connection);
};