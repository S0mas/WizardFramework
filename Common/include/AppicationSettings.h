#pragma once
#include <QObject>
#include <QSettings>
#include <memory>
#include <QDebug>

class ApplicationSettings : public QObject {
	Q_OBJECT
public:
	std::unique_ptr<QSettings> settings;

	ApplicationSettings(const QString& applicationName) : settings(std::make_unique<QSettings>("Bustec", applicationName)) {}

	Q_INVOKABLE QString get(const QString& paramName, const QString& defaultValue) const noexcept {
		return settings->value(paramName, defaultValue).toString();
	}
	
	Q_INVOKABLE void set(const QString& paramName, const QString& value) noexcept {
		settings->setValue(paramName, value);
	}
};