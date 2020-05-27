#pragma once
#include <QGroupBox>
#include <QLabel>
#include <QWidget>

class DeviceState;

class StatusView : public QGroupBox {
	Q_OBJECT
	QLabel* masterIdLabel_ = new QLabel("Master Id:");
	QLabel* masterIdValue_ = new QLabel;
	QLabel* dataStreamLabel_ = new QLabel("Data Stream:");
	QLabel* dataStreamValue_ = new QLabel;
	QLabel* stateLabel_ = new QLabel("State:");
	QLabel* stateValue_ = new QLabel;
	QLabel* currentTimeLabel_ = new QLabel("Current Time:");
	QLabel* currentTimeValue_ = new QLabel;
public:
	StatusView(QWidget* parent = nullptr);
public slots:
	void update(DeviceState const& status) noexcept;
};
