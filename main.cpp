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
	int global_frame_count = 0;
	int prev_frame_count = 0;

	// Save state-related variables
	SaveStateManager save_state;

	// Takeover-related variables
	int takeoverCountdown;
	int takeoverCountdownFrames = 0;
	int playerToTakeover;

	// Rewind-related variables
	int rewindPoolSize;
	struct RewindState* rewindPool;
	int rewindReadIndex = 0;
	int rewindWriteIndex = 0;
	int rewindReadCount = 0;
	int rewindWriteCount = 0;

	// Process stuff
	DWORD exitCode = 0;

	// UI variables
	std::string P1Text = "\0";
	std::string P2Text = "\0";

	// Config file variables
	int cfg_takeover_countdown_amount;
	int cfg_takeover_countdown_speed;
	int cfg_max_rewind_time;

	// 0: unpaused (normal)
	// 1: paused
	// 2: takeover countdown
	// 3: taking over
	int state = 0;

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

	// Read config file
	delete rewindPool;

	cfg_takeover_countdown_amount =
		GetPrivateProfileInt("takeover", "takeover_countdown_amount", 3, ".\\takeover-config.ini");
	cfg_takeover_countdown_amount = std::min(std::max(cfg_takeover_countdown_amount, 0), 999);

	cfg_takeover_countdown_speed =
		GetPrivateProfileInt("takeover", "takeover_countdown_speed", 30, ".\\takeover-config.ini");
	cfg_takeover_countdown_speed = std::max(cfg_takeover_countdown_speed, 1);

	cfg_max_rewind_time =
		GetPrivateProfileInt("rewind", "max_rewind_time", 30, ".\\takeover-config.ini");
	cfg_max_rewind_time = std::max(cfg_max_rewind_time, 1);

	rewindPoolSize = cfg_max_rewind_time * (60 / 2);
	rewindPool = new struct RewindState [rewindPoolSize];

	takeoverCountdown = cfg_takeover_countdown_amount;
	takeoverCountdownFrames = 0;

	// Game is in replay mode at this point. All is good.

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
		"D      = Rewinds the replay for a maximum of %d seconds.\n"
		"                                                                  \n"
		"Check for new releases at:\n"
		"         https://github.com/wehrliaa/mbaacc-replay-takeover/\n"
		//"rewindPool[%d] is using ~%dMB of memory.\n\n"
		,cfg_max_rewind_time
		//,rewindPoolSize,
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
			state = 0;
			game_state.play();

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

		if ((FN1Frames == 1) && (state < 2)) { // either unpaused or paused
			state = !state; // !0 is 1, !1 is 0

			if (state == 1) {
				P1Text = "PAUSED\0";
				P2Text = "\0";

				save_state.save(&game_state);
				game_state.pause();

			} else if (state == 0) {
				P1Text = "PLAYING\0";
				P2Text = "\0";

				save_state.load(&game_state);
				game_state.play();
			}
		}

		if ((FN2Frames == 1) && (state == 3)) {
			// stop taking over, pause, and load state
			state = 1;

			P1Text = "PAUSED\0";
			P2Text = "\0";

			game_state.untakeover();
			save_state.load(&game_state);
			game_state.pause();
		}

		// Replay takeover
		if ((state == 1) && ((BFrames == 1) || (CFrames == 1))) {
			state = 2;

			P1Text = "TAKING OVER\0";

			takeoverCountdown = cfg_takeover_countdown_amount;
			takeoverCountdownFrames = 0;

			// If B and C are pressed on the same frame, P2 will be selected.
			playerToTakeover = (CFrames == 1) + 1;
		}

		// Countdown before taking over
		if (state == 2) {
			std::stringstream ss;

			if (takeoverCountdownFrames == 0) {
				game_state.aSound1.write_memory((char*)"\x01", 0, false);
				ss.str("");
				ss << "PLAYER " << playerToTakeover << " IN " << takeoverCountdown << "\0";
				P2Text = ss.str();
			}

			takeoverCountdownFrames++;

			if (takeoverCountdownFrames == cfg_takeover_countdown_speed) {
				takeoverCountdown--;
				takeoverCountdownFrames = 0;
			}

			// Countdown has finished, start takeover
			if (takeoverCountdown == 0) {
				state = 3;

				ss.str("");
				ss << "PLAYER " << playerToTakeover << "\0";
				P2Text = ss.str();

				// Load current state before taking over, just to make sure.
				save_state.load(&game_state);

				if (playerToTakeover == 1)
					game_state.takeoverP1();
				else if (playerToTakeover == 2)
					game_state.takeoverP2();

				game_state.play();
			}
		}

		// Replay rewind
		// This is basically a circular buffer that is read backwards from the
		// latest write location.
		if (state == 0) {
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
