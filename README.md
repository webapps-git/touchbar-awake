# touchbar-awake

A tiny background agent that stops the **Touch Bar flicker next to Touch ID** on MacBook Pro.

On some MacBook Pros the right end of the Touch Bar, right next to the Touch ID sensor, starts
strobing after the Touch Bar goes to sleep. `touchbar-awake` stops this without keeping the
Touch Bar lit, so it does not add OLED wear and does not keep your Mac awake.

## How it works

When you stop using the keyboard and trackpad, `TouchBarServer` (the macOS process that drives
the Touch Bar) does this:

1. After about 60 seconds it dims the Touch Bar (dimming step `2`).
2. About 15 seconds later it sets the dimming step to "off" (`4`) and disables the panel.

The same happens when the display turns off, for example on a locked screen.

The flicker shows up in that sleeping state. `touchbar-awake` checks the dimming step every
2 seconds and resets it to full brightness (`1`). `TouchBarServer` still turns the panel off,
but with the step at full the flicker stops.

It talks to the private `DFRBrightness` framework that macOS uses for Touch Bar brightness.
No root, no kernel extension, no system settings changed. It uses about 4 MB of RAM and almost
no CPU.

## Status

This is a workaround found by experiment, not an official fix.

| | |
|---|---|
| Tested on | MacBookPro15,1 (15-inch 2018, Intel, T2), macOS 15.8.2 |
| Result | No flicker seen in two days of normal use |
| Apple silicon Touch Bar Macs (13-inch M1/M2) | **Not tested** |

If it works (or doesn't) on your model, please open an issue with your model identifier
(`sysctl -n hw.model`) and macOS version.

## Install

You need the Xcode Command Line Tools (`xcode-select --install`).

```bash
git clone https://github.com/webapps-git/touchbar-awake.git
cd touchbar-awake
make install
```

`make install` builds the program, copies it to `~/Library/Application Support/touchbar-awake/`,
adds a LaunchAgent at `~/Library/LaunchAgents/local.touchbar-awake.plist` and starts it. It
starts automatically every time you log in. macOS may show a notification that a background
item was added.

## Check that it runs

```bash
make status
```

Each time the Touch Bar goes to sleep you should see lines like these in
`~/Library/Logs/touchbar-awake.log`:

```
2026-11-09 14:05:48 reset dimming step: step 2 -> 1
2026-11-09 14:06:02 reset dimming step: step 4 -> 1
```

The Touch Bar still goes dark after about 75 seconds of inactivity. That is expected.

## Uninstall

```bash
make uninstall
```

## Run without installing

```bash
make
./touchbar-awake      # Ctrl+C to stop
```

An optional argument sets the check interval in seconds (default `2`).

## Documentation

- [How it works](docs/how-it-works.md): what macOS does with the Touch Bar, what this
  program changes, and how to watch it in the system log.
- [Manual install](docs/manual-install.md): build and set up the LaunchAgent by hand,
  with the full plist and what each key does.
- [Troubleshooting](docs/troubleshooting.md): checking that it runs, reading the log, common
  errors.

## What did not work

Things tried before this approach, in case you want to go further:

- **Keeping the Touch Bar lit.** Resetting the dimming step does not turn the panel back on:
  `TouchBarServer` disables the panel itself. It only wakes up again on real input from a
  keyboard, trackpad or mouse.
- **`caffeinate -u`** (`IOPMAssertionDeclareUserActivity`) does not wake the Touch Bar.
- **Synthetic mouse events** (`CGEventPost`) do not wake it either.
- **A virtual HID device** cannot be created without root or a special entitlement.

## Caveats

- It relies on a private framework. A macOS update can break it. If that happens the log shows
  an error, or no new lines appear when the Touch Bar sleeps.
- It works around the flicker. If your panel has a hardware fault, it does not fix that.

## License

[MIT](LICENSE)
