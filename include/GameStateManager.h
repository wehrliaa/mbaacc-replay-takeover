// GameStateManager.h: Game state-related code
#pragma once

// Big big class containing every relevant memory address.
class GameStateManager {
public:
	MemoryBlock aDisableFN1_1_AD    = MemoryBlock(0x41F654, 12);
	MemoryBlock aDisableFN1_2_AD    = MemoryBlock(0x41F652, 2);
	MemoryBlock aFN1Key             = MemoryBlock(0x37144C, 1);
	MemoryBlock aFN2Key             = MemoryBlock(0x37144D, 1);
	MemoryBlock aBKey               = MemoryBlock(0x37139A, 1);
	MemoryBlock aCKey               = MemoryBlock(0x37139B, 1);
	MemoryBlock aGameMode           = MemoryBlock(0x14EEE8, 2);
	MemoryBlock aEXFlashTimer       = MemoryBlock(0x162A48, 1);

	// NOPing these three function calls effectively pauses the game, replay
	// iterators, input interpreter, etc. It's not a pretty solution, but it's
	// simple enough for me to prefer it over other methods of pausing.
	//
	// This place in the binary calls the function at 4745e0
	MemoryBlock aFnCall1            = MemoryBlock(0x02373D, 5);
	// This place in the binary calls the function at 46db40
	MemoryBlock aFnCall2            = MemoryBlock(0x023742, 5);
	// This place in the binary calls the function at 453b80
	MemoryBlock aFnCall3            = MemoryBlock(0x0237B1, 5);

	// This place in the binary calls the function at 41f5a0. NOPing it
	// prevents the A button from fastforwarding the replay.
	MemoryBlock aFnCall4            = MemoryBlock(0x072AF3, 5);

	// Manipulating this address the right way makes the game ignore any inputs
	// coming from the replay to a specific player. Because the game is always
	// taking the player's inputs, and then overwriting it with inputs from the
	// replay file (in replay mode only), that automatically makes it possible
	// to control that player.
	MemoryBlock aIgnoreRepInputs    = MemoryBlock(0x06D6B7, 3);

	// By manipulating these specific addresses, it becomes possible to control
	// Player 2 using P1's inputs. I don't know why this works, I just know
	// that it does.
	MemoryBlock aJump1              = MemoryBlock(0x06D81A, 2);
	MemoryBlock aJump2              = MemoryBlock(0x06D849, 2);
	MemoryBlock aCmp1               = MemoryBlock(0x06D7DA, 10);

	MemoryBlock aTimer              = MemoryBlock(0x162A40, 4);

	// This points to an as-of-yet unidentified, dynamically allocated struct
	// or w/e.
	//
	// 2. Add 0x120 to the address you get here to get the address to the
	// PlayerReplayData[2] for the first round.
	// 3. Add 0x90 N times to that address to get to round N+1's stuff.
	MemoryBlock aPointerToSomething = MemoryBlock(0x37BF98, 4);

	MemoryBlock aRoundNumber        = MemoryBlock(0x37BFA4, 4);

	GameStateManager() {
		this->aDisableFN1_1_AD.write_memory((char*)"\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90", 0x41F654, true);
		this->aDisableFN1_2_AD.write_memory((char*)"\x90\x90", 0x41F654, true);
	}

	void
	fetch_game_data() {
		this->aFN1Key.read_memory(false);
		this->aBKey.read_memory(false);
		this->aCKey.read_memory(false);
		this->aGameMode.read_memory(false);
		this->aTimer.read_memory(false);
	}

	void
	pause() {
		// NOP IT ALLLLLL!!!!!!!! WRRRRRRRAAAAAAAAAAAAAAAAAAHHHHHHHHHHH!!!!!!
		this->aFnCall1.write_memory((char*)"\x90\x90\x90\x90\x90", 0, false);
		this->aFnCall2.write_memory((char*)"\x90\x90\x90\x90\x90", 0, false);
		this->aFnCall3.write_memory((char*)"\x90\x90\x90\x90\x90", 0, false);

		this->aEXFlashTimer.read_memory(false);
	}

	void
	play() {
		// Actually nevermind un-NOP it please thank you very much
		this->aFnCall1.write_memory((char*)"\xe8\x9e\x0e\x05\x00", 0, false);
		this->aFnCall2.write_memory((char*)"\xe8\xf9\xa3\x04\x00", 0, false);
		this->aFnCall3.write_memory((char*)"\xe8\xca\x03\x03\x00", 0, false);
	}

	void
	takeoverP1() {
		// cmp esi,01
		this->aIgnoreRepInputs.write_memory((char*)"\x83\xfe\x01", 0, false);

		// Prevent the A button from fastforwarding the replay
		this->aFnCall4.write_memory((char*)"\x90\x90\x90\x90\x90", 0, false);
	}

	void
	takeoverP2() {
		// cmp esi,00
		this->aIgnoreRepInputs.write_memory((char*)"\x83\xfe\x00", 0, false);

		// Prevent the A button from fastforwarding the replay
		this->aFnCall4.write_memory((char*)"\x90\x90\x90\x90\x90", 0, false);

		// Control P2 using P1 inputs
		this->aJump1.write_memory((char*)"\x75\x0d", 0, false);
		this->aJump2.write_memory((char*)"\x75\x3c", 0, false);
		this->aCmp1.write_memory((char*)"\x81\x3d\x74\x2a\x56\x00\x01\x00\x00\x00", 0, false);
	}

	void
	untakeover() {
		// cmp edx,02
		this->aIgnoreRepInputs.write_memory((char*)"\x83\xfa\x02", 0, false);

		// Make the A button fastforward the replay again
		this->aFnCall4.write_memory((char*)"\xe8\xa8\xca\xfa\xff", 0, false);

		// Restore normal controls
		this->aJump1.write_memory((char*)"\x74\x0d", 0, false);
		this->aJump2.write_memory((char*)"\x74\x3c", 0, false);
		this->aCmp1.write_memory((char*)"\x81\x3d\x74\x2a\x56\x10\x10\x00\x00\x00", 0, false);
	}
};
