#pragma once
#include "Device6991/Device6991.h"

class Device6111 : public QObject {
	Q_OBJECT
	FecIdType::Type id_;
	Device6991* devIF_;
public:
	Device6111(const FecIdType::Type& fcId, Device6991* dev6991, QObject* parent = nullptr) noexcept : devIF_(dev6991) {}
	~Device6111() = default;
};

