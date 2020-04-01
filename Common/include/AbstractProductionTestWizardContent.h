#pragma once
#include <QThread>
#include <QVector>
#include <functional>
#include <set>
#include <map>

#include "TestsSelectionModel.h"
#include "AbstractHardwareConnector.h"
#include "PrintInterface.h"
#include "ResourcesHandler.h"
#include "DevicesManagerController.h"
#include "TestsRunnerController.h"
#include "UserActionRequestController.h"

////////////////////////////////////////////////////////////////////////////////////////////////////
/// @class	AbstractProductionTestWizardContent
///
/// @brief	This class is responsible for combining and holding all the components of the application. Also, it starts and stops all worker threads.
///
/// @author	Krzysztof Sommerfeld
/// @date	04.02.2020
////////////////////////////////////////////////////////////////////////////////////////////////////

class AbstractProductionTestWizardContent : public QObject {
    Q_OBJECT
	std::vector<QThread*> threads_;
protected:
	TestsRunner preTestsRunner_;
	TestsRunner mainTestsRunner_;
	DevicesManager devicesManager_;
public:
	ResourcesHandler addressesIdentificationPageContent;
	ResourcesHandler uutIdentificationPageContent;
	ResourcesHandler testEquipmentIdentificationPageContent;

	DevicesManagerController devicesManagerController_;
	TestsRunnerController preTestsRunnerController_;
	TestsRunnerController mainTestsRunnerController_;
	UserActionRequestController userActionRequestController_;
private:
	////////////////////////////////////////////////////////////////////////////////////////////////////
	/// @fn	void AbstractProductionTestWizardContent::runObjectOnSeparetedThread(QObject& obj);
	///
	/// @brief	Executes the object on separeted thread.
	///
	/// @author	Krzysztof Sommerfeld
	/// @date	04.02.2020
	///
	/// @param obj	The object.
	////////////////////////////////////////////////////////////////////////////////////////////////////

	void runObjectOnSeparetedThread(QObject& obj);

	////////////////////////////////////////////////////////////////////////////////////////////////////
	/// @fn	virtual void AbstractProductionTestWizardContent::initializeTests() = 0;
	///
	/// @brief	Creates all the tests.
	///
	/// @author	Krzysztof Sommerfeld
	/// @date	04.02.2020
	////////////////////////////////////////////////////////////////////////////////////////////////////

	virtual void initializeTests() = 0;

	////////////////////////////////////////////////////////////////////////////////////////////////////
	/// @fn	virtual void AbstractProductionTestWizardContent::createAddressIdentificationPageContent() = 0;
	///
	/// @brief	Creates the content of the AddressesIdentificationPage.qml.
	///
	/// @author	Krzysztof Sommerfeld
	/// @date	04.02.2020
	////////////////////////////////////////////////////////////////////////////////////////////////////

	virtual void createAddressIdentificationPageContent() = 0;

	////////////////////////////////////////////////////////////////////////////////////////////////////
	/// @fn	virtual void AbstractProductionTestWizardContent::createUutIdentificationPageContent() = 0;
	///
	/// @brief	Creates the content of the UutIdentificationPage.qml.
	///
	/// @author	Krzysztof Sommerfeld
	/// @date	04.02.2020
	////////////////////////////////////////////////////////////////////////////////////////////////////

	virtual void createUutIdentificationPageContent() = 0;

	////////////////////////////////////////////////////////////////////////////////////////////////////
	/// @fn	virtual void AbstractProductionTestWizardContent::createTestEquipmentIdentificationPageContent() = 0;
	///
	/// @brief	Creates the content of the TestEquipmentIdentificationPage.qml.
	///
	/// @author	Krzysztof Sommerfeld
	/// @date	04.02.2020
	////////////////////////////////////////////////////////////////////////////////////////////////////

	virtual void createTestEquipmentIdentificationPageContent() = 0;
public:
	////////////////////////////////////////////////////////////////////////////////////////////////////
	/// @fn	AbstractProductionTestWizardContent::AbstractProductionTestWizardContent(QObject* parent = nullptr);
	///
	/// @brief	Constructor
	///
	/// @author	Krzysztof Sommerfeld
	/// @date	04.02.2020
	///
	/// @param [in]	parent	(Optional) If not null - will be set as constructed object parent. In the
	/// 					QObject tree.
	////////////////////////////////////////////////////////////////////////////////////////////////////

	AbstractProductionTestWizardContent(QObject* parent = nullptr);

	////////////////////////////////////////////////////////////////////////////////////////////////////
	/// @fn	AbstractProductionTestWizardContent::~AbstractProductionTestWizardContent() override;
	///
	/// @brief	Destructor, it stops all the used threads.
	///
	/// @author	Krzysztof Sommerfeld
	/// @date	04.02.2020
	////////////////////////////////////////////////////////////////////////////////////////////////////

	~AbstractProductionTestWizardContent() override;

	////////////////////////////////////////////////////////////////////////////////////////////////////
	/// @fn	const std::vector<AbstractTest*>& AbstractProductionTestWizardContent::mainTests() noexcept;
	///
	/// @brief	Gets the main tests.
	///
	/// @author	Krzysztof Sommerfeld
	/// @date	04.02.2020
	///
	/// @returns	Const reference to the main tests;
	////////////////////////////////////////////////////////////////////////////////////////////////////

	const std::vector<AbstractTest*>& mainTests() noexcept;

	////////////////////////////////////////////////////////////////////////////////////////////////////
	/// @fn	void AbstractProductionTestWizardContent::initialize();
	///
	/// @brief	Initializes this object
	///
	/// @author	Krzysztof Sommerfeld
	/// @date	04.02.2020
	////////////////////////////////////////////////////////////////////////////////////////////////////

	void initialize();

	////////////////////////////////////////////////////////////////////////////////////////////////////
	/// @fn	virtual int AbstractProductionTestWizardContent::channelsCount() const noexcept = 0;
	///
	/// @brief	Gets the channels count of the tested device.
	///
	/// @author	Krzysztof Sommerfeld
	/// @date	04.02.2020
	///
	/// @returns	The channels count.
	////////////////////////////////////////////////////////////////////////////////////////////////////

	virtual int channelsCount() const noexcept = 0;
};
