#pragma once
#include <QPushButton>
#include <QStateMachine>
#include <QTimer>

#include <functional>

class TwoStateButton : public QPushButton {
	Q_OBJECT
	QStateMachine* sm_ = new QStateMachine(this);
	std::function<void(void)> action_;
	std::function<void(void)> action1_;
	std::function<void(void)> action2_;
	QState* state1_ = new QState;
	QState* state2_ = new QState;
	QState* processing_ = new QState;
	QTimer* timer1_ = new QTimer;
	QTimer* timer2_ = new QTimer;
signals:
	void actionStarted() const;
public:
	TwoStateButton(QString const& actionText1, std::function<void(void)> const& action1, QString const& actionText2, std::function<void(void)> const& action2, QWidget* parent = nullptr);


signals:
	void stateChange1() const;
	void stateChange2() const;
};

