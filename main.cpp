// main.cpp: Main file

#include "main.h"

// This was declared in process_handling.h first.
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

	//printf("base address is %lu\n", gProc.baddr);
	//return 0;

	int prev_frame_count = 0;
	int global_frame_count = 0;

	Save_State_Manager save_state;
	Game_State_Manager game_state;
	Action_Handler action_handler = Action_Handler(game_state, save_state);

	while (1) {
		game_state.fetch_game_data();

		//if (game_state.aGameMode.int_data == 20) {
		//	action_handler.is_save_flag = false;
		//	printf("\rIn character select...            ");
		//}

		// Everything below this chunk of code is synced with the game's
		// framerate.
		// TODO: Change this to replay timer, so you can pause while in intro state 1
		global_frame_count = game_state.timer_check();
		if (global_frame_count == prev_frame_count)
			continue;

		prev_frame_count = global_frame_count;

		action_handler.action_handle();
	}

	return 0;
}
