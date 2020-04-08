#include "../../include/gui/EnumSelector.h"

#include <QHBoxLayout>
#include <QLabel>

void EnumSelector::createConnections() const noexcept {
	QObject::connect(comboBox_, QOverload<int>::of(&QComboBox::currentIndexChanged),
		[this](int const index) {
			//lineEdit_->setText(comboBox_->toHex());
			//comboBox_->isCustomSelected() ? lineEdit_->setEnabled(true) : lineEdit_->setDisabled(true);
			emit enumChanged(comboBox_->currentData().toInt());
		}
	);
	emit comboBox_->currentIndexChanged(comboBox_->currentIndex());
}

unsigned int EnumSelector::value() const noexcept {
	return comboBox_->currentData().toInt();
}
