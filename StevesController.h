#pragma once

#include <QWidget>
#include <QCheckBox>
#include <QComboBox>
#include <QGroupBox>
#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QTimer>
#include <QMessageBox>
#include <QLabel>
#include <QLineEdit>
#include "../WizardFramework/Common/include/Device6132.h"

enum ConfigAction {
	SET_GAIN,
	SET_FILTER
};

enum Target {
	ALL = 0
};

class StevesController : public QWidget {
	Q_OBJECT
	Device6132* devIF_;
	QPushButton* measureButton_ = new QPushButton("Single Conversion");
	QPushButton* startStopAcqButton_ = new QPushButton("Continous Conversion");
	QGroupBox* setterGroup_ = new QGroupBox("Configure");
	QTimer* acqTimer_ = new QTimer(this);
	std::vector<bool> enabledChannels_;
	std::vector<uint32_t> allChannels = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16 };

	void doMeasurement() {
		std::vector<uint32_t> channels;
		for (auto chId : allChannels)
			if (enabledChannels_[chId - 1])
				channels.push_back(chId);
		if (auto result = devIF_->measure(channels); result)
			emit measurementUpdate(*result);
	}

	void fetchConfig() {
		std::vector<std::optional<GainType6132::Type>> gains;
		for (auto channel : allChannels)
			gains.push_back(devIF_->gain(channel));
		emit gainsUpdate(gains);
		std::vector<std::optional<FilterType6132::Type>> filters;
		for (auto channel : allChannels)
			filters.push_back(devIF_->filter(channel));
		emit filtersUpdate(filters);
	}
signals:
	void measurementUpdate(std::vector<std::optional<double>> const& values);
	void gainsUpdate(std::vector<std::optional<GainType6132::Type>> const& values);
	void filtersUpdate(std::vector<std::optional<FilterType6132::Type>> const& values);
public:
	StevesController(Device6132* devIF, QWidget* parent = nullptr) : QWidget(parent), devIF_(devIF) {
		connect(measureButton_, &QPushButton::clicked, this, &StevesController::doMeasurement);
		connect(acqTimer_, &QTimer::timeout, this, &StevesController::doMeasurement);
		connect(startStopAcqButton_, &QPushButton::clicked, 
			[this]() {
				if (startStopAcqButton_->text() == "Continous Conversion") {
					fetchConfig();
					std::vector<uint32_t> channels;
					for (auto chId : allChannels)
						if (enabledChannels_[chId - 1])
							channels.push_back(chId);
					if (!channels.empty()) {
						acqTimer_->start(100);
						startStopAcqButton_->setText("Stop");
						measureButton_->setDisabled(true);
						setterGroup_->setDisabled(true);
					}
				}
				else {
					acqTimer_->stop();
					startStopAcqButton_->setText("Continous Conversion");
					measureButton_->setEnabled(true);
					setterGroup_->setEnabled(true);
				}
			}
		);

		connect(this, &StevesController::stopMeasurement, 
			[this]() { 
				if (startStopAcqButton_->text() == "Stop") {
					acqTimer_->stop();
					startStopAcqButton_->setText("Continous Conversion");
					measureButton_->setEnabled(true);
					setterGroup_->setEnabled(true);
		
				}
			}
		);

		auto comboBoxAction = new QComboBox;
		comboBoxAction->addItem("SET_GAIN", SET_GAIN);
		comboBoxAction->addItem("SET_FILTER", SET_FILTER);
		auto comboBoxValue = new QComboBox;
		auto comboBoxTarget = new QComboBox;
		comboBoxTarget->addItem("ALL_CHANNELS", 0);
		for (auto channel : allChannels)
			comboBoxTarget->addItem(QString("%1").arg(channel), channel);
		auto setButton = new QPushButton("Set");

		auto hlayout = new QHBoxLayout;
		hlayout->addWidget(comboBoxAction);
		hlayout->addWidget(comboBoxValue);
		hlayout->addWidget(comboBoxTarget);
		hlayout->addWidget(setButton);
		setterGroup_->setLayout(hlayout);

		connect(comboBoxAction, QOverload<QString const&>::of(&QComboBox::currentIndexChanged),
			[comboBoxValue](QString const& action) {
				comboBoxValue->clear();
				if (action == "SET_GAIN") 
					for (auto gain : GainType6132::TYPES)
						comboBoxValue->addItem(GainType6132::toString(gain), gain);
				if (action == "SET_FILTER")
					for (auto filter : FilterType6132::TYPES)
						comboBoxValue->addItem(FilterType6132::toString(filter), filter);			
			}
		);
		emit comboBoxAction->currentIndexChanged(comboBoxAction->currentText());

		connect(setButton, &QPushButton::clicked,
			[comboBoxAction, comboBoxValue, comboBoxTarget, this]() {
				std::vector<uint32_t> channels = comboBoxTarget->currentData().toUInt() == 0 ? allChannels : std::vector<uint32_t>{comboBoxTarget->currentData().toUInt()};
				bool result = false;
				if (comboBoxAction->currentData().toUInt() == SET_GAIN)
					result = devIF_->setGain(channels, static_cast<GainType6132::Type>(comboBoxValue->currentData().toUInt()));
				else if(comboBoxAction->currentData().toUInt() == SET_FILTER) 
					result = devIF_->setFilter(channels, static_cast<FilterType6132::Type>(comboBoxValue->currentData().toUInt()));
				if (!result)
					QMessageBox::critical(this, "Error", QString("Faild to %1.").arg(comboBoxAction->currentText()));
				else
					fetchConfig();
			}
		);
		auto checkAllButton = new QPushButton("Select all");
		auto uncheckAllButton = new QPushButton("Deselect all");
		
		auto getRow = [thisObject = this, this, checkAllButton, uncheckAllButton](int id) {
			enabledChannels_.push_back(true);
			auto hlayout = new QHBoxLayout;
			auto channelIdLabel = new QLabel(QString("Channel %1").arg(id));
			auto gain = new QLabel("NO DATA");
			auto filter = new QLabel("NO DATA");
			auto checkBox = new QCheckBox;
			checkBox->setChecked(true);
			connect(checkAllButton, &QPushButton::clicked, [checkBox]() {checkBox->setChecked(true); });
			connect(uncheckAllButton, &QPushButton::clicked, [checkBox]() {checkBox->setChecked(false); });
			auto measurement = new QLabel("THERE WAS NO MEASUREMENT");

			QObject::connect(thisObject, &StevesController::measurementUpdate,
				[id, measurement, thisObject](auto const& values) {
					if (thisObject->enabledChannels_[id - 1])
						if(values[id - 1])
							measurement->setText(QString("%1V").arg(*values[id - 1], 10, 'f', 7));
						else
							measurement->setText("ERR");
					else
						measurement->setText("OFF");
				}
			);

			QObject::connect(thisObject, &StevesController::gainsUpdate,
				[id, gain, thisObject](auto const& values) {
					if (values[id - 1])
						gain->setText(GainType6132::toString(*values[id - 1]));
					else
						gain->setText("ERR");
				}
			);

			QObject::connect(thisObject, &StevesController::filtersUpdate,
				[id, filter, thisObject](auto const& values) {
					if (values[id - 1])
						filter->setText(FilterType6132::toString(*values[id - 1]));
					else
						filter->setText("ERR");
				}
			);

			QObject::connect(checkBox, &QCheckBox::stateChanged, 
				[thisObject, id](auto const state) {
					thisObject->enabledChannels_[id - 1] = state == Qt::Checked;
				}
			);

			hlayout->addWidget(channelIdLabel);
			hlayout->addWidget(gain);
			hlayout->addWidget(filter);
			hlayout->addWidget(checkBox);
			hlayout->addWidget(measurement);
			return hlayout;
		};

		auto statusGroup = new QGroupBox("Status");
		auto vlayout = new QVBoxLayout;
		for (auto channel : allChannels)
			vlayout->addLayout(getRow(channel));

		hlayout = new QHBoxLayout;
		hlayout->addWidget(checkAllButton);
		hlayout->addWidget(uncheckAllButton);
		vlayout->addLayout(hlayout);
		statusGroup->setLayout(vlayout);

		auto contollerGroup = new QGroupBox("Conversion Controller");
		hlayout = new QHBoxLayout;
		hlayout->addWidget(measureButton_);
		hlayout->addWidget(startStopAcqButton_);
		contollerGroup->setLayout(hlayout);


		hlayout = new QHBoxLayout;
		auto fcIdLabel = new QLabel("Fec Id:");
		auto idCombobox = new QComboBox;
		idCombobox->addItem("1", FecIdType::_1);
		idCombobox->addItem("2", FecIdType::_2);
		hlayout->addWidget(fcIdLabel);
		hlayout->addWidget(idCombobox);

		auto vLayout = new QVBoxLayout;
		vLayout->addLayout(hlayout);

		hlayout = new QHBoxLayout;
		auto connectButton = new QPushButton("Connect");
		auto ipLabel = new QLabel("IP Address:");
		auto lineEdit = new QLineEdit;
		QSettings settings;
		lineEdit->setText(devIF_->parentDevice()->inputResources().back()->load());

		hlayout->addWidget(ipLabel);
		hlayout->addWidget(lineEdit);
		hlayout->addWidget(connectButton);
		vLayout->addLayout(hlayout);

		QObject::connect(lineEdit, &QLineEdit::editingFinished,
			[this, lineEdit]() {
				devIF_->parentDevice()->inputResources().back()->setValue(lineEdit->text());
				devIF_->parentDevice()->inputResources().back()->save();
			}
		);

		QObject::connect(connectButton, &QPushButton::clicked,
			[this, connectButton, idCombobox, statusGroup, contollerGroup]() {
				connectButton->setText("Connecting...");
				emit stopMeasurement();
				devIF_->setFcId(static_cast<FecIdType::Type>(idCombobox->currentData().toUInt()));
				devIF_->connect();
			}
		);

		QObject::connect(devIF_->parentDevice(), &AbstractDevice::connectionStatusChanged,
			[this, connectButton, idCombobox, statusGroup, contollerGroup](QString const& name, bool const status) {
				if (status) {
					connectButton->setText("Connected - click to reconnect");
					setterGroup_->setEnabled(true);
					statusGroup->setEnabled(true);
					contollerGroup->setEnabled(true);
				}
				else {
					connectButton->setText("Failed - try again");
					setterGroup_->setEnabled(false);
					statusGroup->setEnabled(false);
					contollerGroup->setEnabled(false);
				}
			}
		);

		auto connectWidgetGroup = new QGroupBox("Connection Controller");
		connectWidgetGroup->setLayout(vLayout);
		

		auto layout = new QVBoxLayout;
		layout->addWidget(connectWidgetGroup);
		layout->addWidget(setterGroup_);
		layout->addWidget(statusGroup);
		layout->addWidget(contollerGroup);

		setterGroup_->setEnabled(false);
		statusGroup->setEnabled(false);
		contollerGroup->setEnabled(false);

		setLayout(layout);
	}

signals:
	void stopMeasurement() const;
};