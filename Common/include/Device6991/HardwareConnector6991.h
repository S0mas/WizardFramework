#pragma once
#include "../AbstractHardwareConnector.h"
#include "../Resource.h"
#include <string>
#include <memory>
#include <QDebug>
#include <visa.h>

class HardwareConnector6991 : public AbstractHardwareConnector {
	std::shared_ptr<AbstractDataResource> ipResource;
	unsigned long rmVi_ = 0;
	std::string resource() const noexcept {
		return QString("TCPIP::%1::INSTR").arg(ipResource->value()).toStdString(); 
	}
	bool isConnectionPossible() const override { 
		ViSession rmSession = 0;
		ViSession session = 0;
		auto result = innerCall(viOpenDefaultRM, "viOpenDefaultRM", &rmSession) >= 0 && innerCall(viOpen, "viOpen", rmSession, resource().data(), 0, 0, &session) >= 0;
		if (session > 0)
			innerCall(viClose, "viClose", session);
		if (rmSession > 0)
			innerCall(viClose, "viClose", rmSession);
		return result;
	}	
	void connectImpl() noexcept override {
		innerCall(viOpenDefaultRM, "viOpenDefaultRM", &rmVi_) && innerCall(viOpen, "viOpen", rmVi_, resource().data(), 0, 0, &vi_);
	}
	void disconnectImpl() noexcept override {
		innerCall(viClose, "viClose", vi_);
		innerCall(viClose, "viClose", rmVi_);
	}
public:
	////////////////////////////////////////////////////////////////////////////////////////////////////
	/// @fn	HardwareConnector6991::HardwareConnector6991(const QString& nameId, QObject* parent = nullptr) noexcept;
	///
	/// @brief	Constructor
	///
	/// @author	Krzysztof Sommerfeld
	/// @date	05.02.2020
	///
	/// @param 		   	nameId			 	The name identifier.
	/// @param [in]	parent	(Optional) If not null - will be set as constructed object parent. In the
	/// 					QObject tree.
	////////////////////////////////////////////////////////////////////////////////////////////////////

	HardwareConnector6991(const QString& nameId, QObject* parent = nullptr) noexcept {
		errorChecker = std::make_unique<ErrorChecker>([](auto a, auto b, auto c) { return 0; } );
	}
	~HardwareConnector6991() override = default;
	std::vector<std::shared_ptr<AbstractDataResource>> resources() const noexcept override {
		return { ipResource }; 
	}
};