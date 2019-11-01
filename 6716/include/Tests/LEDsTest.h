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

class LEDsTest : public Abstract6716Test {
	enum class LED_STATE { OFF, RED, GREEN };
	bool switchLED_inTestMode(unsigned char channel, LED_STATE state) const;
protected:
	bool test() const override;
public:
	LEDsTest(const std::shared_ptr<Communication_6716>& connection);
};