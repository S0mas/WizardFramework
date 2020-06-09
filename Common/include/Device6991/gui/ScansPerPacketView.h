#pragma once
#include <QGroupBox>
#include <QComboBox>
#include <QSpinBox>

class ScansPerPacketView : public QGroupBox {
	Q_OBJECT
	int const LIMIT = 512;
	QSpinBox* valueSpinBox_ = new QSpinBox;
public:
	ScansPerPacketView(QWidget* parent = nullptr);
	uint32_t value() const noexcept;
	void setValue(uint32_t const value) noexcept;
};