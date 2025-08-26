// SaveStateManager.h: Savestate-related code
#pragma once

class SaveStateManager {
	// キャラクター構造体のサイズを定義
	DWORD PlayerStructSize = 0xAFC; // 3084 (?)
	// 1Pデータの開始位置を定義
	DWORD DAT_P1_AD = 0x155140;     // 971 bytes

	MemoryBlock aCam1XY            = MemoryBlock(0x164B14, 8);
	MemoryBlock aCam2XY            = MemoryBlock(0x15DEC4, 8);
	MemoryBlock aCamZoom           = MemoryBlock(0x14EB70, 12);
	MemoryBlock aObjects           = MemoryBlock(0x27BD70, 74576);
	MemoryBlock aStoppageStatus    = MemoryBlock(0x158600, 1632);
	MemoryBlock aDamage            = MemoryBlock(0x157DD8, 52);
	MemoryBlock aDamage2           = MemoryBlock(0x157E10, 1004);
	MemoryBlock aShiftControlFlag1 = MemoryBlock(0x157DB8, 4);
	MemoryBlock aShiftControlFlag2 = MemoryBlock(0x157DBC, 4);
	MemoryBlock aEXFlashTimer      = MemoryBlock(0x162A48, 4);
	MemoryBlock aCharacterObj1     = MemoryBlock(this->DAT_P1_AD, this->PlayerStructSize);
	MemoryBlock aCharacterObj2     = MemoryBlock(this->DAT_P1_AD + (this->PlayerStructSize * 1), this->PlayerStructSize);
	MemoryBlock aCharacterObj3     = MemoryBlock(this->DAT_P1_AD + (this->PlayerStructSize * 2), this->PlayerStructSize);
	MemoryBlock aCharacterObj4     = MemoryBlock(this->DAT_P1_AD + (this->PlayerStructSize * 3), this->PlayerStructSize);
	MemoryBlock aRoundTimer        = MemoryBlock(0x162A3C, 4);
	MemoryBlock aSomeKONumber      = MemoryBlock(0x162A6F, 1);
	MemoryBlock aRngState1         = MemoryBlock(0x163778, 8);
	MemoryBlock aRngState2         = MemoryBlock(0x164068, 225);
	MemoryBlock aSlowdownTimer     = MemoryBlock(0x15D208, 2);

	struct PlayerReplayData prd[6][2]; // Maximum of 6 rounds, 2 players

	std::vector<MemoryBlock> mem_pairs_list;

public:
	// Feio pra caralho, eu sei
	SaveStateManager() {
		mem_pairs_list.push_back(this->aCam1XY);
		mem_pairs_list.push_back(this->aCam2XY);
		mem_pairs_list.push_back(this->aCamZoom);
		mem_pairs_list.push_back(this->aObjects);
		mem_pairs_list.push_back(this->aStoppageStatus);
		mem_pairs_list.push_back(this->aDamage);
		mem_pairs_list.push_back(this->aDamage2);
		mem_pairs_list.push_back(this->aShiftControlFlag1);
		mem_pairs_list.push_back(this->aShiftControlFlag2);
		mem_pairs_list.push_back(this->aEXFlashTimer);
		mem_pairs_list.push_back(this->aCharacterObj1);
		mem_pairs_list.push_back(this->aCharacterObj2);
		mem_pairs_list.push_back(this->aCharacterObj3);
		mem_pairs_list.push_back(this->aCharacterObj4);
		mem_pairs_list.push_back(this->aRoundTimer);
		mem_pairs_list.push_back(this->aSomeKONumber);
		mem_pairs_list.push_back(this->aRngState1);
		mem_pairs_list.push_back(this->aRngState2);
		mem_pairs_list.push_back(this->aSlowdownTimer);
	}

	void
	save(GameStateManager* gsm) {
		// Save gamestate
		for (MemoryBlock i : this->mem_pairs_list)
			i.read_memory(false);

		// Save replay readers(?)
		saveReplayData(gsm, this->prd);
	}
	
	void
	load(GameStateManager* gsm) {
		// Load gamestate
		for (MemoryBlock i : this->mem_pairs_list)
			i.write_memory(NULL, 0, false);

		// Load replay readers(?)
		loadReplayData(gsm, this->prd);
	}
};
