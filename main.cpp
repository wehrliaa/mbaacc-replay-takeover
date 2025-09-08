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

	// Input-related variables
	int BButton = 0;
	int BFrames = 0;

	int CButton = 0;
	int CFrames = 0;

	int DButton = 0;
	int DFrames = 0;
	int DLastFrame = 0;

	int FN1Button = 0;
	int FN1Frames = 0;

	int FN2Button = 0;
	int FN2Frames = 0;

	// Game state-related variables
	GameStateManager game_state;
	bool isPaused = false;
	int global_frame_count = 0;
	int prev_frame_count = 0;

	// Save state-related variables
	SaveStateManager save_state;

	// Takeover-related variables
	bool isTakingOver = false;

	// Rewind-related variables
	int rewindPoolSize = 30 * (60 / 2); // 30 seconds, saving every other frame
	struct RewindState* rewindPool = new struct RewindState [rewindPoolSize];
	int rewindReadIndex = 0;
	int rewindWriteIndex = 0;
	int rewindReadCount = 0;
	int rewindWriteCount = 0;

	// Process stuff
	DWORD exitCode = 0;

	// UI variables
	std::string P1Text = "\0";
	std::string P2Text = "\0";

start: // FUCKING LOVE GOTOS HELL YEAHHHHH

	system("title MBAACC Replay Takeover v1.0");

	system("cls");
	printf(
		"\n"
		"Please open the game in Replay Mode. In CCCaster v3.1, go to:\n\n"
		"Main menu -> [4] Offline -> [5] Replay\n\n"
	);

 	gProc = wait_process("MBAA.exe");

	// Check for replay mode. Can't put this in the main loop below because
	// any printf calls lag the whole thing. Maybe I could multithread this
	// actually... Ehhhh who cares anyway, this works. Ugly but works.
	//
	// Actually putting it in the main loop is unnecessary, since once replay
	// gets detected, it's not gonna go back to non-replay mode. Unless if
	// you're running the game from the base exe... You're not doing that, are
	// you?
	while (game_state.aVersusCheck.read_memory(false) != 2) {
		printf("\rReplay mode wasn't detected!");

		GetExitCodeProcess(gProc.handle, &exitCode);
		if (gProc.handle == 0x0 || exitCode != 259) goto start;

		Sleep(500);
	}

	prepareChallengerText();
	changeChallengerText(P1Text.c_str(), P2Text.c_str());

	set_cursor_pos(0, 1);
	printf(
		"FN1    = Pauses and unpauses the replay.                          \n"
		"                                                                  \n"
		"FN2    = While taking over, resets back to where you last\n"
		"         paused, and pauses the replay.\n"
		"                                                                  \n"
		"B or C = With the replay paused, takes over P1's or P2's actions\n"
		"         respectively, after a little countdown.\n"
		"                                                                  \n"
		"D      = Rewinds the replay for a maximum of 30 seconds (to\n"
		"         save memory). Reset the round if you need to go back\n"
		"         further.\n"
		"                                                                  \n"
		"Check for new releases at:\n"
		"         https://github.com/wehrliaa/mbaacc-replay-takeover/\n"
		//"rewindPool[%d] is using ~%dMB of memory.\n\n",
		//rewindPoolSize,
		//sizeof(struct RewindState[rewindPoolSize]) / (1024 * 1024)
	);

	while (1) {
		// Restart if game has been closed.
		GetExitCodeProcess(gProc.handle, &exitCode);
		if (gProc.handle == 0x0 || exitCode != 259) goto start;

		game_state.fetch_game_data();
		global_frame_count = game_state.aTimer.int_data;

		// Reset stuff at the start of each round
		if (global_frame_count == 0) {
			isPaused = false;
			//game_state.play();

			isTakingOver = false;
			game_state.untakeover();

			rewindReadCount = 0;
			rewindWriteCount = 0;
			rewindWriteIndex = rewindReadIndex;

			P1Text = "\0";
			P2Text = "\0";
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

		// Only read inputs after intro state 1, and before the winning
		// character's winpose.
		if (
			((game_state.aOutroState.int_data == 0) || (game_state.aOutroState.int_data == 199)) &&
			(game_state.aIntroState.int_data == 0)
		) {
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
		}

		// Handle state SECOND //
		// srry im dumb i need to remind myself of this //

		if ((FN1Frames == 1) && (!isTakingOver)) {
			isPaused = !isPaused;

			if (isPaused) {
				P1Text = "PAUSED\0";
				P2Text = "\0";

				save_state.save(&game_state);
				//game_state.pause();
			} else {
				P1Text = "PLAYING\0";
				P2Text = "\0";

				save_state.load(&game_state);
				//game_state.play();
			}
		}

		if ((FN2Frames == 1) && (isTakingOver)) {
			// stop taking over, pause, and load state
			P1Text = "PAUSED\0";
			P2Text = "\0";

			isTakingOver = false;
			game_state.untakeover();

			save_state.load(&game_state);

			isPaused = true;
			//game_state.pause();
		}

		if (isPaused) {
			game_state.aEXFlashTimer.write_memory((char*)"\xff", 0, false);
		}

		// Replay takeover
		if ((isPaused) && ((BFrames == 1) || (CFrames == 1))) {

			isTakingOver = true;

			P1Text = "TAKING OVER\0";
			std::stringstream ss;

			// Countdown before taking over
			for (int i = 3; i >= 1; i--) {
				game_state.aSound1.write_memory((char*)"\x01", 0, false);

				ss.str("");
				ss << "PLAYER " << (CFrames == 1) + 1 << " IN " << i << "\0";
				P2Text = ss.str();
				changeChallengerText(P1Text.c_str(), P2Text.c_str());

				Sleep(500);
			}

			ss.str("");
			ss << "PLAYER " << (CFrames == 1) + 1 << "\0";
			P2Text = ss.str();

			// Load current state before taking over, just to make sure.
			save_state.load(&game_state);

			// If B and C are pressed on the same frame, P2 will be selected.
			switch (CFrames == 1) {
				case 0:
					isPaused = false;
					game_state.takeoverP1();
					//game_state.play();
					break;
				case 1:
					isPaused = false;
					game_state.takeoverP2();
					//game_state.play();
					break;
			}
		}

		// Replay rewind
		// This is basically a circular buffer that is read backwards from the
		// latest write location.
		if ((!isTakingOver) && (!isPaused)) {
			if (DButton >= 1) {
				// Reading from the buffer
				if (rewindReadCount < rewindWriteCount) {
					rewindReadIndex = (rewindReadIndex - 1 + rewindPoolSize) % rewindPoolSize;
					rewindReadCount += 1;
				}

				P1Text = "REWINDING\0";
				P2Text = "\0";
				//printf("reading - rindex=%d, windex=%d, rcount=%d, wcount=%d\n", rewindReadIndex, rewindWriteIndex, rewindReadCount, rewindWriteCount);
				loadRewind(&game_state, &rewindPool[rewindReadIndex]);

			} else {
				// Writing to the buffer
				if (DLastFrame >= 1)
					rewindWriteCount -= rewindReadCount;

				P1Text = "PLAYING\0";
				P2Text = "\0";

				if (global_frame_count % 2 == 0) {
					if (rewindReadIndex == rewindWriteIndex) {
						//printf("writing - rindex=%d, windex=%d, rcount=%d, wcount=%d\n", rewindReadIndex, rewindWriteIndex, rewindReadCount, rewindWriteCount);
						saveRewind(&game_state, &rewindPool[rewindWriteIndex]);
						rewindWriteIndex = (rewindWriteIndex + 1) % rewindPoolSize;
					}

					rewindReadIndex = (rewindReadIndex + 1) % rewindPoolSize;
					if (rewindWriteCount < rewindPoolSize) rewindWriteCount += 1;
				}

				rewindReadCount = 0;
			}

			DLastFrame = DButton;
		}

		changeChallengerText(P1Text.c_str(), P2Text.c_str());
	}

	signal_handler(0);
}
