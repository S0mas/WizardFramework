#pragma once
#define INSTR_LANGUAGE_SPECIFIC
#include "../../../visa/include/visa.h"
#include "../../../bu3100/include/bu3100.h"
#include "../../../bu6716/include/bu6716.h"
#include "../../../bu5716/t028/include/t028.h"
#include "../../../bu3416/include/bu3416.h"
#include "../../../bu6100/include/bu6100.h"
#include <stdio.h>
#include <string>
#include <QString>
#include "../AbstractTest6716.h"

class ChannelGainTest : public AbstractTest6716 {
	Limit limit1901 = {"L1901", 8.99, 9.01};
	Limit limit1902 = {"L1902", 8.99, 9.01};
	Limit limit1903 = {"L1903", 8.95, 9.05 };
	Limit limit1904 = {"L1904", 8.80, 9.20 };
	Limit limit1905 = {"L1905", 0.95, 1.05};
	Limit limit1906 = {"L1906", 9.95, 10.05};
	Limit limit1907 = {"L1907", 99.5, 100.5};
	Limit limit1908 = {"L1908", 990, 1010};
protected:
	Result test() const override;
public:
	ChannelGainTest();
};
