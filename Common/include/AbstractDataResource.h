#pragma once
#include <QString>
#include <functional>
#include <memory>
#include <QObject>
#include "ResourceManagementStrategies.h"
#include "ResourceContentValidationStrategies.h"

class Enums : public QObject {
	Q_OBJECT
public:
	enum class GUI_Type {
		TYPE_1,
		TYPE_2
	};

	Q_ENUM(GUI_Type)
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// @class	AbstractDataResource
///
/// @brief	An abstract data resource class. Is used to handle the data that are used from the different layers of the application
///
/// @author	Krzysztof Sommerfeld
/// @date	03.02.2020
////////////////////////////////////////////////////////////////////////////////////////////////////

class AbstractDataResource {
	QString nameId_;
	QString description_;
	QString value_;
	QString temporaryValue_;
	Enums::GUI_Type guiType_;
	bool readOnly_;
protected:
	QString inputMask_;
	std::unique_ptr<AbstractResourceManagementStrategy> managementStrategy_ = nullptr;
	std::unique_ptr<AbstractResourceContentValidationStrategy> validationStrategy_ = nullptr;
	std::function<bool(const QString&)> validate_ = nullptr;
public:

	////////////////////////////////////////////////////////////////////////////////////////////////////
	/// @fn	AbstractDataResource::AbstractDataResource(const QString& nameId, const QString& description, const Enums::GUI_Type guiType, const bool readOnly = false) noexcept
	///
	/// @brief	Constructor
	///
	/// @author	Krzysztof Sommerfeld
	/// @date	03.02.2020
	///
	/// @param 	nameId	   	Identifier for the name.
	/// @param 	description	The description.
	/// @param 	guiType	   	Type of the graphical user interface.
	/// @param 	readOnly   	(Optional) True to read-only.
	////////////////////////////////////////////////////////////////////////////////////////////////////

	AbstractDataResource(const QString& nameId, const QString& description, const Enums::GUI_Type guiType, const bool readOnly = false) noexcept
		: nameId_(nameId),
		description_(description),
		value_(""),
		temporaryValue_(value()),
		inputMask_(""),
		guiType_(guiType),
		readOnly_(readOnly) {}

	////////////////////////////////////////////////////////////////////////////////////////////////////
	/// @fn	virtual AbstractDataResource::~AbstractDataResource() = default;
	///
	/// @brief	Destructor
	///
	/// @author	Krzysztof Sommerfeld
	/// @date	03.02.2020
	////////////////////////////////////////////////////////////////////////////////////////////////////

	virtual ~AbstractDataResource() = default;

	////////////////////////////////////////////////////////////////////////////////////////////////////
	/// @fn	QString AbstractDataResource::nameId() const noexcept
	///
	/// @brief	Name identifier
	///
	/// @author	Krzysztof Sommerfeld
	/// @date	03.02.2020
	///
	/// @returns	The nameId.
	////////////////////////////////////////////////////////////////////////////////////////////////////

	QString nameId() const noexcept {
		return nameId_;
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////
	/// @fn	QString AbstractDataResource::description() const noexcept
	///
	/// @brief	Gets the description
	///
	/// @author	Krzysztof Sommerfeld
	/// @date	03.02.2020
	///
	/// @returns	The description.
	////////////////////////////////////////////////////////////////////////////////////////////////////

	QString description() const noexcept {
		return description_;
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////
	/// @fn	QString AbstractDataResource::value() const noexcept
	///
	/// @brief	Gets the value
	///
	/// @author	Krzysztof Sommerfeld
	/// @date	03.02.2020
	///
	/// @returns	The value.
	////////////////////////////////////////////////////////////////////////////////////////////////////

	QString value() const noexcept {
		return value_;
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////
	/// @fn	void AbstractDataResource::setValue(const QString& newValue) noexcept
	///
	/// @brief	Sets a value
	///
	/// @author	Krzysztof Sommerfeld
	/// @date	03.02.2020
	///
	/// @param 	newValue	The new value.
	////////////////////////////////////////////////////////////////////////////////////////////////////

	void setValue(const QString& newValue) noexcept {
		if(validate(newValue))
			value_ = newValue;
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////
	/// @fn	QString AbstractDataResource::temporaryValue() const noexcept
	///
	/// @brief	Gets the temporary value
	///
	/// @author	Krzysztof Sommerfeld
	/// @date	03.02.2020
	///
	/// @returns	The temporary value.
	////////////////////////////////////////////////////////////////////////////////////////////////////

	QString temporaryValue() const noexcept {
		return temporaryValue_;
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////
	/// @fn	void AbstractDataResource::setTemporaryValue(const QString& newValue) noexcept
	///
	/// @brief	Sets temporary value
	///
	/// @author	Krzysztof Sommerfeld
	/// @date	03.02.2020
	///
	/// @param 	newValue	The new value.
	////////////////////////////////////////////////////////////////////////////////////////////////////

	void setTemporaryValue(const QString& newValue) noexcept {
		temporaryValue_ = newValue;
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////
	/// @fn	QString AbstractDataResource::inputMask() const noexcept
	///
	/// @brief	Gets the input mask
	///
	/// @author	Krzysztof Sommerfeld
	/// @date	03.02.2020
	///
	/// @returns	The input mask.
	////////////////////////////////////////////////////////////////////////////////////////////////////

	QString inputMask() const noexcept {
		return validationStrategy_->inputMask();
	}

	Enums::GUI_Type guiType() const noexcept {
		return guiType_;
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////
	/// @fn	bool AbstractDataResource::readOnly() const noexcept
	///
	/// @brief	Gets the read-only param.
	///
	/// @author	Krzysztof Sommerfeld
	/// @date	03.02.2020
	///
	/// @returns	The value of read-only param.
	////////////////////////////////////////////////////////////////////////////////////////////////////

	bool readOnly() const noexcept {
		return readOnly_;
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////
	/// @fn	void AbstractDataResource::setReadOnly(const bool newValue) noexcept
	///
	/// @brief	Sets read-only param.
	///
	/// @author	Krzysztof Sommerfeld
	/// @date	03.02.2020
	///
	/// @param 	newValue	The new value.
	////////////////////////////////////////////////////////////////////////////////////////////////////

	void setReadOnly(const bool newValue) noexcept {
		readOnly_ = newValue;
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////
	/// @fn	QString AbstractDataResource::load() noexcept
	///
	/// @brief	Gets the load
	///
	/// @author	Krzysztof Sommerfeld
	/// @date	03.02.2020
	///
	/// @returns	The loaded value.
	////////////////////////////////////////////////////////////////////////////////////////////////////

	QString load() noexcept {
		value_ = managementStrategy_->load();
		temporaryValue_ = value();
		return value();
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////
	/// @fn	void AbstractDataResource::save() const noexcept
	///
	/// @brief	Saves this object
	///
	/// @author	Krzysztof Sommerfeld
	/// @date	03.02.2020
	////////////////////////////////////////////////////////////////////////////////////////////////////

	void save() const noexcept {
		if(!readOnly())
			managementStrategy_->save(value());
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////
	/// @fn	bool AbstractDataResource::validate(const QString& proposedValue) const noexcept
	///
	/// @brief	Validates the given proposed value
	///
	/// @author	Krzysztof Sommerfeld
	/// @date	03.02.2020
	///
	/// @param 	proposedValue	The proposed value.
	///
	/// @returns	True if it succeeds, false if it fails.
	////////////////////////////////////////////////////////////////////////////////////////////////////

	bool validate(const QString& proposedValue) const noexcept {
		return validationStrategy_->validate(proposedValue);
	}
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// @fn	inline bool operator<(const AbstractDataResource& lhs, const AbstractDataResource& rhs) noexcept
///
/// @brief	Less-than comparison operator
///
/// @author	Krzysztof Sommerfeld
/// @date	03.02.2020
///
/// @param 	lhs	The first instance to compare.
/// @param 	rhs	The second instance to compare.
///
/// @returns	True if the first parameter is less than the second.
////////////////////////////////////////////////////////////////////////////////////////////////////

inline bool operator<(const AbstractDataResource& lhs, const AbstractDataResource& rhs) noexcept {
	return lhs.nameId() < rhs.nameId();
}