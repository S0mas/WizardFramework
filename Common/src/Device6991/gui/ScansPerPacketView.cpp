#include "../../../include/Device6991/gui/ScansPerPacketView.h"
#include "../../../include/Device6991/Defines6991.h"
#include <QHBoxLayout>

ScansPerPacketView::ScansPerPacketView(QWidget* parent) : QGroupBox("Scan Per Packet", parent) {
	valueSpinBox_->setMaximumWidth(50);
	valueSpinBox_->setMinimumWidth(50);
	valueSpinBox_->setSingleStep(1);
	valueSpinBox_->setRange(1, LIMIT);

	auto layout = new QHBoxLayout;
	layout->addWidget(valueSpinBox_);

	setLayout(layout);
}

uint32_t ScansPerPacketView::value() const noexcept {
	return valueSpinBox_->value();
}

void ScansPerPacketView::setValue(uint32_t const value) noexcept {
	valueSpinBox_->setValue(value);
}
