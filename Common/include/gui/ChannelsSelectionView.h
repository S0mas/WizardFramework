#pragma once
#include <QGridLayout>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGroupBox>
#include <QPushButton>
#include "ChannelsGroupSelectionButton.h"
#include "ChannelStatus.h"

class ChannelsSelectionView : public QGroupBox {
	Q_OBJECT
	int channelsPerGroup_;
	QVector<ChannelsGroupSelectionButton*> groupsButtons_;
public:
	ChannelsSelectionView(ChannelStatuses& statuses, const int groupsNo, const QString& groupDesc, int const groupsPerRow = 4, QWidget* parent = nullptr) : QGroupBox(groupDesc, parent) {
		auto vlayout = new QVBoxLayout;
		QHBoxLayout* hlayout = nullptr;
		auto selectAllButton = new QPushButton("Select All", this);
		auto deselectAllButton = new QPushButton("Deselect All", this);
		channelsPerGroup_ = statuses.size() / groupsNo;
		for (int groupId = 1; groupId <= groupsNo; ++groupId) {
			QVector<ChannelSelectionButton*> buttons;
			for (int i = 0; i < channelsPerGroup_; ++i) {
				ChannelId id = (groupId - 1) * channelsPerGroup_ + 1 + i;
				auto button = new ChannelSelectionButton(id, this);
				connect(button, &ChannelSelectionButton::toggled, [&statuses, id](const bool state) { statuses.set(id, state); });
				buttons.push_back(button);
			}
			
			if (groupId % groupsPerRow == 1) {
				if (hlayout)
					vlayout->addLayout(hlayout);
				hlayout = new QHBoxLayout;
			}
			
			auto groupConfigButton = new ChannelsGroupSelectionButton(groupId, buttons, this);
			connect(selectAllButton, &QPushButton::clicked, groupConfigButton, &ChannelsGroupSelectionButton::select);
			connect(deselectAllButton, &QPushButton::clicked, groupConfigButton, &ChannelsGroupSelectionButton::deselect);
			hlayout->addWidget(groupConfigButton);
			groupsButtons_.push_back(groupConfigButton);
			if(groupId == groupsNo && hlayout)
				vlayout->addLayout(hlayout);
		}

		hlayout = new QHBoxLayout;
		hlayout->addWidget(selectAllButton);
		hlayout->addWidget(deselectAllButton);
		vlayout->addLayout(hlayout);

		setLayout(vlayout);
	}

	QVector<ChannelsGroupSelectionButton*> groupsButtons() {
		return groupsButtons_;
	}


};