#pragma once
#include "Device6991/Device6991.h"

class Device6111 : public QObject {
	Q_OBJECT
	Device6991* devIF_;
public:
	Device6111(Device6991* dev6991, QObject* parent = nullptr) noexcept : devIF_(dev6991) {}
	~Device6111() = default;
};

