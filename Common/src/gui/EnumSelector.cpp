#include "../../include/gui/EnumSelector.h"
#include "../../include/Device6991/Defines6991.h"
#include <QHBoxLayout>
#include <QLabel>

void EnumSelector::createConnections() const noexcept {
	QObject::connect(comboBox_, static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
		[this](int const index) {
			lineEdit_->setText(toHex(comboBox_->currentData().toUInt(), 8));
			isCustomSelected() ? lineEdit_->setEnabled(true) : lineEdit_->setDisabled(true);
			emit enumChanged(comboBox_->currentData().toInt());
		}
	);
	emit comboBox_->currentIndexChanged(comboBox_->currentIndex());
}

bool EnumSelector::isCustomSelected() const noexcept {
	return cutomEnabled_ && comboBox_->currentText() == "CUSTOM";
}

uint32_t EnumSelector::value() const noexcept {
	return isCustomSelected() ? lineEdit_->text().toUInt(nullptr, 16) : comboBox_->currentData().toUInt();
}
