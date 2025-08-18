# MBAACC Replay Takeover

This is a very simple implementation of replay takeover for Melty Blood Actress Again Current Code, community edition.

The code is based on my [partial rewrite of kosunan's training tool](https://github.com/wehrliaa/mbaacc-training-linux).

## Note for Linux users!!!!

**Due to a limitation in (or feature of) Wine, this tool and the game need to be run in the same Wine prefix**, otherwise it won't detect the game at all. So if you're running the game through Lutris, Proton, Steam, etc, you might run into issues.

## Goals

- [X] Pause the game indefinitely
- [X] Prevent the replay from desyncing after pausing
- [X] Save/load state
- [X] Take over inputs
- [ ] Play sounds to indicate current state of the program (about to start takeover, saved or loaded state, etc)

## Building and usage

Only dependencies are MinGW-w64 and `make`.

```
git clone https://github.com/wehrliaa/mbaacc-replay-takeover
cd mbaacc-replay-takeover
make
```

You should get an executable called `takeover.exe`. Run it alongside the game in replay mode (Offline > Replay, in CCCaster 3.1).

**Linux users**: Run it from your terminal with Wine, Proton, whatever you use.

## Controls

With the replay unpaused:

- Press FN1 to pause the replay.

With the replay paused:

- Press FN1 to unpause.
- Press B to take over Player 1's actions.
- Press C to take over Player 2's actions.

While taking over:

- Press FN1 to go back to where you first paused before taking over, and pause the replay.

## Acknowledgements

- [Meepster99](https://github.com/Meepster99/), for helping me with the Ghidra stuff.

## License

This is public domain software.
