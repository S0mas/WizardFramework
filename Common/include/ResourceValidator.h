#pragma once
#include <QValidator>
#include <functional>

class ResourceValidator : public QValidator {
	std::function<bool(QString const&)> validator_;
public:
	ResourceValidator(std::function<bool(QString const&)> const& validator, QObject* parent = nullptr) : QValidator(parent), validator_(validator) {}

	QValidator::State validate(QString& input, int& pos) const {
		return validator_(input) ? State::Acceptable : State::Invalid;
	}
};