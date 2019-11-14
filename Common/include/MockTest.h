#include "AbstractTest.h"
#include <chrono>
#include <thread>

class MockTest : public AbstractTest {
	inline static unsigned id = 0;
	bool mockResult;
protected:
	bool test() const override {	
		for (int i = 0; i < 10; ++i) {
			waitForUserAction("Test msg", UserActionType::CONFIRMATION);
			std::this_thread::sleep_for(std::chrono::milliseconds(100));
			log(QString("step_%1...").arg(i));
		}
		return !problemReportedByUser;
	}

	void preTestSetUp() const override {
		log("PRECONDITIONS..");
	}

	void postTestCleanUp() const override {
		log("POSTCONDITIONS..");
	}
public:
	MockTest(const bool mockResult) : AbstractTest(QString("Mock Test TEST %1").arg(id++), true), mockResult(mockResult) {}
};
