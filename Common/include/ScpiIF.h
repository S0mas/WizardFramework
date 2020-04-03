#pragma once

class ScpiIF {
public:
	virtual int read(int const vi, unsigned char* dataPtr, int size, unsigned long* ret) const noexcept = 0;
	virtual int write(int const vi, unsigned char* dataPtr, int size, unsigned long* ret) const noexcept = 0;
};

class ScpiIFMock : public ScpiIF {
public:
	int read(int const vi, unsigned char* dataPtr, int size, unsigned long* ret) const noexcept override { *ret = 1; return 0; }
	int write(int const vi, unsigned char* dataPtr, int size, unsigned long* ret) const noexcept override { *ret = 1; return 0; }
};
