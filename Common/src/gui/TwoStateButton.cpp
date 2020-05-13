#include "../../include/gui/TwoStateButton.h"
#include <QDebug>

TwoStateButton::TwoStateButton(QString const& actionText1, std::function<void(void)> const& action1, QString const& actionText2, std::function<void(void)> const& action2, QWidget* parent)
	: QPushButton("", parent),
	action1_(action1),
	action2_(action2) {
	timer1_->setSingleShot(true);
	timer2_->setSingleShot(true);
	connect(this, &QPushButton::clicked, [this]() { action_(); });

	connect(state1_, &QState::entered,
		[this, actionText2]() {
			timer1_->stop();
			action_ = [this]() {
				timer2_->start(5000);
				emit actionStarted();
				action2_();
			};
			setText(actionText2);
		}
	);

	connect(state2_, &QState::entered,
		[this, actionText1]() {
			timer2_->stop();
			action_ = [this]() {
				timer1_->start(5000);
				emit actionStarted();
				action1_();
			};
			setText(actionText1);
		}
	);

	connect(processing_, &QState::entered,
		[this]() {
			action_ = []() {};
			setText("Processing...");
		}
	);

	state2_->addTransition(this, &TwoStateButton::actionStarted, processing_);
	state2_->addTransition(this, &TwoStateButton::stateChange1, state1_);
	processing_->addTransition(this, &TwoStateButton::stateChange1, state1_);
	processing_->addTransition(this, &TwoStateButton::stateChange2, state2_);
	processing_->addTransition(timer1_, &QTimer::timeout, state2_);
	processing_->addTransition(timer2_, &QTimer::timeout, state1_);
	state1_->addTransition(this, &TwoStateButton::actionStarted, processing_);
	state1_->addTransition(this, &TwoStateButton::stateChange2, state2_);

	sm_->addState(state1_);
	sm_->addState(state2_);
	sm_->addState(processing_);
	sm_->setInitialState(state2_);
	sm_->start();
}
