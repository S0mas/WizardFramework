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
	FecIdType::Type index_;
	void createConnections() noexcept;
	void initializeStateMachine() noexcept;
	EnumSelector* parentCommandSelector_;
signals:
	void stateReq(FecIdType::Type const fecId) const;
	void registerReadReq(FecIdType::Type const fecId, unsigned int const address) const;
	void registerWriteReq(FecIdType::Type const fecId, unsigned int const address, unsigned int const data) const;
private slots:
	void handleRegisterReadResp(FecIdType::Type const fecId, unsigned int const data) const;
public slots:
	void sendStateReq() const;
public:
	TargetFrontendCardView(EnumSelector* parentCommandSelector, Device6991* devIF, FecIdType::Type const index, QWidget* parent = nullptr);
	QLineEdit* dataLineEdit_ = new QLineEdit;
	void sendCommand(Commands1Enum::Type const cmd, unsigned int const address) noexcept;
};

class RegisterControllerFrontend : public QGroupBox {
	Q_OBJECT
	EnumSelector* commandSelector_ = new EnumSelector(Commands1Enum(), "Command");
	EnumSelector* addressSelector_ = new EnumSelector(FecRegistersEnum(), "Address", true, 8);
	TargetFrontendCardView* frontend1_;
	TargetFrontendCardView* frontend2_;
	QPushButton* refreshButton = new QPushButton("Refresh");
	QPushButton* executeButton = new QPushButton("Execute");
	void createConnections() noexcept;
public:
	RegisterControllerFrontend(Device6991* devIF, QWidget* parent = nullptr);
};

class RegisterController6991 : public QGroupBox {
	Q_OBJECT
	EnumSelector* commandSelector_ = new EnumSelector(Commands2Enum(), "Command");
	EnumSelector* addressSelector_ = new EnumSelector(RegistersEnum(), "Address", true, 8);
	QLineEdit* dataLineEdit_ = new QLineEdit;
	QPushButton* executeButton = new QPushButton("Execute");
	Device6991* deviceIF_;
	void createConnections() noexcept;
private slots:
	void handleRegisterReadResp(unsigned int const data) const;
signals:
	void registerReadReq(unsigned int const address) const;
	void registerWriteReq(unsigned int const address, unsigned int const data) const;
public:
	RegisterController6991(Device6991* devIF, QWidget* parent = nullptr);
};