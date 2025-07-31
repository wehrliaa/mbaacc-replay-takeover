// gamestatemanager.h: Game state-related code
#pragma once

// Big big class containing every relevant memory address.
class Game_State_Manager {
public:
	MemoryBlock aCircuitPosition   = MemoryBlock(0x15DEF0, 2);
	MemoryBlock aCombAfterTimer    = MemoryBlock(0x36E708, 2);
	MemoryBlock aDisableFN1_1_AD   = MemoryBlock(0x41F654, 12);
	MemoryBlock aDisableFN1_2_AD   = MemoryBlock(0x41F652, 2);
	MemoryBlock aDummyState        = MemoryBlock(0x34D7F8, 2);
	MemoryBlock aFN1Key            = MemoryBlock(0x37144C, 1);
	MemoryBlock aFN2Key            = MemoryBlock(0x37144D, 1);

	// See notes.md
	MemoryBlock aGameMode          = MemoryBlock(0x14EEE8, 2);

	MemoryBlock aMaxDamage         = MemoryBlock(0x157E0C, 4);
	MemoryBlock aRecordingMode     = MemoryBlock(0x155137, 2);
	MemoryBlock aRoundReset        = MemoryBlock(0x15DEC3, 1);

	// This is actually related to the super flash freeze, which is probably
	// why "pausing" the game by setting this to 0xFF desyncs the replay.
	MemoryBlock aStopFlag          = MemoryBlock(0x162A48, 1);

	MemoryBlock aTimer             = MemoryBlock(0x162A40, 4);
	MemoryBlock aTrainingMenuPause = MemoryBlock(0x162A64, 2);

	std::vector<MemoryBlock> mem_pairs_list;

	// DONT FORGET TO ADD NEW ADDRESSES HERE!!!!!!!!!!!!!!!!!!!!
	Game_State_Manager() {
		mem_pairs_list.push_back(this->aCircuitPosition);
		mem_pairs_list.push_back(this->aCombAfterTimer);
		mem_pairs_list.push_back(this->aDisableFN1_1_AD);
		mem_pairs_list.push_back(this->aDisableFN1_2_AD);
		mem_pairs_list.push_back(this->aDummyState);
		mem_pairs_list.push_back(this->aFN1Key);
		mem_pairs_list.push_back(this->aFN2Key);
		mem_pairs_list.push_back(this->aGameMode);
		mem_pairs_list.push_back(this->aMaxDamage);
		mem_pairs_list.push_back(this->aRecordingMode);
		mem_pairs_list.push_back(this->aRoundReset);
		mem_pairs_list.push_back(this->aStopFlag);
		mem_pairs_list.push_back(this->aTimer);
		mem_pairs_list.push_back(this->aTrainingMenuPause);

		this->aDisableFN1_1_AD.write_memory((char*)"\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90", 0x41F654, true);
		this->aDisableFN1_2_AD.write_memory((char*)"\x90\x90", 0x41F654, true);
	}

	void
	comb_after_timer_reset() {
		this->aCombAfterTimer.write_memory((char*)"\xff", 0, false);
	}

	void
	fetch_game_data() {
		this->aFN1Key.read_memory(false);
		this->aFN2Key.read_memory(false);
		this->aDummyState.read_memory(false);
		this->aGameMode.read_memory(false);
	}

	int
	game_mode_check() {
		return this->aGameMode.read_memory(false);
	}

	void
	game_reset() {
		this->aRoundReset.write_memory((char*)"\xff", 0, false);
	}

	void
	pause() {
		this->aStopFlag.write_memory((char*)"\xff", 0, false);
		//this->aStopFlag2EB.write_memory((char*)"\x01", 0, false);
	}

	void
	play() {
		this->aStopFlag.write_memory((char*)"\x00", 0, false);
		//this->aStopFlag2EB.write_memory((char*)"\x00", 0, false);
	}

	int
	timer_check() {
		return this->aTimer.read_memory(false);
	}

};
