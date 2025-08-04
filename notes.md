crossed-out text ~~like this~~ are to be ignored. they are old ideas that turned out to not be 100% correct.

# general idea

basically while i was thinking about how i would implement this, i thought that maybe there would some kind of iterator thing that keeps track of which part of the replay file is being read at the moment. turns out that might actually be true, but getting a hold of that value will probably be quite an involved process. i need that value so i can load states without desyncing the replay: i would load each character's positions, actions, health values, meter, etc, *and* where in the replay that moment happened.

when i say "replay iterator", i'm referring to that number.

actually... scratch that, the "replay iterator" might be more than a single number. see below.

# memory addresses and offsets et cetera

seems like the game uses a different ~~address~~ ~~variable~~ address to store the replay iterator for each round.

~~there's always going to be more than 1 address for this. one stops getting incremented after the winning character begins its win pose animation (presumably the end of that section/round in the replay file) (wp), the other one stops after the screen transition animation starts (st).~~ this is actually all... not really "wrong", but it doesn't show the whole picture. just read the diagram below.

- "round N wp" values seem to be just general frame counters. they are incremented every frame, and stop being incremented at the start of the winning character's Win Pose animation (hence "wp").
- "round N st" values also seem to be general frame counters. they are incremented every frame, but stop being incremented at the start of the Screen Transition animation (hence "st"), and thus end at a higher value than round N wp, when the next round starts.

"round N wp" and "round N st" can have the same value at the start of the next round if you mash to skip the win pose animation.

```
(p1 is player 1, p2 is player 2)

x                             = some kind of base address, idk        (? bytes) (ex: 0x29C1F90)
y                             = some other kind of base address, idk  (? bytes) (ex: 0x2A88258)

Round 1:

x + 0x10                      = round 1 wp                            (4 bytes) (ex: 0x29C1FA0)
x + 0x18                      = player 1's Nth input in the replay    (4 bytes) (ex: 0x29C1FA8)
x + 0x1C                      = frames since player 1's last input    (4 bytes) (ex: 0x29C1FAC)

x + 0x20 + 0x10               = r1 wp but for p2? same value as r1 wp (4 bytes) (ex: 0x29C1FC0)
x + 0x20 + 0x18               = player 2's Nth input in the replay    (4 bytes) (ex: 0x29C1FC8)
x + 0x20 + 0x1C               = frames since player 2's last input    (4 bytes) (ex: 0x29C1FCC)

y + 0x12C                     = round 1 st                            (4 bytes) (ex: 0x2A88384)

Round 2:

x + 0x90 + 0x10               = round 2 wp                            (4 bytes) (ex: 0x29C2030)
x + 0x90 + 0x18               = player 1's Nth input in the replay    (4 bytes) (ex: 0x29C2038)
x + 0x90 + 0x1C               = frames since player 1's last input    (4 bytes) (ex: 0x29C203C)

x + 0x90 + 0x20 + 0x10        = r2 wp but for p2? same value as r2 wp (4 bytes) (ex: 0x29C2050)
x + 0x90 + 0x20 + 0x18        = player 2's Nth input in the replay    (4 bytes) (ex: 0x29C2058)
x + 0x90 + 0x20 + 0x1C        = frames since player 2's last input    (4 bytes) (ex: 0x29C205C)

y + 0x140 + 0x12C             = round 2 st                            (4 bytes) (ex: 0x2A884C4)

Round 3:

x + 0x90 + 0x90 + 0x10        = round 3 wp                            (4 bytes) (ex: 0x29C20C0)
x + 0x90 + 0x90 + 0x18        = player 1's Nth input in the replay    (4 bytes) (ex: 0x29C20C8)
x + 0x90 + 0x90 + 0x1C        = frames since player 1's last input    (4 bytes) (ex: 0x29C20CC)

x + 0x90 + 0x90 + 0x20 + 0x10 = r3 wp but for p2? same value as r3 wp (4 bytes) (ex: 0x29C20E0)
x + 0x90 + 0x90 + 0x20 + 0x18 = player 2's Nth input in the replay    (4 bytes) (ex: 0x29C20E8)
x + 0x90 + 0x90 + 0x20 + 0x1C = frames since player 2's last input    (4 bytes) (ex: 0x29C20EC)

y + 0x140 + 0x140 + 0x12C     = round 3 st                            (4 bytes) (ex: 0x2A88604)

```

it goes on and on like this for later rounds, for a total of 6 times (maximum of 5 rounds in 3 win battle + final round)

i can confirm that changing these the "Nth input" and "frames since last input" with Cheat Engine desyncs the replay. it also seems that the game keeps track of how long each Nth input lasts.

let's say player 1's 28th input lasts for exactly 16 frames. if you pause the game in the middle of it, reset the "frames since last input" counter, and unpause, player 1 will still hold that input until that counter reaches 15 (16 - 1. or maybe it's just 16. idk). if you do this forever, player 1 will keep holding that input forever. after it reaches its target value (15 or 16 in this case), it will proceed as normal, but now it is heavily desynced.

anyway, this whole thing is extremely important, and will prevent desyncs when loading states.

...

actually it's like this

```c
// the address leading to here is stored at [0x77bf98]+0x120
struct PlayerReplayData {      // 32 (0x20) bytes long

	// offset 0x00 is unknown or undefined. looking at it with cheat engine
	// doesn't tell me anything.

	int hugenumber1;           // 4 bytes long, offset 0x04
	int hugenumber2;           // 4 bytes long, offset 0x08

	// (hugenumber2 - hugenumber1) >> 3 = number of total inputs

	// offset 0x0C is unknown or undefined

	int roundN_wp;             // 4 bytes long, offset 0x10

	// offset 0x14 is unknown or undefined

	uint NthInput;             // 4 bytes long, offset 0x18
	uint framesSinceLastInput; // 4 bytes long, offset 0x1C
}
```

1. there's a maximum of 6 rounds
2. each round's data struct or array (don't know which one it is yet) is 144 (0x90) bytes long
3. 64 (0x40) bytes of that is each player's data (32 bytes each). idk what the remaining 80 (0x50) bytes are.
4. 144 * 6 = 864 (0x360)
5. the entire array of rounds could be at least 864 (0x360) bytes long

`77bf34+64` holds the address to something
`77bf34+70` is the current round number (zero-indexed)
`[77bf34+64]+120` holds the address leading to the start of PlayerReplayData stuff

## relevant functions found with ghidra

Assuming the game is in replay mode (actually watching a replay, not the replay menu)...

- `40e390` is "the main,, 'function' that is called once a frame".
- `432c50` "updates the game (i think)(".
- `444b10` reads the value of "frames since player N's last input". doesn't look like it does much else.
- `444d00` is "called at roundstarts and replay read". among other things, it resets round N wp and everything related to it to 0
- `444d60` mainly updates the values of everything in the diagram above. very important function to look at.
- - called by `4463a0`
- - - called by `4412c0`. sets ESI to `0x77bf34`
- `44c3f0` is called when the game is paused (press Start). sets `g_ISPAUSED` to 1.
- `44c8f0` is called when the game is unpaused (exit pause menu). sets `g_ISPAUSED` to 0.
- `48e0a0` "handles inputs (i believe, im not sure)".

# numbers in MBAA.CT (Cheat Engine)

game mode:
- 1 game
- 5 menu
- 8 vs screen
- 20 char select
- 26 replay selection menu

intro state:
- 2 character intros
- 1 pre-round start (characters can move but not attack)
- 0 round start (can attack now)

~~replay iterator~~ round N wp value is 225 + game timer (past intro state 0)

# diary thing

maybe it would be a better idea to call `444d00`, since that seems to be the only function capable of affecting the replay iterator. but let me try out something first.

i forgot what i wanted to "try out" here but oh well.
