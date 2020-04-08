#pragma once
#include <memory>
#include <vector>
#include <QString>
#include <QComboBox>
#include <QWidget>

class EnumComboBox : public QComboBox {
	Q_OBJECT
public:
	template<typename T>
	EnumComboBox(T const&, QWidget* parent = nullptr) : QComboBox(parent) {
		for (auto concreteEnum : T::TYPES)
			addItem(T::toString(concreteEnum), concreteEnum);
	}
};