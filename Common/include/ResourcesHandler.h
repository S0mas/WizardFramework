#pragma once
#include "Resource.h"
#include <map>
#include <QStringList>
#include "AbstractDevice.h"

////////////////////////////////////////////////////////////////////////////////////////////////////
/// @class	ResourcesHandler
///
/// @brief	The resources handler.
///
/// @author	Krzysztof Sommerfeld
/// @date	03.02.2020
////////////////////////////////////////////////////////////////////////////////////////////////////

class ResourcesHandler : public QObject {
	Q_OBJECT

	/// @brief	The unique resources container. The resources are mapped to their nameIds.
	std::map<QString, std::shared_ptr<AbstractDataResource>> uniqueResources_;
public:

	////////////////////////////////////////////////////////////////////////////////////////////////////
	/// @fn	ResourcesHandler::ResourcesHandler(QObject* parent = nullptr) noexcept
	///
	/// @brief	Constructor
	///
	/// @author	Krzysztof Sommerfeld
	/// @date	03.02.2020
	///
	/// @param [in]	parent	(Optional) If not null - will be set as constructed object parent. In the
	/// 					QObject tree.
	////////////////////////////////////////////////////////////////////////////////////////////////////

	ResourcesHandler(QObject* parent = nullptr) noexcept : QObject(parent) {}

	////////////////////////////////////////////////////////////////////////////////////////////////////
	/// @fn	ResourcesHandler::~ResourcesHandler() = default;
	///
	/// @brief	Default destructor
	///
	/// @author	Krzysztof Sommerfeld
	/// @date	03.02.2020
	////////////////////////////////////////////////////////////////////////////////////////////////////

	~ResourcesHandler() override = default;

	////////////////////////////////////////////////////////////////////////////////////////////////////
	/// @fn	void ResourcesHandler::addResources(const std::vector<AbstractDevice*>& devices) noexcept
	///
	/// @brief	Adds the resources.
	///
	/// @author	Krzysztof Sommerfeld
	/// @date	03.02.2020
	///
	/// @param 	devices	The devices from which resources should be added.
	////////////////////////////////////////////////////////////////////////////////////////////////////

	void addResources(const std::vector<AbstractDevice*>& devices) noexcept {
		for (auto const& device : devices)
			addResources(device->inputResources());
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////
	/// @fn	void ResourcesHandler::addResources(const std::vector<std::shared_ptr<AbstractDataResource>>& resources) noexcept
	///
	/// @brief	Adds the resources.
	///
	/// @author	Krzysztof Sommerfeld
	/// @date	03.02.2020
	///
	/// @param 	resources	The resources to add.
	////////////////////////////////////////////////////////////////////////////////////////////////////

	void addResources(const std::vector<std::shared_ptr<AbstractDataResource>>& resources) noexcept {
		for (auto const& resource : resources)
			uniqueResources_.insert({ resource->nameId(), resource });
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////
	/// @fn	void ResourcesHandler::clearResources() noexcept
	///
	/// @brief	Removes all the resources.
	///
	/// @author	Krzysztof Sommerfeld
	/// @date	03.02.2020
	////////////////////////////////////////////////////////////////////////////////////////////////////

	void clearResources() noexcept {
		uniqueResources_.clear();
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////
	/// @fn	Q_INVOKABLE QString ResourcesHandler::mask(const QString& nameId) const
	///
	/// @brief	Returns the mask of the resource.
	///
	/// @author	Krzysztof Sommerfeld
	/// @date	03.02.2020
	///
	/// @param 	nameId	Name identifier of the resource.
	///
	/// @returns	The mask.
	////////////////////////////////////////////////////////////////////////////////////////////////////

	Q_INVOKABLE QString mask(const QString& nameId) const {
		return uniqueResources_.at(nameId)->inputMask();
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////
	/// @fn	Q_INVOKABLE QString ResourcesHandler::value(const QString& nameId) const
	///
	/// @brief	Reads the value of the resource.
	///
	/// @author	Krzysztof Sommerfeld
	/// @date	03.02.2020
	///
	/// @param 	nameId	Name identifier of the resource.
	///
	/// @returns	The value of the resource.
	////////////////////////////////////////////////////////////////////////////////////////////////////

	Q_INVOKABLE QString value(const QString& nameId) const {
		return uniqueResources_.at(nameId)->value();
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////
	/// @fn	Q_INVOKABLE QString ResourcesHandler::load(const QString& nameId) noexcept
	///
	/// @brief	Loads the resource.
	///
	/// @author	Krzysztof Sommerfeld
	/// @date	03.02.2020
	///
	/// @param 	nameId	Name identifier of the resource.
	///
	/// @returns	The value of the resource.
	////////////////////////////////////////////////////////////////////////////////////////////////////

	Q_INVOKABLE QString load(const QString& nameId) noexcept {
		return uniqueResources_.at(nameId)->load();
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////
	/// @fn	Q_INVOKABLE void ResourcesHandler::save(const QString& nameId) const noexcept
	///
	/// @brief	Saves the resource.
	///
	/// @author	Krzysztof Sommerfeld
	/// @date	03.02.2020
	///
	/// @param 	nameId	Name identifier of the resource.
	////////////////////////////////////////////////////////////////////////////////////////////////////

	Q_INVOKABLE void save(const QString& nameId) const noexcept {
		uniqueResources_.at(nameId)->save();
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////
	/// @fn	Q_INVOKABLE bool ResourcesHandler::validate(const QString& nameId, const QString& value) const
	///
	/// @brief	Validates the resource for the selected value.
	///
	/// @author	Krzysztof Sommerfeld
	/// @date	03.02.2020
	///
	/// @param 	nameId	Name identifier of the resource.
	/// @param 	value 	The value.
	///
	/// @returns	True if it succeeds, false if it fails.
	////////////////////////////////////////////////////////////////////////////////////////////////////

	Q_INVOKABLE bool validate(const QString& nameId, const QString& value) const {
		return uniqueResources_.at(nameId)->validate(value);
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////
	/// @fn	Q_INVOKABLE QString ResourcesHandler::description(const QString& nameId) const
	///
	/// @brief	Gets descriptions of the resource.
	///
	/// @author	Krzysztof Sommerfeld
	/// @date	03.02.2020
	///
	/// @param 	nameId	Name identifier of the resource.
	///
	/// @returns	The description of the resource.
	////////////////////////////////////////////////////////////////////////////////////////////////////

	Q_INVOKABLE QString description(const QString& nameId) const {
		return uniqueResources_.at(nameId)->description();
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////
	/// @fn	Q_INVOKABLE QStringList ResourcesHandler::namesIds() const noexcept
	///
	/// @brief	Names identifiers of the resources.
	///
	/// @author	Krzysztof Sommerfeld
	/// @date	03.02.2020
	///
	/// @returns	A QStringList.
	////////////////////////////////////////////////////////////////////////////////////////////////////

	Q_INVOKABLE QStringList namesIds() const noexcept {
		QStringList namesIds;
		for (auto const& resource : uniqueResources_)
			namesIds.push_back(resource.first);
		return namesIds;
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////
	/// @fn	Q_INVOKABLE bool ResourcesHandler::setValue(const QString& nameId, const QString& value)
	///
	/// @brief	Tries to set the value of the selected resource.
	///
	/// @author	Krzysztof Sommerfeld
	/// @date	03.02.2020
	///
	/// @param 	nameId	Name identifier of the resource.
	/// @param 	value 	The value.
	///
	/// @returns	True if it succeeds, false if it fails.
	////////////////////////////////////////////////////////////////////////////////////////////////////

	Q_INVOKABLE bool setValue(const QString& nameId, const QString& value) {
		auto result = validate(nameId, value);
		if (result)
			uniqueResources_.at(nameId)->setValue(value);
		return result;
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////
	/// @fn	Q_INVOKABLE bool ResourcesHandler::readOnly(const QString& nameId) const
	///
	/// @brief	Checks if the resource is read-only.
	///
	/// @author	Krzysztof Sommerfeld
	/// @date	03.02.2020
	///
	/// @param 	nameId	Name identifier of the resource.
	///
	/// @returns	True if it succeeds, false if it fails.
	////////////////////////////////////////////////////////////////////////////////////////////////////

	Q_INVOKABLE bool readOnly(const QString& nameId) const {
		return uniqueResources_.at(nameId)->readOnly();
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////
	/// @fn	Q_INVOKABLE void ResourcesHandler::setTemporary(const QString& nameId, const QString& value) noexcept
	///
	/// @brief	Sets a temporary value of the resource.
	///
	/// @author	Krzysztof Sommerfeld
	/// @date	03.02.2020
	///
	/// @param 	nameId	Name identifier of the resource.
	/// @param 	value 	The value.
	////////////////////////////////////////////////////////////////////////////////////////////////////

	Q_INVOKABLE void setTemporary(const QString& nameId, const QString& value) noexcept {
		uniqueResources_.at(nameId)->setTemporaryValue(value);
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////
	/// @fn	Q_INVOKABLE void ResourcesHandler::loadAll() noexcept
	///
	/// @brief	Loads all the resources.
	///
	/// @author	Krzysztof Sommerfeld
	/// @date	03.02.2020
	////////////////////////////////////////////////////////////////////////////////////////////////////

	Q_INVOKABLE void loadAll() noexcept {
		for (auto& resource : uniqueResources_)
			resource.second->load();
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////
	/// @fn	Q_INVOKABLE void ResourcesHandler::saveAll() noexcept
	///
	/// @brief	Saves all the resources.
	///
	/// @author	Krzysztof Sommerfeld
	/// @date	03.02.2020
	////////////////////////////////////////////////////////////////////////////////////////////////////

	Q_INVOKABLE void saveAll() noexcept {
		for (auto& resource : uniqueResources_)
			resource.second->save();
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////
	/// @fn	Q_INVOKABLE bool ResourcesHandler::allValid() noexcept
	///
	/// @brief	Determines if all the resources are in valid state.
	///
	/// @author	Krzysztof Sommerfeld
	/// @date	03.02.2020
	///
	/// @returns	True if it succeeds, false if it fails.
	////////////////////////////////////////////////////////////////////////////////////////////////////

	Q_INVOKABLE bool allValid() noexcept {
		for (auto& resource : uniqueResources_)
			if (!resource.second->validate(resource.second->temporaryValue()))
				return false;
		return true;
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////
	/// @fn	Q_INVOKABLE void ResourcesHandler::setAll() noexcept
	///
	/// @brief	Sets all the resources to its temporary value.
	///
	/// @author	Krzysztof Sommerfeld
	/// @date	03.02.2020
	////////////////////////////////////////////////////////////////////////////////////////////////////

	Q_INVOKABLE void setAll() noexcept {
		for (auto& resource : uniqueResources_)
			setValue(resource.second->nameId(), resource.second->temporaryValue());
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////
	/// @fn	Q_INVOKABLE void ResourcesHandler::disableAll() noexcept
	///
	/// @brief	Disables all the resources.
	///
	/// @author	Krzysztof Sommerfeld
	/// @date	03.02.2020
	////////////////////////////////////////////////////////////////////////////////////////////////////

	Q_INVOKABLE void disableAll() noexcept {
		for (auto& resource : uniqueResources_)
			resource.second->setReadOnly(true);
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////
	/// @fn	Q_INVOKABLE void ResourcesHandler::enableAll() noexcept
	///
	/// @brief	Enables all the resources.
	///
	/// @author	Krzysztof Sommerfeld
	/// @date	03.02.2020
	////////////////////////////////////////////////////////////////////////////////////////////////////

	Q_INVOKABLE void enableAll() noexcept {
		for (auto& resource : uniqueResources_)
			resource.second->setReadOnly(false);
	}
};