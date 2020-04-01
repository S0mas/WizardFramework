#pragma once
#include "AbstractDataResource.h"

////////////////////////////////////////////////////////////////////////////////////////////////////
/// @class	QSettingsResource
///
/// @brief	The QSettings resource. The validation strategy is not specified and manage strategy is set to use QSettings.
///
/// @author	Krzysztof Sommerfeld
/// @date	03.02.2020
////////////////////////////////////////////////////////////////////////////////////////////////////

class QSettingsResource : public AbstractDataResource {
public:
	QSettingsResource(const QString& nameId, const QString& description, const Enums::GUI_Type guiType, std::unique_ptr<AbstractResourceContentValidationStrategy>&& validationStrategy, const bool readOnly = false) noexcept
		: AbstractDataResource(nameId, description, guiType, readOnly) {
		validationStrategy_ = std::move(validationStrategy);
		managementStrategy_ = std::make_unique<QSettingsResourceManagment>(nameId, validationStrategy_->defaultValue());
	}
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// @class	CustomDataResource
///
/// @brief	A custom data resource. Both strategies needs to be given as parameters.
///
/// @author	Krzysztof Sommerfeld
/// @date	03.02.2020
////////////////////////////////////////////////////////////////////////////////////////////////////

class CustomDataResource : public AbstractDataResource {
public:
	CustomDataResource(const QString& nameId, const QString& description, const Enums::GUI_Type guiType, std::unique_ptr<AbstractResourceManagementStrategy>&& managementStrategy, std::unique_ptr<AbstractResourceContentValidationStrategy>&& validationStrategy, const bool readOnly = false) noexcept
		: AbstractDataResource(nameId, description, guiType, readOnly) {
		managementStrategy_ = std::move(managementStrategy);
		validationStrategy_ = std::move(validationStrategy);
	}
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// @class	ConstantResource
///
/// @brief	A constant resource. The validation stratedy is NoValidation and the management strategy is normal reading and writing to class member variable.
///
/// @author	Krzysztof Sommerfeld
/// @date	03.02.2020
////////////////////////////////////////////////////////////////////////////////////////////////////

class ConstantResource : public AbstractDataResource {
public:
	ConstantResource(const QString& nameId, const QString& description, const Enums::GUI_Type guiType, const QString& val) noexcept
		: AbstractDataResource(nameId, description, guiType, true) {
		validationStrategy_ = std::make_unique<NoValidation>();
		setValue(val);
		managementStrategy_ = std::make_unique<CustomResourceManagment>([v = value()]() { return v; }, [](...) {});
	}
};


