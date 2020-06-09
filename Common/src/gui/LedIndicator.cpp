#include "../../include/gui/LedIndicator.h"
#include <QPainter>

LedIndicator::LedIndicator(QWidget* parent) :
	QWidget(parent) {
	setFixedSize(28, 28);
	state_ = false;
	onColor_ = Qt::green;
	offColor_ = Qt::red;
	onPattern_ = Qt::SolidPattern;
	offPattern_ = Qt::SolidPattern;
	size_ = 20;
}

void LedIndicator::paintEvent(QPaintEvent*) {
	QPainter p(this);
	state_ ? p.setBrush(QBrush(onColor_, onPattern_)) : p.setBrush(QBrush(offColor_, offPattern_));
	p.drawEllipse(0, 0, size_, size_);
}

void LedIndicator::switchLedIndicator() {
	state_ = !state_;
	repaint();
}
void LedIndicator::setState(bool const state) {
	state_ = state;
	repaint();
}
void LedIndicator::toggle() {
	state_ = !state_;
	repaint();
}

void LedIndicator::setOnColor(QColor const onColor) {
	onColor_ = onColor;
	repaint();
}
void LedIndicator::setOffColor(QColor const offColor) {
	offColor_ = offColor;
	repaint();
}
void LedIndicator::setOnPattern(Qt::BrushStyle const onPattern) {
	onPattern_ = onPattern;
	repaint();
}
void LedIndicator::setOffPattern(Qt::BrushStyle const offPattern) {
	offPattern_ = offPattern;
	repaint();
}
void LedIndicator::setSize(int const size) {
	size_ = size;
	setFixedSize(size + 10, size + 10);
	repaint();
}