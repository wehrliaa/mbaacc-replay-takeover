# MBAACC Replay Takeover

This is a very simple implementation of replay takeover for Melty Blood Actress Again Current Code, community edition.

The code is based on my [partial rewrite of kosunan's training tool](https://github.com/wehrliaa/mbaacc-training-linux).

## Goals

- [X] Pause the game indefinitely
- [X] Prevent the replay from desyncing after pausing
- [X] Save/load state
- [X] Take over inputs
- [X] Play sounds to indicate current state of the program (about to start takeover, saved or loaded state, etc)
- [X] Replay rewind

## Known issues

None.

## Building and usage

Only dependencies are MinGW-w64 and `make`.

```
git clone https://github.com/wehrliaa/mbaacc-replay-takeover
cd mbaacc-replay-takeover
make
```

You should get an executable called `takeover.exe`. Run it alongside the game in replay mode (Offline > Replay, in CCCaster 3.1).

### Linux

You should be able to run it by just clicking on it, but if that doesn't work, copy the `takeover.sh` script in the repo over to the same directory as `takeover.exe`, and then run the script.

Due to a limitation in (or feature of) Wine, **this tool and the game need to be run in the same Wine prefix**, otherwise it won't detect the game at all. So if you're running the game through stuff like Lutris or Steam, you might have to tweak some stuff to make it work properly. Consider modifying the `takeover.sh` script as well.

## Controls

- FN1: Pauses and unpauses the replay, and while taking over, goes back to where you last paused.
- B and C: With the game paused, takes over Player 1's or Player 2's actions respectively, after a little countdown.
- D: Rewinds the replay, for a maximum of 30 seconds (to save memory).

## Acknowledgements

- [Meepster99](https://github.com/Meepster99/), for providing me with her Ghidra project for this game. This would've taken a lot longer to make if it wasn't for that, so thanks a lot!

## License

This is public domain software.
