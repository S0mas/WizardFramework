#include "../include/DeviceIdentityResourcesIF.h"
#include "../include/Resource.h"

std::shared_ptr<AbstractDataResource> DeviceIdentityResourcesIF::resource(const QString& resourceName, const QString& description, std::function<QString()>&& loadFunction, std::function<void(const QString&)>&& saveFunction, std::unique_ptr<AbstractResourceContentValidationStrategy>&& validation, const bool readOnly) const noexcept {
	auto management = std::make_unique<CustomResourceManagment>(std::move(loadFunction), std::move(saveFunction));
	return std::make_shared<CustomDataResource>(resourceName, description, Enums::GUI_Type::TYPE_1, std::move(management), std::move(validation), readOnly);
}

std::shared_ptr<AbstractDataResource> DeviceIdentityResourcesIF::serialnumberResource(const bool readOnly) const noexcept {
	return resource(QString("serial number %1").arg(nameId_), QString("%1 serial number").arg(nameId_), [this]() { return loadSerialnumber(); }, [this](const QString& subtype) { saveSerialnumber(subtype); }, std::make_unique<SerialnumberContentValidation>(), readOnly);
}

std::shared_ptr<AbstractDataResource> DeviceIdentityResourcesIF::subtypeResource(const bool readOnly) const noexcept {
	return resource(QString("subtype %1").arg(nameId_), QString("%1 subtype").arg(nameId_), [this]() { return loadSubtype(); }, [this](const QString& subtype) { saveSubtype(subtype); }, std::make_unique<SubtypeContentValidation>(), readOnly);
}

std::shared_ptr<AbstractDataResource> DeviceIdentityResourcesIF::firmwareRevisionResource(const bool readOnly) const noexcept {
	return resource(QString("firmware revision %1").arg(nameId_), QString("%1 firmware revision").arg(nameId_), [this]() { return loadFirmwareRevision(); }, [this](const QString& subtype) { saveFirmwareRevision(subtype); }, std::make_unique<FrimwareRevisionContentValidation>(), readOnly);
}

std::shared_ptr<AbstractDataResource> DeviceIdentityResourcesIF::driverRevisionResource(const bool readOnly) const noexcept {
	return resource(QString("driver revision %1").arg(nameId_), QString("%1 driver revision").arg(nameId_), [this]() { return loadDriverRevision(); }, [this](const QString& subtype) { saveDriverRevision(subtype); }, std::make_unique<DriverRevisionContentValidation>(), readOnly);
}
