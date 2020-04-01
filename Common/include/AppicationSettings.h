#pragma once
#include <QObject>
#include <QSettings>
#include <memory>

////////////////////////////////////////////////////////////////////////////////////////////////////
/// @class	ApplicationSettings
///
/// @brief	Responsible for saving and loading application-wide values and storing them in the system.
///
/// @author	Krzysztof Sommerfeld
/// @date	03.02.2020
////////////////////////////////////////////////////////////////////////////////////////////////////

class ApplicationSettings : public QObject {
	Q_OBJECT
	QSettings settings_;
public:

	////////////////////////////////////////////////////////////////////////////////////////////////////
	/// @fn	ApplicationSettings::ApplicationSettings(QObject* parent = nullptr)
	///
	/// @brief	Constructor
	///
	/// @author	Krzysztof Sommerfeld
	/// @date	03.02.2020
	///
	/// @param [in]	parent	(Optional) If not null - will be set as constructed object parent. In the
	/// 					QObject tree.
	////////////////////////////////////////////////////////////////////////////////////////////////////

	ApplicationSettings(QObject* parent = nullptr) : QObject(parent) {}

	////////////////////////////////////////////////////////////////////////////////////////////////////
	/// @fn	ApplicationSettings::~ApplicationSettings() override = default;
	///
	/// @brief	Default destructor
	///
	/// @author	Krzysztof Sommerfeld
	/// @date	03.02.2020
	////////////////////////////////////////////////////////////////////////////////////////////////////

	~ApplicationSettings() override = default;

	////////////////////////////////////////////////////////////////////////////////////////////////////
	/// @fn	Q_INVOKABLE QString ApplicationSettings::get(const QString& paramName, const QString& defaultvalue) const noexcept
	///
	/// @brief	Gets the resource value if it exists. Default value otherwise.
	///
	/// @author	Krzysztof Sommerfeld
	/// @date	03.02.2020
	///
	/// @param 	paramName   	Name of the parameter.
	/// @param 	defaultvalue	The default value.
	///
	/// @returns	The resource value or default value.
	////////////////////////////////////////////////////////////////////////////////////////////////////

	Q_INVOKABLE QString get(const QString& paramName, const QString& defaultvalue) const noexcept {
		return settings_.value(paramName, defaultvalue).toString();
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////
	/// @fn	Q_INVOKABLE void ApplicationSettings::set(const QString& paramName, const QString& value) noexcept
	///
	/// @brief	Sets the resource value
	///
	/// @author	Krzysztof Sommerfeld
	/// @date	03.02.2020
	///
	/// @param 	paramName	Name of the parameter.
	/// @param 	value	 	The value.
	////////////////////////////////////////////////////////////////////////////////////////////////////

	Q_INVOKABLE void set(const QString& paramName, const QString& value) noexcept {
		settings_.setValue(paramName, value);
	}
};