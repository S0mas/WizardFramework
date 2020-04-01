#pragma once
#include <QString>
#include <functional>
#include <QHostAddress>

////////////////////////////////////////////////////////////////////////////////////////////////////
/// @class	AbstractResourceContentValidationStrategy
///
/// @brief	An abstract resource content validation strategy interface. It should be inherited
/// 		and implemented to use some kind of validation strategy for the resource that uses it.
///
/// @author	Krzysztof Sommerfeld
/// @date	03.02.2020
////////////////////////////////////////////////////////////////////////////////////////////////////

class AbstractResourceContentValidationStrategy {
protected:
	QString inputMask_;
	QString defaultValue_;
	std::function<bool(const QString&)> validate_ = nullptr;
public:
	////////////////////////////////////////////////////////////////////////////////////////////////////
	/// @fn	AbstractResourceContentValidationStrategy::AbstractResourceContentValidationStrategy() noexcept = default;
	///
	/// @brief	Default constructor
	///
	/// @author	Krzysztof Sommerfeld
	/// @date	03.02.2020
	////////////////////////////////////////////////////////////////////////////////////////////////////

	AbstractResourceContentValidationStrategy() noexcept = default;

	////////////////////////////////////////////////////////////////////////////////////////////////////
	/// @fn	AbstractResourceContentValidationStrategy::AbstractResourceContentValidationStrategy(const AbstractResourceContentValidationStrategy& original) noexcept
	///
	/// @brief	Copy constructor
	///
	/// @author	Krzysztof Sommerfeld
	/// @date	03.02.2020
	///
	/// @param 	original	The original to copy.
	////////////////////////////////////////////////////////////////////////////////////////////////////

	AbstractResourceContentValidationStrategy(const AbstractResourceContentValidationStrategy& original) noexcept
		: validate_(original.validate_) {}

	////////////////////////////////////////////////////////////////////////////////////////////////////
	/// @fn	virtual AbstractResourceContentValidationStrategy::~AbstractResourceContentValidationStrategy() = default;
	///
	/// @brief	Default destructor
	///
	/// @author	Krzysztof Sommerfeld
	/// @date	03.02.2020
	////////////////////////////////////////////////////////////////////////////////////////////////////

	virtual ~AbstractResourceContentValidationStrategy() = default;

	////////////////////////////////////////////////////////////////////////////////////////////////////
	/// @fn	QString AbstractResourceContentValidationStrategy::inputMask() const noexcept
	///
	/// @brief	Gets the input mask.
	///
	/// @author	Krzysztof Sommerfeld
	/// @date	03.02.2020
	///
	/// @returns	The input mask for the content.
	////////////////////////////////////////////////////////////////////////////////////////////////////

	QString inputMask() const noexcept {
		return inputMask_;
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////
	/// @fn	QString AbstractResourceContentValidationStrategy::defaultValue() const noexcept
	///
	/// @brief	Gets the default value.
	///
	/// @author	Krzysztof Sommerfeld
	/// @date	03.02.2020
	///
	/// @returns	The default value of the content.
	////////////////////////////////////////////////////////////////////////////////////////////////////

	QString defaultValue() const noexcept {
		return inputMask_;
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////
	/// @fn	bool AbstractResourceContentValidationStrategy::validate(const QString& candidate) const noexcept
	///
	/// @brief	Validates, if the given candidate me the validation requirements.
	///
	/// @author	Krzysztof Sommerfeld
	/// @date	03.02.2020
	///
	/// @param 	candidate	The candidate.
	///
	/// @returns	True if it succeeds, false if it fails.
	////////////////////////////////////////////////////////////////////////////////////////////////////

	bool validate(const QString& candidate) const noexcept {
		return validate_(candidate);
	}
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// @class	NoValidation
///
/// @brief	A no validation strategy. It simply always returns true from validation function.
///
/// @author	Krzysztof Sommerfeld
/// @date	03.02.2020
////////////////////////////////////////////////////////////////////////////////////////////////////

class NoValidation : public AbstractResourceContentValidationStrategy {
public:
	NoValidation() noexcept {
		validate_ = [](const QString& proposedValue) {
			return true;
		};
		inputMask_ = "";
		defaultValue_ = "";
	}
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// @class	IPContentValidation
///
/// @brief	An IP content validation strategy.
///
/// @author	Krzysztof Sommerfeld
/// @date	03.02.2020
////////////////////////////////////////////////////////////////////////////////////////////////////

class IPContentValidation : public AbstractResourceContentValidationStrategy {
public:
	IPContentValidation() noexcept {
		validate_ = [](const QString& proposedValue) {
			QHostAddress address;
			return address.setAddress(proposedValue) && address.toString() == proposedValue;
		};
		inputMask_ = "000.000.000.000;";
		defaultValue_ = "0.0.0.0";
	}
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// @class	FunctionCardIdContentValidation
///
/// @brief	A function card identifier content validation strategy.
///
/// @author	Krzysztof Sommerfeld
/// @date	03.02.2020
////////////////////////////////////////////////////////////////////////////////////////////////////

class FunctionCardIdContentValidation : public AbstractResourceContentValidationStrategy {
public:
	FunctionCardIdContentValidation() noexcept {
		validate_ = [](const QString& proposedValue) {
			return proposedValue.toInt() > 0 && proposedValue.toInt() < 5;
		};
		inputMask_ = "0;_";
		defaultValue_ = "1";
	}
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// @class	SubtypeContentValidation
///
/// @brief	A subtype content validation strategy.
///
/// @author	Krzysztof Sommerfeld
/// @date	03.02.2020
////////////////////////////////////////////////////////////////////////////////////////////////////

class SubtypeContentValidation : public AbstractResourceContentValidationStrategy {
public:
	SubtypeContentValidation() noexcept {
		validate_ = [](const QString& proposedValue) {
			return proposedValue.size() == 2;
		};
		inputMask_ = ">AA;_";
		defaultValue_ = "XX";
	}
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// @class	SerialnumberContentValidation
///
/// @brief	A serialnumber content validation strategy.
///
/// @author	Krzysztof Sommerfeld
/// @date	03.02.2020
////////////////////////////////////////////////////////////////////////////////////////////////////

class SerialnumberContentValidation : public AbstractResourceContentValidationStrategy {
public:
	SerialnumberContentValidation() noexcept {
		validate_ = [](const QString& proposedValue) {
			return proposedValue.size() == 8;
		};
		inputMask_ = "99999999;_";
		defaultValue_ = "00000000";
	}
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// @class	DriverRevisionContentValidation
///
/// @brief	A driver revision content validation strategy.
///
/// @author	Krzysztof Sommerfeld
/// @date	03.02.2020
////////////////////////////////////////////////////////////////////////////////////////////////////

class DriverRevisionContentValidation : public AbstractResourceContentValidationStrategy {
public:
	DriverRevisionContentValidation() noexcept {
		validate_ = [](const QString& proposedValue) {
			return true;
		};
		inputMask_ = "";
		defaultValue_ = "";
	}
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// @class	FrimwareRevisionContentValidation
///
/// @brief	A frimware revision content validation strategy.
///
/// @author	Krzysztof Sommerfeld
/// @date	03.02.2020
////////////////////////////////////////////////////////////////////////////////////////////////////

class FrimwareRevisionContentValidation : public AbstractResourceContentValidationStrategy {
public:
	FrimwareRevisionContentValidation() noexcept {
		validate_ = [](const QString& proposedValue) {
			return true;
		};
		inputMask_ = "";
		defaultValue_ = "";
	}
};