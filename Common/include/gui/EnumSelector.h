#pragma once
#include <QComboBox>
#include <QLineEdit>
#include <memory>

class QLabel;
class SmartEnum;

class EnumSelector : public QWidget {
	Q_OBJECT
	QLabel* label_;
	QComboBox* comboBox_ = new QComboBox;
	QLineEdit* lineEdit_ = new QLineEdit;
	void createConnections() const noexcept;
public:
	template<typename T>
	EnumSelector(T const& t, QString const& name, bool const translatesToOtherValue = false, QWidget* parent = nullptr)
		: QWidget(parent), label_(new QLabel(name)) {
		for (auto concreteEnum : T::TYPES)
			comboBox_->addItem(T::toString(concreteEnum), concreteEnum);

		label_->setMinimumWidth(50);
		label_->setMaximumWidth(50);

		auto layout = new QHBoxLayout;
		layout->addWidget(label_);
		layout->addWidget(comboBox_);
		//if (translatesToOtherValue) {
		//	lineEdit_->setMaximumWidth(50);
		//	lineEdit_->setDisabled(true);
		//	lineEdit_->setInputMask(comboBox_->inputMask());
		//	layout->addWidget(lineEdit_);
		//}

		setLayout(layout);
		createConnections();
	}
	unsigned int value() const noexcept;
signals:
	void enumChanged(int const concreteEnum) const;
};