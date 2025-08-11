// PlayerReplayData.h: Everything related to the PlayerReplayData struct.
#pragma once

// This is a recreation of a single struct (or of what looks like a single
// struct) that holds the information regarding what part of the replay file is
// being read at the moment, for one player. Since there's 2 players, there's
// going to be two of these one next to each other in the game's memory, like
// an array.
// 
// (hugenumber2 - hugenumber1) << 3 = total number of inputs this player made
// in the current round.
struct PlayerReplayData {              // 32 (0x20) bytes long
	int unknown1;                      // offset 0x00
	int hugenumber1;                   // offset 0x04
	int hugenumber2;                   // offset 0x08
	int unknown2;                      // offset 0x0C
	int roundN_WP;                     // offset 0x10
	int unknown3;                      // offset 0x14
	unsigned int NthInput;             // offset 0x18
	unsigned int framesSinceLastInput; // offset 0x1C
};

void
saveReplayData(GameStateManager* gsm, struct PlayerReplayData (&prdArray)[6][2]) {
	int pointerToRoundStuff;
	int pointerToSomeOtherThing;
	int roundNumber;

	// Could maybe get this value only once, since the game only sets it once.
	pointerToSomeOtherThing = gsm->aPointerToSomething.read_memory(false) + 0x120;

	ReadProcessMemory(
		gProc.handle,
		(LPVOID)(pointerToSomeOtherThing),
		&pointerToRoundStuff, 4, NULL);

	roundNumber = gsm->aRoundNumber.read_memory(false);

	ReadProcessMemory(
		gProc.handle,
		(LPVOID)(pointerToRoundStuff + (0x90 * roundNumber)),
		&prdArray[roundNumber], sizeof(struct PlayerReplayData[2]), NULL);
}

void
loadReplayData(GameStateManager* gsm, struct PlayerReplayData (&prdArray)[6][2]) {
	int pointerToRoundStuff;
	int pointerToSomeOtherThing;
	int roundNumber;

	// Could maybe get this value only once, since the game only sets it once.
	pointerToSomeOtherThing = gsm->aPointerToSomething.read_memory(false) + 0x120;

	ReadProcessMemory(
		gProc.handle,
		(LPVOID)(pointerToSomeOtherThing),
		&pointerToRoundStuff, 4, NULL);

	roundNumber = gsm->aRoundNumber.read_memory(false);

	WriteProcessMemory(
		gProc.handle,
		(LPVOID)(pointerToRoundStuff + (0x90 * roundNumber)),
		&prdArray[roundNumber], sizeof(struct PlayerReplayData[2]), NULL);
}
