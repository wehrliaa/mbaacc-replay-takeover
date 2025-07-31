# general idea

basically while i was thinking about how i would implement this, i thought that maybe there would some kind of iterator thing that keeps track of which part of the replay file is being read at the moment. turns out that might actually be true, but getting a hold of that value will probably be quite an involved process. i need that value so i can load states without desync the replay: i would load each character's positions, actions, health values, meter, etc, *and* where in the replay that moment happened.

when i say "replay iterator", i'm referring to that number.

# numbers in MBAA.CT

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

replay iterator is 225 + game timer (past intro state 0)

# asm stuff

for some reason the game uses a different ~~address~~ variable to store the replay iterator for each round.

there's always going to be more than 1 address for this. one stops getting incremented after the winning character begins its win pose animation (presumably the end of that section in the replay file), the other one stops after the screen transition animation starts.

## functions in ghidra

Assuming the game is in replay mode (actually watching a replay, not the replay menu)...

- `40e390` is "the main,, 'function' that is called once a frame".
- `432c50` "updates the game (i think)(".
- `444d00` seems to be "called at roundstarts and replay read".
- `444d60` is ??? but contains 444e06 which every frame past intro state 1 adds 1 to something that looks like a replay iterator.
- `44c3f0` is called when the game is paused (press Start). sets `g_ISPAUSED` to 1.
- `44c8f0` is called when the game is unpaused (exit pause menu). sets `g_ISPAUSED` to 0.
- `48e0a0` "handles inputs (i believe, im not sure)".

At `444d00`:  
```
for (i = 0;
	(iVar1 = *(int *)(unaff_ESI + 0x120), iVar1 != 0 && (i < (uint)(*(int *)(unaff_ESI + 0x124) - iVar1 >> 5)));
	i = i + 1)
{
	if ((iVar1 == 0) || ((uint)(*(int *)(unaff_ESI + 0x124) - iVar1 >> 5) <= i)) {
		UnrecoveredJumptable___invalid_parameter_noinfo_IDA?();
	}
...
```

At `444d60`:  
```
for (i = 0;
	(iVar1 = *(int *)(unaff_EBX + 0x120), iVar1 != 0 && (i < (uint)(*(int *)(unaff_EBX + 0x124) - iVar1 >> 5)));
	i = i + 1)
{
	if ((iVar1 == 0) || ((uint)n2 <= i)) {
		UnrecoveredJumptable___invalid_parameter_noinfo_IDA?();
	}
```

Only difference is `unaff_ESI` and `unaff_EBX`, but outside of that it's the exact same chunk of code.

Simplified:

```
for (i = 0;
	(iVar1 = n1, iVar1 != 0 && (i < ((uint)n2 - iVar1 >> 5)));
	i = i + 1)
{
	if ((iVar1 == 0) || ((uint)(n2 - iVar1 >> 5) <= i)) {
		UnrecoveredJumptable___invalid_parameter_noinfo_IDA?();
	}
...
```
