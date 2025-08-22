// Ui.h: User interface utilities
//
// The "Challenger Text" functions change the text right above the health
// bars. I called them that because they are always just "Challenger" in
// replay mode.

// Setup stuff
void
prepareChallengerText() {
	// This forces the P1 challenger text to use one buffer, and the P2 to use
	// another. By default they both use the same buffer, with the same string.
	MemoryBlock aTest1 = MemoryBlock(0x026BE3, 2);
	MemoryBlock aCmp1 = MemoryBlock(0x026BFB, 3);

	aTest1.write_memory((char*)"\x85\xc9", 0, false);
	aCmp1.write_memory((char*)"\x83\xf9\x01", 0, false);

	// Change permissions on the relevant text buffers to read/write.
	DWORD oldProtect;
	VirtualProtectEx(gProc.handle, (LPVOID)(gProc.baddr + 0x135BB4), 32, PAGE_READWRITE, &oldProtect);
}

// Change stuff
void
changeChallengerText(const char* P1Text, const char* P2Text) {
	MemoryBlock aP1TextBuffer = MemoryBlock(0x135BC4, 16);
	MemoryBlock aP2TextBuffer = MemoryBlock(0x135BB4, 16);

	aP1TextBuffer.write_memory((char*)P1Text, 0, false);
	aP2TextBuffer.write_memory((char*)P2Text, 0, false);
}
