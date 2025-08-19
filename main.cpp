// main.cpp: Main file

#include "main.h"

// This was declared in ProcessHandling.h first.
struct Process gProc;

void
signal_handler(int signum) {
	HANDLE han = GetStdHandle(STD_OUTPUT_HANDLE);
	CONSOLE_CURSOR_INFO cursorInfo;
	GetConsoleCursorInfo(han, &cursorInfo);
	cursorInfo.bVisible = true;
	SetConsoleCursorInfo(han, &cursorInfo);

	printf("\n\n");
	exit(signum);
}

int
main() {
	signal(SIGINT, signal_handler);
	signal(SIGTERM, signal_handler);

	HANDLE han = GetStdHandle(STD_OUTPUT_HANDLE);
	CONSOLE_CURSOR_INFO cursorInfo;
	GetConsoleCursorInfo(han, &cursorInfo);
	cursorInfo.bVisible = false;
	SetConsoleCursorInfo(han, &cursorInfo);

	system("cls");
 	gProc = wait_process("MBAA.exe");

	// Input-related variables
	int BButton;
	int BFrames;
	int CButton;
	int CFrames;
	int DButton;
	int DFrames;
	int FN1Button;
	int FN1Frames;

	// Game state-related variables
	GameStateManager game_state;
	bool isPaused = false;
	int global_frame_count = 0;
	int prev_frame_count = 0;

	// Save state-related variables
	SaveStateManager save_state;
	struct PlayerReplayData prdArray[6][2]; // Maximum of 6 rounds, 2 players
	bool loadedState = false;

	// Takeover-related variables
	bool isTakingOver = false;

	// Rewind-related variables
	// ~53MB of memory in total
	struct RewindState* rewindPool = new struct RewindState [600];
	int rewindIndex = 0;
	int rewindCount = 0;

	while (1) {
		game_state.fetch_game_data();
		global_frame_count = game_state.aTimer.int_data;

		// Reset stuff at the start of each round
		if (global_frame_count == 0) {
			isPaused = false;
			game_state.play();

			isTakingOver = false;
			game_state.untakeover();

			rewindIndex = 0;
			rewindCount = 0;
		}

		// Everything below this chunk of code is synced with the game's
		// framerate.
		// TODO: Change this to replay timer, so you can pause and take over
		// while in intro state 1
		if (global_frame_count == prev_frame_count) {
			Sleep(2); // Reduce CPU usage for free with this one simple trick!
			continue;
		}
		prev_frame_count = global_frame_count;

		// Handle inputs FIRST //

		// FN1
		FN1Button = game_state.aFN1Key.int_data;
		if (FN1Button >= 1) {
			FN1Frames += 1;
		} else FN1Frames = 0;

		// B
		BButton = game_state.aBKey.int_data;
		if (BButton >= 1) {
			BFrames += 1;
		} else BFrames = 0;

		// C
		CButton = game_state.aCKey.int_data;
		if (CButton >= 1) {
			CFrames += 1;
		} else CFrames = 0;

		// D
		DButton = game_state.aDKey.int_data;
		if (DButton >= 1) {
			DFrames += 1;
		} else DFrames = 0;

		// Handle state SECOND //
		// srry im dumb i need to remind myself of this //

		if (FN1Frames == 1) {
			if (!isTakingOver) {
				isPaused = !isPaused;

				if (isPaused) {
					game_state.aEXFlashTimer.read_memory(false);
					game_state.pause();
				} else {
					// Very ugly way to prevent desyncs when unpausing during
					// EX flash
					if (loadedState) {
						char buf[4];
						memcpy(&buf, &save_state.EXFlashTimer, 4);
						game_state.aEXFlashTimer.write_memory(buf, 0, false);
						loadedState = false;
					} else {
						game_state.aEXFlashTimer.write_memory(NULL, 0, false);
					}

					game_state.play();
				}
			} else {
				// stop taking over, pause, and load state
				isTakingOver = false;
				game_state.untakeover();

				isPaused = true;
				game_state.pause();

				loadedState = true;
				save_state.load();
				loadReplayData(&game_state, prdArray);
			}
		}

		if ((isPaused) && ((BFrames == 1) || (CFrames == 1))) {

			// Save current state before taking over.
			save_state.save(&game_state);
			saveReplayData(&game_state, prdArray);

			isTakingOver = true;

			// Countdown before taking over
			for (int i = 0; i < 3; i++) {
				game_state.aSound1.write_memory((char*)"\x01", 0, false);
				Sleep(500);
			}

			// If B and C are pressed on the same frame, P2 will be selected.
			switch (CFrames == 1) {
				case 0:
					isPaused = false;
					game_state.takeoverP1();
					game_state.play();
					break;
				case 1:
					isPaused = false;
					game_state.takeoverP2();
					game_state.play();
					break;
			}
		}

		// Replay rewind
		if ((!isTakingOver) && (!isPaused)) {
			if (DButton >= 1) {
				if ((rewindCount < 600) && (rewindIndex > 0)) {
					rewindCount += 1;
					rewindIndex = (rewindIndex - 1) % 600;
				}

				loadRewind(&game_state, &rewindPool[rewindIndex]);
			} else {
				rewindCount = 0;
				// Save every other frame to use twice less memory. And also
				// to rewind twice as fast.
				if (global_frame_count % 2 == 0) {
					saveRewind(&game_state, &rewindPool[rewindIndex]);
					rewindIndex = (rewindIndex + 1) % 600;
				}
			}
		}
	}

	return 0;
}
