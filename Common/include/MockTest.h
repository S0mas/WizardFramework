#include "AbstractTest.h"
#include <chrono>
#include <thread>

class MockTest : public AbstractTest {
	inline static unsigned id = 0;
	bool mockResult;
protected:
	bool test() const override {
		for (int i = 0; i < 10; ++i) {
			std::this_thread::sleep_for(std::chrono::milliseconds(100));
			log(QString("step_%1...\n").arg(i));
		}
		return mockResult;
	}

	void preTestSetUp() const override {
		log("PRECONDITIONS..\n");
	}

	void postTestCleanUp() const override {
		log("POSTCONDITIONS..\n");
	}
public:
	MockTest(const bool mockResult) : AbstractTest(QString("Mock Test TEST TEST TEST TEST TEST TEST TEST %1").arg(id++)), mockResult(mockResult) {}
};
