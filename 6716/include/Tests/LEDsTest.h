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

class LEDsTest : public AbstractTest6716 {
	enum class LED_STATE { OFF, RED, GREEN };
	bool switchLED_inTestMode(const unsigned char channel, const LED_STATE state) const;
	void logAboutMarkedFailedChannels(const std::vector<bool>& mask) const noexcept;
	void testLED(const LED_STATE state) const;
protected:
	Result test() const override;
public:
	LEDsTest();
};