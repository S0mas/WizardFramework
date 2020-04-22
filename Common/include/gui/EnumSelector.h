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
	bool cutomEnabled_;
	void createConnections() const noexcept;
	bool isCustomSelected() const noexcept;
public:
	template<typename T>
	EnumSelector(T const& t, QString const& name, bool const cutomEnabled = false, int const inputwidth = 8, QWidget* parent = nullptr)
		: QWidget(parent), label_(new QLabel(name)), cutomEnabled_(cutomEnabled) {
		for (auto concreteEnum : T::TYPES)
			comboBox_->addItem(T::toString(concreteEnum), concreteEnum);

		label_->setMinimumWidth(50);
		label_->setMaximumWidth(50);

		auto layout = new QHBoxLayout;
		layout->addWidget(label_);
		layout->addWidget(comboBox_);
		if (cutomEnabled) {
			lineEdit_->setMaximumWidth(70);
			lineEdit_->setDisabled(true);
			lineEdit_->setInputMask(QString("\\0\\x%1;_").arg(QString(inputwidth, 'H')));
			layout->addWidget(lineEdit_);
			comboBox_->addItem("CUSTOM", 0x00);
		}

		setLayout(layout);
		createConnections();
	}
	unsigned int value() const noexcept;
signals:
	void enumChanged(int const concreteEnum) const;
};