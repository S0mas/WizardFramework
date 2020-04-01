#pragma once
#include <QSettings>
#include <QString>
#include <functional>

////////////////////////////////////////////////////////////////////////////////////////////////////
/// @class	AbstractResourceManagementStrategy
///
/// @brief	An abstract resource management strategy interface. It should be inherited
/// 		and implemented to use some kind of management strategy for the loading and saving the value of the resource that uses it.
///
/// @author	Krzysztof Sommerfeld
/// @date	03.02.2020
////////////////////////////////////////////////////////////////////////////////////////////////////

class AbstractResourceManagementStrategy {
protected:
	std::function<QString()> load_ = nullptr;
	std::function<void(const QString&)> save_ = nullptr;
public:

	////////////////////////////////////////////////////////////////////////////////////////////////////
	/// @fn	AbstractResourceManagementStrategy::AbstractResourceManagementStrategy() noexcept = default;
	///
	/// @brief	Default constructor
	///
	/// @author	Krzysztof Sommerfeld
	/// @date	03.02.2020
	////////////////////////////////////////////////////////////////////////////////////////////////////

	AbstractResourceManagementStrategy() noexcept = default;

	////////////////////////////////////////////////////////////////////////////////////////////////////
	/// @fn	AbstractResourceManagementStrategy::AbstractResourceManagementStrategy(const AbstractResourceManagementStrategy& original) noexcept
	///
	/// @brief	Copy constructor
	///
	/// @author	Krzysztof Sommerfeld
	/// @date	03.02.2020
	///
	/// @param 	original	The original.
	////////////////////////////////////////////////////////////////////////////////////////////////////

	AbstractResourceManagementStrategy(const AbstractResourceManagementStrategy& original) noexcept
		: load_(original.load_),
		save_(original.save_) {}

	////////////////////////////////////////////////////////////////////////////////////////////////////
	/// @fn	virtual AbstractResourceManagementStrategy::~AbstractResourceManagementStrategy() = default;
	///
	/// @brief	Default destructor
	///
	/// @author	Krzysztof Sommerfeld
	/// @date	03.02.2020
	////////////////////////////////////////////////////////////////////////////////////////////////////

	virtual ~AbstractResourceManagementStrategy() = default;

	////////////////////////////////////////////////////////////////////////////////////////////////////
	/// @fn	QString AbstractResourceManagementStrategy::load() const noexcept
	///
	/// @brief	Loads and returns the value of the managed resource.
	///
	/// @author	Krzysztof Sommerfeld
	/// @date	03.02.2020
	///
	/// @returns	A QString.
	////////////////////////////////////////////////////////////////////////////////////////////////////

	QString load() const noexcept {
		return load_();
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////
	/// @fn	void AbstractResourceManagementStrategy::save(const QString& newValue) const noexcept
	///
	/// @brief	Saves the given value into the managed resource.
	///
	/// @author	Krzysztof Sommerfeld
	/// @date	03.02.2020
	///
	/// @param 	newValue	The new value to save.
	////////////////////////////////////////////////////////////////////////////////////////////////////

	void save(const QString& newValue) const noexcept {
		save_(newValue);
	}
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// @class	QSettingsResourceManagment
///
/// @brief	Uses QSettings class for resource value management.
///
/// @author	Krzysztof Sommerfeld
/// @date	03.02.2020
////////////////////////////////////////////////////////////////////////////////////////////////////

class QSettingsResourceManagment : public AbstractResourceManagementStrategy {
public:
	QSettingsResourceManagment(const QString& resourceKey, const QString& defaultValue = "") noexcept {
		load_ = [resourceKey, defaultValue]() { return QSettings().value(resourceKey, defaultValue).toString(); };
		save_ = [resourceKey](const QString& value) { QSettings().setValue(resourceKey, value); };
	}
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// @class	CustomResourceManagment
///
/// @brief	A custom resource managment strategy.
///
/// @author	Krzysztof Sommerfeld
/// @date	03.02.2020
////////////////////////////////////////////////////////////////////////////////////////////////////

class CustomResourceManagment : public AbstractResourceManagementStrategy {
public:
	CustomResourceManagment(std::function<QString()>&& loadFunction, std::function<void(const QString&)>&& saveFunction) noexcept {
		load_ = std::move(loadFunction);
		save_ = std::move(saveFunction);
	}
};