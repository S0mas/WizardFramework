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
#include <atomic>

class TedsClass2Test : public AbstractTest6716 {
	void _initTeds2() const;
	bool _resetTeds2(unsigned channel, unsigned step) const;
protected:
	Result test() const override;
public:
	TedsClass2Test();
};