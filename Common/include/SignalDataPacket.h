#pragma once
#include <vector>

namespace SignalDataPacket {
	using Sample6111 = bool;
	using Sample6132 = float;
	enum class SampleType {
		Sample6111 = 6111,
		Sample6132 = 6132
	};
	struct Header {
		SampleType sampleType_;
		uint32_t scansNo_;
		uint32_t dataSize_;
	};
	template<typename T>
	struct Data {
		Data(uint32_t const samples = 0) {
			samples_.resize(samples);
		}
		void resize(uint32_t const samples) {
			samples_.resize(samples);
		}
		struct TimeStamp {
			uint32_t seconds_;
			uint32_t nanoseconds_;
		};
		std::vector<T> samples_;
	};
}
