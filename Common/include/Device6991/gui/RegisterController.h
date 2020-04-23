#pragma once
#include <QWidget>
#include <QGroupBox>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QComboBox>
#include <QCheckBox>
#include <QPushButton>
#include <QDebug>

#include "../Command.h"
#include "../Device6991.h"
#include "../CommandTypes.h"
#include "../../CustomTransitions.h"
#include "../../gui/EnumSelector.h"
#include "../RegisterTypes.h"

class TargetFrontendCardView : public QGroupBox {
	Q_OBJECT
	QCheckBox* checkBox_ = new QCheckBox;
	QStateMachine sm_;
	Device6991* deviceIF_;
	int index_;
	void createConnections() noexcept;
	void initializeStateMachine() noexcept;
	EnumSelector* parentCommandSelector_;
signals:
	void stateReq() const;
public:
	TargetFrontendCardView(EnumSelector* parentCommandSelector, AbstractHardwareConnector* hwConnector, ScpiIF* scpiIF, int const index, QWidget* parent = nullptr);
	QLineEdit* lineEdit_ = new QLineEdit;
public slots:
	void sendCommand(Commands1Enum::Type const cmd, uint32_t const address) noexcept;
};

class RegisterControllerFrontend : public QGroupBox {
	Q_OBJECT
	EnumSelector* commandSelector_ = new EnumSelector(Commands1Enum(), "Command");
	EnumSelector* addressSelector_ = new EnumSelector(RegistersEnum(), "Address", true, 8);
	TargetFrontendCardView* frontend1_;
	TargetFrontendCardView* frontend2_;
	QPushButton* refreshButton = new QPushButton("Refresh");
	QPushButton* executeButton = new QPushButton("Execute");
	void createConnections() noexcept;
public:
	RegisterControllerFrontend(AbstractHardwareConnector* hwConnector, ScpiIF* scpiIF, QWidget* parent = nullptr);
};

class RegisterController6991 : public QGroupBox {
	Q_OBJECT
	EnumSelector* commandSelector_ = new EnumSelector(Commands2Enum(), "Command");
	EnumSelector* addressSelector_ = new EnumSelector(RegistersEnum(), "Address", true, 8);
	QLineEdit* lineEdit_ = new QLineEdit;
	QPushButton* executeButton = new QPushButton("Execute");
	Device6991* deviceIF_;
	void createConnections() noexcept;
public:
	RegisterController6991(AbstractHardwareConnector* hwConnector, ScpiIF* scpiIF, QWidget* parent = nullptr);
};