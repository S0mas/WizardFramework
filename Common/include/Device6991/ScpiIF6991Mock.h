#pragma once
#include "scpiExec.h"
#include <QString>
#include "HardwareMock6991.h"
#include "../ScpiIF.h"

class ScpiIF6991Mock : public ScpiIF {
	HardwareMock6991 devMock_;
	std::unique_ptr<ScpiExec> exec_;
	mutable QString responseContainer_;
public:
	ScpiIF6991Mock() : ScpiIF() {
		auto root = std::make_unique<ScpiCmdNode>("root");
		ScpiCmdNode* navigator = root.get();
		navigator->children.push_back(std::make_unique<ScpiCmdNode>("SYSTem"));
		navigator->children.push_back(std::make_unique<ScpiCmdNode>("CONFigure"));
		navigator->children.push_back(std::make_unique<ScpiCmdNode>("MEASure"));
		navigator->children.push_back(std::make_unique<ScpiCmdNode>("*STB?", ScpiFunction::getScpiFunction<>(
			[this](std::vector<ScpiArg> const& args) {
				responseContainer_ = QString::number(devMock_.stb(), 16);
				return 0;
			}
		)));
		navigator->children.push_back(std::make_unique<ScpiCmdNode>("*HW"));
		navigator->children.push_back(std::make_unique<ScpiCmdNode>("*DBG", ScpiFunction::getScpiFunction<double>(
			[this](std::vector<ScpiArg> const& args) {
				devMock_.setDbgMode(args[0].get<double>());
				return 0;
			}
		)));
		navigator->children.push_back(std::make_unique<ScpiCmdNode>("*DBG?", ScpiFunction::getScpiFunction<>(
			[this](std::vector<ScpiArg> const& args) {
				responseContainer_ = QString::number(devMock_.dbgMode());
				return 0;
			}
		)));
		navigator = root->findCmd(std::vector<std::string>({ "*HW" }));

		navigator->children.push_back(std::make_unique<ScpiCmdNode>("FPGA", ScpiFunction::getScpiFunction<unsigned long long, unsigned long long>(
			[this](std::vector<ScpiArg> const& args) {
				devMock_.writeFpgaReg(args[0].get<unsigned long long>(), args[1].get<unsigned long long>());
				return 0;
			}
		)));
		navigator->children.push_back(std::make_unique<ScpiCmdNode>("FPGA?", ScpiFunction::getScpiFunction<unsigned long long>(
			[this](std::vector<ScpiArg> const& args) {
				responseContainer_ = QString::number(devMock_.readFpgaReg(args[0].get<unsigned long long>()), 16);
				return 0;
			}
		)));
		navigator->children.push_back(std::make_unique<ScpiCmdNode>("REG", ScpiFunction::getScpiFunction<unsigned long long, unsigned long long>(
			[this](std::vector<ScpiArg> const& args) {
				devMock_.writeCpuReg(args[0].get<unsigned long long>(), args[1].get<unsigned long long>());
				return 0;
			}
		)));
		navigator->children.push_back(std::make_unique<ScpiCmdNode>("REG?", ScpiFunction::getScpiFunction<unsigned long long>(
			[this](std::vector<ScpiArg> const& args) {
				responseContainer_ = QString::number(devMock_.readCpuReg(args[0].get<unsigned long long>()), 16);
				return 0;
			}
		)));
		navigator->children.push_back(std::make_unique<ScpiCmdNode>("FEC", ScpiFunction::getScpiFunction<double, unsigned long long, unsigned long long>(
			[this](std::vector<ScpiArg> const& args) {
				devMock_.writeFcReg(args[0].get<double>(), args[1].get<unsigned long long>(), args[2].get<unsigned long long>());
				return 0;
			}
		)));
		navigator->children.push_back(std::make_unique<ScpiCmdNode>("FEC?", ScpiFunction::getScpiFunction<double, unsigned long long>(
			[this](std::vector<ScpiArg> const& args) {
				responseContainer_ = QString::number(devMock_.readFcReg(args[0].get<double>(), args[1].get<unsigned long long>()), 16);
				return 0;
			}
		)));

		navigator = root->findCmd(std::vector<std::string>({ "SYSTem" }));
		navigator->children.push_back(std::make_unique<ScpiCmdNode>("ERRor?", ScpiFunction::getScpiFunction<>(
			[this](std::vector<ScpiArg> const& args) {
				responseContainer_ = devMock_.readError();
				return 0;
			}
		)));
		navigator->children.push_back(std::make_unique<ScpiCmdNode>("STATe?", ScpiFunction::getScpiFunction<>(
			[this](std::vector<ScpiArg> const& args) {
				responseContainer_ = QString("%1,%2").arg(DeviceStateEnum::toString(devMock_.status().state())).arg(QString::number(devMock_.status().toUInt(), 16));
				return 0;
			}
		)));
		navigator->children.push_back(std::make_unique<ScpiCmdNode>("PTPTime?", ScpiFunction::getScpiFunction<>(
			[this](std::vector<ScpiArg> const& args) {
				responseContainer_ = devMock_.ptpTime().toString();
				return 0;
			}
		)));
		navigator->children.push_back(std::make_unique<ScpiCmdNode>("IDENtify", ScpiFunction::getScpiFunction<>(
			[this](std::vector<ScpiArg> const& args) {
				qDebug() << "not mocked";
				return 0;
			}
		)));
		navigator->children.push_back(std::make_unique<ScpiCmdNode>("FAN", ScpiFunction::getScpiFunction<double>(
			[this](std::vector<ScpiArg> const& args) {
				devMock_.setFansMode(static_cast<FansModeEnum::Type>(static_cast<int>(args[0].get<double>())));
				return 0;
			}
		)));
		navigator->children.push_back(std::make_unique<ScpiCmdNode>("FAN?", ScpiFunction::getScpiFunction<>(
			[this](std::vector<ScpiArg> const& args) {
				responseContainer_ = QString::number(devMock_.fansMode());
				return 0;
			}
		)));
		navigator->children.push_back(std::make_unique<ScpiCmdNode>("TEMPerature?", ScpiFunction::getScpiFunction<std::string>(
			[this](std::vector<ScpiArg> const& args) {
				if (args.empty()) {
					for (auto type : TemperaturesEnum::TYPES) {
						responseContainer_.append(QString::number(devMock_.temperature(type).temperature_));
						responseContainer_.append(',');
					}
					responseContainer_.chop(1);
				}
				else
					responseContainer_ = QString::number(devMock_.temperature(TemperaturesEnum::fromString(QString::fromStdString(args[0].get<std::string>()))).temperature_);
				return 0;
			}
		)));
		navigator->children.push_back(std::make_unique<ScpiCmdNode>("VOLTage?", ScpiFunction::getScpiFunction<>(
			[this](std::vector<ScpiArg> const& args) {
				responseContainer_ = QString::number(devMock_.voltage());
				return 0;
			}
		)));
		navigator->children.push_back(std::make_unique<ScpiCmdNode>("ATTRibute?", ScpiFunction::getScpiFunction<>(
			[this](std::vector<ScpiArg> const& args) {
				qDebug() << "not mocked";
				return 0;
			}
		)));
		navigator->children.push_back(std::make_unique<ScpiCmdNode>("LOCK", ScpiFunction::getScpiFunction<double>(
			[this](std::vector<ScpiArg> const& args) {
				auto id = static_cast<int>(args[0].get<double>());
				if (id != 0)
					devMock_.takeControl(id);
				else
					devMock_.releaseControl();
				return 0;
			}
		)));
		navigator->children.push_back(std::make_unique<ScpiCmdNode>("LOCK?", ScpiFunction::getScpiFunction<>(
			[this](std::vector<ScpiArg> const& args) {
				responseContainer_ = QString::number(devMock_.controllerId());
				return 0;
			}
		)));

		navigator = root->findCmd(std::vector<std::string>({ "CONFigure" }));
		navigator->children.push_back(std::make_unique<ScpiCmdNode>("ENABled", ScpiFunction::getScpiFunction<>(
			[this](std::vector<ScpiArg> const& args) {
				qDebug() << "not mocked";
				return 0;
			}
		)));
		navigator->children.push_back(std::make_unique<ScpiCmdNode>("ENABled?", ScpiFunction::getScpiFunction<>(
			[this](std::vector<ScpiArg> const& args) {
				qDebug() << "not mocked";
				return 0;
			}
		)));
		navigator->children.push_back(std::make_unique<ScpiCmdNode>("SCAN"));
		navigator->children.push_back(std::make_unique<ScpiCmdNode>("STOPonerr", ScpiFunction::getScpiFunction<double>(
			[this](std::vector<ScpiArg> const& args) {
				devMock_.setStopOnError(args[0].get<double>());
				return 0;
			}
		)));
		navigator->children.push_back(std::make_unique<ScpiCmdNode>("STOPonerr?", ScpiFunction::getScpiFunction<>(
			[this](std::vector<ScpiArg> const& args) {
				responseContainer_ = QString::number(devMock_.stopOnError());
				return 0;
			}
		)));
		navigator->children.push_back(std::make_unique<ScpiCmdNode>("TRIGger"));
		navigator->children.push_back(std::make_unique<ScpiCmdNode>("DIRectread", ScpiFunction::getScpiFunction<double>(
			[this](std::vector<ScpiArg> const& args) {
				devMock_.setScansNoPerDirectReadPacket(args[0].get<double>());
				return 0;
			}
		)));
		navigator->children.push_back(std::make_unique<ScpiCmdNode>("DIRectread?", ScpiFunction::getScpiFunction<>(
			[this](std::vector<ScpiArg> const& args) {
				responseContainer_ = QString::number(devMock_.scansNoPerDirectReadPacket());
				return 0;
			}
		)));

		navigator = root->findCmd(std::vector<std::string>({ "CONFigure", "SCAN" }));
		navigator->children.push_back(std::make_unique<ScpiCmdNode>("RATE", ScpiFunction::getScpiFunction<double, std::string>(
			[this](std::vector<ScpiArg> const& args) {
				ScanRateModel scanRate;
				scanRate.value_ = args[0].get<double>();
				if (args.size() > 1)
					scanRate.units_ = ScanRateUnitsEnum::fromString(QString::fromStdString(args[1].get<std::string>()));
				else
					scanRate.units_ = ScanRateUnitsEnum::HZ;
				devMock_.setScanRate(scanRate);
				return 0;
			}
		)));
		navigator->children.push_back(std::make_unique<ScpiCmdNode>("RATE?", ScpiFunction::getScpiFunction<>(
			[this](std::vector<ScpiArg> const& args) {
				responseContainer_ = devMock_.scanRate().toString();
				return 0;
			}
		)));
		navigator->children.push_back(std::make_unique<ScpiCmdNode>("TIMestamp", ScpiFunction::getScpiFunction<double>(
			[this](std::vector<ScpiArg> const& args) {
				devMock_.setTimeStamps(args[0].get<double>());
				return 0;
			}
		)));
		navigator->children.push_back(std::make_unique<ScpiCmdNode>("TIMestamp?", ScpiFunction::getScpiFunction<>(
			[this](std::vector<ScpiArg> const& args) {
				responseContainer_ = QString::number(devMock_.timestamps());
				return 0;
			}
		)));
		navigator->children.push_back(std::make_unique<ScpiCmdNode>("POST", ScpiFunction::getScpiFunction<double>(
			[this](std::vector<ScpiArg> const& args) {
				devMock_.setScansTreshold(args[0].get<double>());
				return 0;
			}
		)));
		navigator->children.push_back(std::make_unique<ScpiCmdNode>("POST?", ScpiFunction::getScpiFunction<>(
			[this](std::vector<ScpiArg> const& args) {
				responseContainer_ = QString::number(devMock_.scansTreshold());
				return 0;
			}
		)));

		navigator = root->findCmd(std::vector<std::string>({ "CONFigure", "TRIGger" }));
		navigator->children.push_back(std::make_unique<ScpiCmdNode>("STARt", ScpiFunction::getScpiFunction<std::string>(
			[this](std::vector<ScpiArg> const& args) {
				devMock_.setStartMode(AcquisitionStartModeEnum::fromString(QString::fromStdString(args[0].get<std::string>())));
				return 0;
			}
		)));
		navigator->children.push_back(std::make_unique<ScpiCmdNode>("STARt?", ScpiFunction::getScpiFunction<>(
			[this](std::vector<ScpiArg> const& args) {
				responseContainer_ = AcquisitionStartModeEnum::toString(devMock_.startMode());
				return 0;
			}
		)));
		navigator->children.push_back(std::make_unique<ScpiCmdNode>("PTP"));

		navigator = root->findCmd(std::vector<std::string>({ "CONFigure", "TRIGger", "PTP" }));
		navigator->children.push_back(std::make_unique<ScpiCmdNode>("ALARm", ScpiFunction::getScpiFunction<double, double>(
			[this](std::vector<ScpiArg> const& args) {
				devMock_.setPtpAlarm({ static_cast<int32_t>(args[0].get<double>()), static_cast<int32_t>(args[1].get<double>()) });
				return 0;
			}
		)));
		navigator->children.push_back(std::make_unique<ScpiCmdNode>("ALARm?", ScpiFunction::getScpiFunction<>(
			[this](std::vector<ScpiArg> const& args) {
				responseContainer_ = devMock_.ptpAlarm().toString();
				return 0;
			}
		)));

		navigator = root->findCmd(std::vector<std::string>({ "MEASure" }));
		navigator->children.push_back(std::make_unique<ScpiCmdNode>("ASYNc", ScpiFunction::getScpiFunction<>(
			[this](std::vector<ScpiArg> const& args) {
				devMock_.startAcquisition();
				return 0;
			}
		)));
		navigator->children.push_back(std::make_unique<ScpiCmdNode>("FETCh?", ScpiFunction::getScpiFunction<>(
			[this](std::vector<ScpiArg> const& args) {
				qDebug() << "not mocked";
				return 0;
			}
		)));
		navigator->children.push_back(std::make_unique<ScpiCmdNode>("ABORt", ScpiFunction::getScpiFunction<>(
			[this](std::vector<ScpiArg> const& args) {
				devMock_.stopAcquisition();
				return 0;
			}
		)));
		navigator->children.push_back(std::make_unique<ScpiCmdNode>("SCAN?", ScpiFunction::getScpiFunction<>(
			[this](std::vector<ScpiArg> const& args) {
				qDebug() << "not mocked";
				return 0;
			}
		)));

		exec_ = std::make_unique<ScpiExec>(std::move(root));
		ScpiArg arg;
		arg.set<std::string>("Hz");
		ScpiArgsParser::addMnemonic("Hz", arg);
		arg.set<std::string>("us");
		ScpiArgsParser::addMnemonic("us", arg);
		arg.set<double>(1);
		ScpiArgsParser::addMnemonic("ON", arg);
		arg.set<double>(0);
		ScpiArgsParser::addMnemonic("OFF", arg);
	}
	int read(uint32_t const vi, unsigned char* dataPtr, uint32_t size, unsigned long* ret) const noexcept override {
		*ret = 1;
		for (auto c : responseContainer_) {
			*dataPtr = c.toLatin1();
			++dataPtr;
			++(*ret);
		}
		return 0;
	}
	int write(uint32_t const vi, unsigned char* dataPtr, uint32_t size, unsigned long* ret) const noexcept override {
		auto code = exec_->parseAndExecuteCommand(reinterpret_cast<char*>(dataPtr));
		return code;
	}
};