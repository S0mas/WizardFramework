#pragma once
#include <QColor>
#include <QPaintEvent>
#include <QWidget>

class LedIndicator : public QWidget {
	Q_OBJECT
	bool state_;
	int size_;
	QColor onColor_;
	QColor offColor_;
	Qt::BrushStyle onPattern_;
	Qt::BrushStyle offPattern_;
public:
	LedIndicator(QWidget* parent = nullptr);
	void setState(bool const state);
	void setSize(int const size);
	void toggle();
	void setOnColor(QColor const onColor);
	void setOffColor(QColor const offColor);
	void setOnPattern(Qt::BrushStyle const onPattern);
	void setOffPattern(Qt::BrushStyle const offPattern);
public slots:
	void switchLedIndicator();
protected:
	void paintEvent(QPaintEvent*);
};