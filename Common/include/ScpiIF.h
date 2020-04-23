#pragma once
#include <cstdint>
class ScpiIF {
public:
	virtual int read(uint32_t const vi, unsigned char* dataPtr, uint32_t size, unsigned long* ret) const noexcept = 0;
	virtual int write(uint32_t const vi, unsigned char* dataPtr, uint32_t size, unsigned long* ret) const noexcept = 0;
};

class ScpiIFMock : public ScpiIF {
public:
	int read(uint32_t const vi, unsigned char* dataPtr, uint32_t size, unsigned long* ret) const noexcept override { *ret = 1; return 0; }
	int write(uint32_t const vi, unsigned char* dataPtr, uint32_t size, unsigned long* ret) const noexcept override { *ret = 1; return 0; }
};
