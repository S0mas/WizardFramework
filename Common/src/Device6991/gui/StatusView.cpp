#include "../../../include/Device6991/gui/StatusView.h"
#include <QDateTime>
#include <QHBoxLayout>
#include <QString>
#include <QVBoxLayout>
#include "../../../include/Device6991/Defines6991.h"

StatusView::StatusView(QWidget * parent) : QWidget(parent) {
	auto layout = new QVBoxLayout;

	auto hlayout = new QHBoxLayout;
	hlayout->addWidget(idLabel_);
	hlayout->addWidget(idValue_);
	layout->addLayout(hlayout);

	hlayout = new QHBoxLayout;
	hlayout->addWidget(modeLabel_);
	hlayout->addWidget(modeValue_);
	layout->addLayout(hlayout);

	hlayout = new QHBoxLayout;
	hlayout->addWidget(dataStreamLabel_);
	hlayout->addWidget(dataStreamValue_);
	layout->addLayout(hlayout);

	hlayout = new QHBoxLayout;
	hlayout->addWidget(stateLabel_);
	hlayout->addWidget(stateValue_);
	layout->addLayout(hlayout);

	hlayout = new QHBoxLayout;
	hlayout->addWidget(currentTimeLabel_);
	hlayout->addWidget(currentTimeValue_);
	layout->addLayout(hlayout);

	setLayout(layout);
}

void StatusView::update(DeviceState const& state) noexcept {
	idValue_->setText("0xFF");
	modeValue_->setText("NONE");
	stateValue_->setText(DeviceStateEnum::toString(state.state()));
	QString list;
	int streamId = 1;
	for (auto streamState : state.linksConnectionStatus())
		streamState ? list.append(QString("[<font color=\"green\">%1</font>]").arg(streamId++)) : list.append(QString("[<font color=\"red\">%1</font>]").arg(streamId++));
	dataStreamValue_->setText(list);
	currentTimeValue_->setText(QDateTime::currentDateTime().toString("dd.MM.yyyy, hh:mm:ss"));
}