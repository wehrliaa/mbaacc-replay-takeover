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

	// Game state-related variables
	Save_State_Manager save_state;
	Game_State_Manager game_state;
	bool isPaused = false;
	bool isReplayDataSaved = false;
	int global_frame_count = 0;
	int prev_frame_count = 0;

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
			Sleep(1); // Reduce CPU usage for free with this one simple trick!
			continue;
		}
		prev_frame_count = global_frame_count;

		// Handle inputs FIRST //

		// FN1
		FN1Button = game_state.aFN1Key.int_data;
		if (FN1Button >= 1) {
			FN1Frames += 1;
			if (FN1Frames == 1) { // Not being held
				isPaused = !isPaused;
			}
		} else FN1Frames = 0;

		// Handle state SECOND //
		// srry im dumb i need to remind myself of this //

		// Reset stuff at the start of each round
		if (global_frame_count == 0) {
			isPaused = false;
			isReplayDataSaved = false;
		}

		if (isPaused) {
			// save replay data and pause... or maybe just pause
			if (!isReplayDataSaved) {
				isReplayDataSaved = true;
				//saveReplayData(&game_state, prdArray);
			}
			game_state.pause();

		} else {
			// load replay data and play
			if (FN1Frames == 1) {
				//loadReplayData(&game_state, prdArray);
				isReplayDataSaved = false;
				game_state.play();
			}
		}
	}

	return 0;
}
