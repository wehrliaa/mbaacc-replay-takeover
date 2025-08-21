// RewindSystem.h: Replay rewind stuff.

// This is a struct containing the absolute minimum that needs to be saved
// for replay rewind to work. I'm deliberately not using MemoryBlocks here to
// keep it as small as possible.
struct RewindState {
	char aObjects[74576];
	char aStoppageStatus[1632];
	char aDamage[52];
	char aDamage2[1004];
	char aShiftControlFlag12[8]; // 1 and 2 merged to use less R/WProcMem calls
	char aEXFlashTimer[4];
	char aCharacterObjs[11248]; // All 4 CharObjs merged for the same purpose
	char aRoundTimer[4];
	char aSomeKONumber[1];
	char aRngState1[8]; // Two numbers merged for the same purpose
	char aRngState2[225];

	struct PlayerReplayData prd[6][2];
};

void
saveRewind(GameStateManager* gsm, struct RewindState* rs) {
	ReadProcessMemory(gProc.handle, (LPVOID)(gProc.baddr + 0x27BD70), rs->aObjects, 74576, NULL);
	ReadProcessMemory(gProc.handle, (LPVOID)(gProc.baddr + 0x158600), rs->aStoppageStatus, 1632, NULL);
	ReadProcessMemory(gProc.handle, (LPVOID)(gProc.baddr + 0x157DD8), rs->aDamage, 52, NULL);
	ReadProcessMemory(gProc.handle, (LPVOID)(gProc.baddr + 0x157E10), rs->aDamage2, 1004, NULL);
	ReadProcessMemory(gProc.handle, (LPVOID)(gProc.baddr + 0x157DB8), rs->aShiftControlFlag12, 8, NULL);
	ReadProcessMemory(gProc.handle, (LPVOID)(gProc.baddr + 0x162A48), rs->aEXFlashTimer, 4, NULL);
	ReadProcessMemory(gProc.handle, (LPVOID)(gProc.baddr + 0x155140), rs->aCharacterObjs, 11248, NULL);
	ReadProcessMemory(gProc.handle, (LPVOID)(gProc.baddr + 0x162A3C), rs->aRoundTimer, 4, NULL);
	ReadProcessMemory(gProc.handle, (LPVOID)(gProc.baddr + 0x162A6F), rs->aSomeKONumber, 1, NULL);
	ReadProcessMemory(gProc.handle, (LPVOID)(gProc.baddr + 0x163778), rs->aRngState1, 8, NULL);
	ReadProcessMemory(gProc.handle, (LPVOID)(gProc.baddr + 0x164068), rs->aRngState2, 225, NULL);
	
	saveReplayData(gsm, rs->prd);
}

void
loadRewind(GameStateManager* gsm, struct RewindState* rs) {
	WriteProcessMemory(gProc.handle, (LPVOID)(gProc.baddr + 0x27BD70), rs->aObjects, 74576, NULL);
	WriteProcessMemory(gProc.handle, (LPVOID)(gProc.baddr + 0x158600), rs->aStoppageStatus, 1632, NULL);
	WriteProcessMemory(gProc.handle, (LPVOID)(gProc.baddr + 0x157DD8), rs->aDamage, 52, NULL);
	WriteProcessMemory(gProc.handle, (LPVOID)(gProc.baddr + 0x157E10), rs->aDamage2, 1004, NULL);
	WriteProcessMemory(gProc.handle, (LPVOID)(gProc.baddr + 0x157DB8), rs->aShiftControlFlag12, 8, NULL);
	WriteProcessMemory(gProc.handle, (LPVOID)(gProc.baddr + 0x162A48), rs->aEXFlashTimer, 4, NULL);
	WriteProcessMemory(gProc.handle, (LPVOID)(gProc.baddr + 0x155140), rs->aCharacterObjs, 11248, NULL);
	WriteProcessMemory(gProc.handle, (LPVOID)(gProc.baddr + 0x162A3C), rs->aRoundTimer, 4, NULL);
	WriteProcessMemory(gProc.handle, (LPVOID)(gProc.baddr + 0x162A6F), rs->aSomeKONumber, 1, NULL);
	WriteProcessMemory(gProc.handle, (LPVOID)(gProc.baddr + 0x163778), rs->aRngState1, 8, NULL);
	WriteProcessMemory(gProc.handle, (LPVOID)(gProc.baddr + 0x164068), rs->aRngState2, 225, NULL);

	loadReplayData(gsm, rs->prd);
}
