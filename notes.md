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

for some reason the game uses a different ~~address~~ ~~variable~~ address to store the replay iterator for each round, but there are a few consistencies.

there's always going to be more than 1 address for this. one stops getting incremented after the winning character begins its win pose animation (presumably the end of that section/round in the replay file) (wp), the other one stops after the screen transition animation starts (st).

it is almost always of the form:

```
x                             = ???        (ex: 029CAD20) (4 bytes)
x + 0x10                      = round 1 wp (ex: 029CAD30) (4 bytes)
x + 0x10 + 0x20               = round 1 wp (ex: 029CAD50) (4 bytes)
x + 0x10 + 0x70               = round 2 wp (ex: 029CADC0) (4 bytes)
x + 0x10 + 0x70 + 0x20        = round 2 wp (ex: 029CADE0) (4 bytes)
x + 0x10 + 0x70 + 0x70        = round 3 wp (ex: 029CAE50) (4 bytes)
x + 0x10 + 0x70 + 0x70 + 0x20 = round 3 wp (ex: 029CAE70) (4 bytes)

y                         = ???        (ex: 02A87B88) (4 bytes)
y + 0x12C                 = round 1 st (ex: 02A87CB4) (4 bytes)
y + 0x12C + 0x140         = round 2 st (ex: 02A87DF4) (4 bytes)
y + 0x12C + 0x140 + 0x140 = round 3 st (ex: 02A87F34) (4 bytes)
```

my impression is that `round N wp` more closely resembles a replay iterator than `round N st`, which seems to be a general round timer.

`round N wp` is incremented by `444d60`, and reset to 0 by `444d00`.

## functions in ghidra

Assuming the game is in replay mode (actually watching a replay, not the replay menu)...

- `40e390` is "the main,, 'function' that is called once a frame".
- `432c50` "updates the game (i think)(".
- `444d00` is "called at roundstarts and replay read". among other things, it resets something that looks like a replay iterator to 0.
- `444d60` is ??? but contains 444e06 which every frame past intro state 1 adds 1 to something that looks like a replay iterator.
- - `4463a0` calls `444d60`.
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
