#pragma once
#include "../../Common/include/AbstractTest.h"
#include "DeviceMock.h"

////////////////////////////////////////////////////////////////////////////////////////////////////
/// @class	MockTest
///
/// @brief	Mocks the AbstractTest class for testing purpose.
///
/// @author	Krzysztof Sommerfeld
/// @date	06.02.2020
////////////////////////////////////////////////////////////////////////////////////////////////////

class MockTest : public AbstractChannelsTest {
	Q_OBJECT
	inline static unsigned id = 0;
protected:
	bool mockResult;
	inline static DeviceMock* device1;
	inline static DeviceMock* device2;
	Result test() const override;
	void preTestSetUp() const override;
	void postTestCleanUp() const override;
	QString summary() const noexcept override { return QString("Mocked summary of %1\n").arg(name()); }
public:
	void setChannelsStates(const std::vector<bool>& states) noexcept override;
	inline static void setDevices(AbstractDevice* dev1, AbstractDevice* dev2) {
		device1 = dynamic_cast<DeviceMock*>(dev1);
		device2 = dynamic_cast<DeviceMock*>(dev2);
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////
	/// @fn	MockTest::MockTest(const bool mockResult, QObject* parent = nullptr) noexcept;
	///
	/// @brief	Constructor
	///
	/// @author	Krzysztof Sommerfeld
	/// @date	06.02.2020
	///
	/// @param 		   	mockResult	The result of the test - it is ignored in some mocks tests
	/// 							implementations.
	/// @param [in]	parent	(Optional) If not null - will be set as constructed object parent. In the
	/// 					QObject tree.
	////////////////////////////////////////////////////////////////////////////////////////////////////

	MockTest(const bool mockResult, QObject* parent = nullptr) noexcept;
	~MockTest() override = default;
};

class MockTestWithConfirmationAction : public MockTest {
protected:
	Result test() const override;
public:
	MockTestWithConfirmationAction(const bool mockResult, QObject* parent = nullptr) noexcept;
	~MockTestWithConfirmationAction() override = default;
};

class MockTestWithDecisionAction : public MockTest {
protected:
	Result test() const override;
public:
	MockTestWithDecisionAction(const bool mockResult, QObject* parent = nullptr) noexcept;
	~MockTestWithDecisionAction() override = default;
};

class MockTestWithChannelsErrorSelection : public MockTest {
protected:
	Result test() const override;
public:
	MockTestWithChannelsErrorSelection(const bool mockResult, QObject* parent = nullptr) noexcept;
	~MockTestWithChannelsErrorSelection() override = default;
};

class MockTestWithErrorHandlingSelection : public MockTest {
protected:
	Result test() const override;
public:
	MockTestWithErrorHandlingSelection(const bool mockResult, QObject* parent = nullptr) noexcept;
	~MockTestWithErrorHandlingSelection() override = default;
};