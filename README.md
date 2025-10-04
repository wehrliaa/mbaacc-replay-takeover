# MBAACC Replay Takeover

This is a very simple implementation of replay takeover for Melty Blood Actress Again Current Code, community edition.

The code is based on my [partial rewrite of kosunan's training tool](https://github.com/wehrliaa/mbaacc-training-linux).

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

- FN1: Pauses and unpauses the replay, inside or outside of takeover
- FN2: While taking over, resets back to where you last paused, and takes over the last selected player
- B and C: With the game paused, takes over Player 1's or Player 2's actions respectively, after a little countdown.
- D: Rewinds the replay, for a maximum of 30 seconds (can be configured. see Configuration below).

## Configuration

You can change how the tool behaves by editing the included `takeover-config.ini` file. Here's a summary of it:

- `takeover_countdown_amount`: How long the countdown before takeover should be.
- `takeover_countdown_speed`: How fast each count(?) of the countdown is, in milliseconds.
- `max_rewind_time`: Time limit for the replay rewind, in seconds.

The configuration file is (re)loaded every time the game is (re)opened.

## Acknowledgements

- [Meepster99](https://github.com/Meepster99/), for providing me with her Ghidra project for this game. This would've taken a lot longer to make if it wasn't for that.
- Kou and Lewi, for having helped me find bugs during beta.

## License

This is public domain software.
