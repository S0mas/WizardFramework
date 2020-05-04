#pragma once
#include "ScpiIF.h"

#include "../../../trunk/visa/include/visa.h"

class ScpiIFVisa : public ScpiIF {
public:
	int read(uint32_t const vi, unsigned char* dataPtr, uint32_t size, unsigned long* ret) const noexcept override {
		return viRead(vi, dataPtr, size, ret);
	}
	int write(uint32_t const vi, unsigned char* dataPtr, uint32_t size, unsigned long* ret) const noexcept override {
		return viWrite(vi, dataPtr, size, ret);
	}
};