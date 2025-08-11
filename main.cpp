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
	int FN1Button;
	int FN1Frames;
	int FN2Button;
	int FN2Frames;

	// Game state-related variables
	GameStateManager game_state;
	bool isPaused = false;
	int global_frame_count = 0;
	int prev_frame_count = 0;

	// Save state-related variables
	SaveStateManager save_state;
	bool isStateSaved = false;
	bool loadedState = false;

	// Maximum of 6 rounds, 2 players
	struct PlayerReplayData prdArray[6][2];

	while (1) {
		game_state.fetch_game_data();

		// Everything below this chunk of code is synced with the game's
		// framerate.
		// TODO: Change this to replay timer, so you can pause and take over
		// while in intro state 1
		global_frame_count = game_state.timer_check();
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

		// FN2
		FN2Button = game_state.aFN2Key.int_data;
		if (FN2Button >= 1) {
			FN2Frames += 1;
		} else FN2Frames = 0;

		// Handle state SECOND //
		// srry im dumb i need to remind myself of this //

		// Reset stuff at the start of each round
		if (global_frame_count == 0) {
			isPaused = false;
			isStateSaved = false;
		}

		if (FN1Frames == 1) {
			isPaused = !isPaused;

			if (isPaused) {
				game_state.pause();
			} else {
				// Very ugly way to prevent desyncs when pausing during EX flash
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
		}

		if (FN2Frames == 1) {
			if (isPaused) {
				// save state
				save_state.save(&game_state);
				saveReplayData(&game_state, prdArray);
				isStateSaved = true;
			} else {
				// load state and pause.
				if (isStateSaved) {
					isPaused = true;
					loadedState = true;
					game_state.pause();
					save_state.load();
					loadReplayData(&game_state, prdArray);
				}
			}
		}
	}

	return 0;
}
