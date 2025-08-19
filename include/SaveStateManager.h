// SaveStateManager.h: Savestate-related code

class SaveStateManager {
	// キャラクター構造体のサイズを定義
	DWORD PlayerStructSize = 0xAFC; // 3084 (?)
	// 1Pデータの開始位置を定義
	DWORD DAT_P1_AD = 0x155140;     // 971 bytes

	//MemoryBlock aCam1X             = MemoryBlock(0x164B14, 4);
	//MemoryBlock aCam1Y             = MemoryBlock(0x164B18, 4);
	//MemoryBlock aCam2X             = MemoryBlock(0x15DEC4, 4);
	//MemoryBlock aCam2Y             = MemoryBlock(0x15DEC8, 4);
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

	std::vector<MemoryBlock> mem_pairs_list;

public:
	int EXFlashTimer = 0;

	// Feio pra caralho, eu sei
	SaveStateManager() {
		//mem_pairs_list.push_back(this->aCam1X);
		//mem_pairs_list.push_back(this->aCam1Y);
		//mem_pairs_list.push_back(this->aCam2X);
		//mem_pairs_list.push_back(this->aCam2Y);
		mem_pairs_list.push_back(this->aObjects);
		mem_pairs_list.push_back(this->aStoppageStatus);
		mem_pairs_list.push_back(this->aDamage);
		mem_pairs_list.push_back(this->aDamage2);
		mem_pairs_list.push_back(this->aShiftControlFlag1);
		mem_pairs_list.push_back(this->aShiftControlFlag2);
		mem_pairs_list.push_back(this->aCharacterObj1);
		mem_pairs_list.push_back(this->aCharacterObj2);
		//mem_pairs_list.push_back(this->aCharacterObj3);
		//mem_pairs_list.push_back(this->aCharacterObj4);
	}

	void
	save(GameStateManager* gsm) {
		for (MemoryBlock i : this->mem_pairs_list)
			i.read_memory(false);

		this->EXFlashTimer = gsm->aEXFlashTimer.int_data;
	}
	
	void
	load() {
		for (MemoryBlock i : this->mem_pairs_list)
			i.write_memory(NULL, 0, false);
	}

};
