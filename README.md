# MBAACC Replay Takeover

This is an attempt at implementing replay takeover for Melty Blood Actress Again Current Code, community edition.

The code is based on my [partial rewrite of kosunan's training tool](https://github.com/wehrliaa/mbaacc-training-linux).

Huge thanks to [Meepster99](https://github.com/Meepster99/) for helping me with this!

## Note for Linux users!!!!

**Due to a limitation in (or feature of) Wine, this tool and the game need to be run in the same Wine prefix**, otherwise it won't detect the game at all. So if you're running the game through Lutris, Proton, Steam, etc, you might run into issues.

## Goals

- [X] Pause the game indefinitely (gotta use the super flash freeze tech)
- [ ] Prevent the replay from desyncing after pausing
- [ ] Save/load state
- [ ] Take over inputs
- [ ] Play sounds to indicate current state of the program (about to start takeover, saved or loaded state, etc)

## Known issues

- Desyncs if you pause during a motion input. For some reason the input interpreter ignores what came before pausing?

## Building and usage

Only dependencies are MinGW-w64 and `make`.

```
git clone https://github.com/wehrliaa/mbaacc-replay-takeover
cd mbaacc-replay-takeover
make
```

You should get an executable called `takeover.exe`. Run it from your terminal alongside the game in replay mode (Main menu > Offline > Replay, in CCCaster 3.1).

- Press FN1 to pause the replay. Press it again to unpause.

## License

This is public domain software.
