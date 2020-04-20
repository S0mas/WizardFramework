#pragma once
#include "ScpiIF.h"
#include "../../../visa/include/visa.h"

class ScpiIFVisa : public ScpiIF {
public:
	int read(int const vi, unsigned char* dataPtr, int size, unsigned long* ret) const noexcept override {
		return viRead(vi, dataPtr, size, ret);
	}
	int write(int const vi, unsigned char* dataPtr, int size, unsigned long* ret) const noexcept override {
		return viWrite(vi, dataPtr, size, ret);
	}
};