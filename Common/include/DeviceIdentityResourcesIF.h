#pragma once
#include "AbstractDataResource.h"
#include <memory>

////////////////////////////////////////////////////////////////////////////////////////////////////
/// @class	DeviceIdentityResourcesIF
///
/// @brief	The device identity resources interface. It should be inherit by any device class
/// 		that can be identify by this parameters - serial number, subtype, firmware revision,
/// 		driver revision. All virtual functions needs to be implemented.
///
/// @author	Krzysztof Sommerfeld
/// @date	04.02.2020
////////////////////////////////////////////////////////////////////////////////////////////////////

class DeviceIdentityResourcesIF {
	QString nameId_;

	////////////////////////////////////////////////////////////////////////////////////////////////////
	/// @fn	std::shared_ptr<AbstractDataResource> DeviceIdentityResourcesIF::resource(const QString& resourceName, const QString& description, std::function<QString()>&& loadFunction, std::function<void(const QString&)>&& saveFunction, std::unique_ptr<AbstractResourceContentValidationStrategy>&& validation, const bool readOnly = true) const noexcept;
	///
	/// @brief	Creates the resorce. Takes ownership of the strategies objects.
	///
	/// @author	Krzysztof Sommerfeld
	/// @date	04.02.2020
	///
	/// @param 		   	resourceName	Name of the resource.
	/// @param 		   	description 	The description.
	/// @param [in]		loadFunction	The load function.
	/// @param 		   	saveFunction	The save function.
	/// @param [in]		validation  	The validation.
	/// @param 		   	readOnly		(Optional) False to get modifiable resource.
	///
	/// @returns	The created resource.
	////////////////////////////////////////////////////////////////////////////////////////////////////

	std::shared_ptr<AbstractDataResource> resource(const QString& resourceName, const QString& description, std::function<QString()>&& loadFunction, std::function<void(const QString&)>&& saveFunction, std::unique_ptr<AbstractResourceContentValidationStrategy>&& validation, const bool readOnly = true) const noexcept;

	////////////////////////////////////////////////////////////////////////////////////////////////////
	/// @fn	virtual void DeviceIdentityResourcesIF::saveSerialnumber(const QString& str) const = 0;
	///
	/// @brief	Saves the serialnumber.
	///
	/// @author	Krzysztof Sommerfeld
	/// @date	04.02.2020
	///
	/// @param 	str	The string.
	////////////////////////////////////////////////////////////////////////////////////////////////////

	virtual void saveSerialnumber(const QString& str) const = 0;

	////////////////////////////////////////////////////////////////////////////////////////////////////
	/// @fn	virtual void DeviceIdentityResourcesIF::saveSubtype(const QString& str) const = 0;
	///
	/// @brief	Saves the subtype.
	///
	/// @author	Krzysztof Sommerfeld
	/// @date	04.02.2020
	///
	/// @param 	str	The string.
	////////////////////////////////////////////////////////////////////////////////////////////////////

	virtual void saveSubtype(const QString& str) const = 0;

	////////////////////////////////////////////////////////////////////////////////////////////////////
	/// @fn	virtual void DeviceIdentityResourcesIF::saveFirmwareRevision(const QString& str) const = 0;
	///
	/// @brief	Saves the firmware revision.
	///
	/// @author	Krzysztof Sommerfeld
	/// @date	04.02.2020
	///
	/// @param 	str	The string.
	////////////////////////////////////////////////////////////////////////////////////////////////////

	virtual void saveFirmwareRevision(const QString& str) const = 0;

	////////////////////////////////////////////////////////////////////////////////////////////////////
	/// @fn	virtual void DeviceIdentityResourcesIF::saveDriverRevision(const QString& str) const = 0;
	///
	/// @brief	Saves the driver revision.
	///
	/// @author	Krzysztof Sommerfeld
	/// @date	04.02.2020
	///
	/// @param 	str	The string.
	////////////////////////////////////////////////////////////////////////////////////////////////////

	virtual void saveDriverRevision(const QString& str) const = 0;

	////////////////////////////////////////////////////////////////////////////////////////////////////
	/// @fn	virtual QString DeviceIdentityResourcesIF::loadSerialnumber() const = 0;
	///
	/// @brief	Loads the serialnumber.
	///
	/// @author	Krzysztof Sommerfeld
	/// @date	04.02.2020
	///
	/// @returns	The serialnumber.
	////////////////////////////////////////////////////////////////////////////////////////////////////

	virtual QString loadSerialnumber() const = 0;

	////////////////////////////////////////////////////////////////////////////////////////////////////
	/// @fn	virtual QString DeviceIdentityResourcesIF::loadSubtype() const = 0;
	///
	/// @brief	Loads the subtype.
	///
	/// @author	Krzysztof Sommerfeld
	/// @date	04.02.2020
	///
	/// @returns	The subtype.
	////////////////////////////////////////////////////////////////////////////////////////////////////

	virtual QString loadSubtype() const = 0;

	////////////////////////////////////////////////////////////////////////////////////////////////////
	/// @fn	virtual QString DeviceIdentityResourcesIF::loadFirmwareRevision() const = 0;
	///
	/// @brief	Loads the firmware revision.
	///
	/// @author	Krzysztof Sommerfeld
	/// @date	04.02.2020
	///
	/// @returns	The firmware revision.
	////////////////////////////////////////////////////////////////////////////////////////////////////

	virtual QString loadFirmwareRevision() const = 0;

	////////////////////////////////////////////////////////////////////////////////////////////////////
	/// @fn	virtual QString DeviceIdentityResourcesIF::loadDriverRevision() const = 0;
	///
	/// @brief	Loads the driver revision.
	///
	/// @author	Krzysztof Sommerfeld
	/// @date	04.02.2020
	///
	/// @returns	The driver revision.
	////////////////////////////////////////////////////////////////////////////////////////////////////

	virtual QString loadDriverRevision() const = 0;
public:
	////////////////////////////////////////////////////////////////////////////////////////////////////
	/// @fn	DeviceIdentityResourcesIF::DeviceIdentityResourcesIF(const QString& nameId) noexcept
	///
	/// @brief	Constructor
	///
	/// @author	Krzysztof Sommerfeld
	/// @date	04.02.2020
	///
	/// @param 	nameId	The name identifier of the device.
	////////////////////////////////////////////////////////////////////////////////////////////////////

	DeviceIdentityResourcesIF(const QString& nameId) noexcept : nameId_(nameId) {}

	////////////////////////////////////////////////////////////////////////////////////////////////////
	/// @fn	std::shared_ptr<AbstractDataResource> DeviceIdentityResourcesIF::serialnumberResource(const bool readOnly = true) const noexcept;
	///
	/// @brief	Gets th serialnumber resource.
	///
	/// @author	Krzysztof Sommerfeld
	/// @date	04.02.2020
	///
	/// @param 	readOnly	(Optional) False to get modifiable resource.
	///
	/// @returns	The serialnumber revision resource.
	////////////////////////////////////////////////////////////////////////////////////////////////////

	std::shared_ptr<AbstractDataResource> serialnumberResource(const bool readOnly = true) const noexcept;

	////////////////////////////////////////////////////////////////////////////////////////////////////
	/// @fn	std::shared_ptr<AbstractDataResource> DeviceIdentityResourcesIF::subtypeResource(const bool readOnly = true) const noexcept;
	///
	/// @brief	Gets the subtype resource.
	///
	/// @author	Krzysztof Sommerfeld
	/// @date	04.02.2020
	///
	/// @param 	readOnly	(Optional) False to get modifiable resource.
	///
	/// @returns	The subtype revision resource.
	////////////////////////////////////////////////////////////////////////////////////////////////////

	std::shared_ptr<AbstractDataResource> subtypeResource(const bool readOnly = true) const noexcept;

	////////////////////////////////////////////////////////////////////////////////////////////////////
	/// @fn	std::shared_ptr<AbstractDataResource> DeviceIdentityResourcesIF::firmwareRevisionResource(const bool readOnly = true) const noexcept;
	///
	/// @brief	Gets the firmware revision resource.
	///
	/// @author	Krzysztof Sommerfeld
	/// @date	04.02.2020
	///
	/// @param 	readOnly	(Optional) False to get modifiable resource.
	///
	/// @returns	The firmware revision resource.
	////////////////////////////////////////////////////////////////////////////////////////////////////

	std::shared_ptr<AbstractDataResource> firmwareRevisionResource(const bool readOnly = true) const noexcept;

	////////////////////////////////////////////////////////////////////////////////////////////////////
	/// @fn	std::shared_ptr<AbstractDataResource> DeviceIdentityResourcesIF::driverRevisionResource(const bool readOnly = true) const noexcept;
	///
	/// @brief	Gets the driver revision resource.
	///
	/// @author	Krzysztof Sommerfeld
	/// @date	04.02.2020
	///
	/// @param 	readOnly	(Optional) False to get modifiable resource.
	///
	/// @returns	The driver revision resource.
	////////////////////////////////////////////////////////////////////////////////////////////////////

	std::shared_ptr<AbstractDataResource> driverRevisionResource(const bool readOnly = true) const noexcept;

	////////////////////////////////////////////////////////////////////////////////////////////////////
	/// @fn	std::vector<std::shared_ptr<AbstractDataResource>> DeviceIdentityResourcesIF::allIdentityResources(const bool readOnly = true) const noexcept
	///
	/// @brief	Gets all identity resources.
	///
	/// @author	Krzysztof Sommerfeld
	/// @date	04.02.2020
	///
	/// @param 	readOnly	(Optional) False to get modifiable resources.
	///
	/// @returns	The device identity resources vector.
	////////////////////////////////////////////////////////////////////////////////////////////////////

	std::vector<std::shared_ptr<AbstractDataResource>> allIdentityResources(const bool readOnly = true) const noexcept {
		std::vector<std::shared_ptr<AbstractDataResource>> resources;
		resources.push_back(serialnumberResource(readOnly));
		resources.push_back(subtypeResource(readOnly));
		resources.push_back(firmwareRevisionResource(readOnly));
		resources.push_back(driverRevisionResource(readOnly));
		return resources;
	}
};