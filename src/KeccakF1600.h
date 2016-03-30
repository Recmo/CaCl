#pragma once
#include <array>
#include <cstdint>

class KeccakF1600
{
public:
	static constexpr uint stateSizeBits = 1600;
	static constexpr uint stateSizeBytes = stateSizeBits / 8;
	typedef std::array<std::uint8_t, stateSizeBytes> State;
	
	static void full(State& state) throw() { keccup(state, true); }
	static void half(State& state) throw() { keccup(state, false); }
	
private:
	KeccakF1600() = delete;
	
	static constexpr uint rounds = 24;
	static void keccup(State& state, bool full) throw();
	static const std::uint64_t _roundConstants[rounds];
};
