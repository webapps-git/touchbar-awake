# Troubleshooting

## Is it running?

```bash
make status
```

or without `make`:

```bash
launchctl print gui/$(id -u)/local.touchbar-awake | grep -E "state =|pid =|last exit"
```

```bash
tail ~/Library/Logs/touchbar-awake.log
```

You should see `state = running` and a `pid`.

## It is running but the log shows no resets

The log only gets a line when the Touch Bar goes to sleep. Leave the Mac alone for about
75 seconds with the display on, then check again.

If there is still nothing, see whether `TouchBarServer` put the Touch Bar to sleep at all:

```bash
/usr/bin/log show --last 10m --info --predicate 'process == "TouchBarServer" AND category == "brightness"' --style compact
```

If it shows `BrightnessStep: 2` but touchbar-awake logged nothing, the private API probably
changed in your macOS version. Please open an issue with your macOS version.

## The log says "DFRBrightness ... not found" or "Could not create DFRBrightness client"

Your Mac has no Touch Bar, or your macOS version no longer has these private functions.
The agent exits and launchd does not restart it. Remove it with `make uninstall`.

## "last exit code" is not 0

Look at the end of the log for the error message:

```bash
tail -20 ~/Library/Logs/touchbar-awake.log
```

## The Touch Bar still flickers

touchbar-awake only changes the dimming step. If the flicker continues:

1. Confirm the resets happen at the moment the Touch Bar goes dark (see above).
2. Restart the Mac and check again.
3. If it still flickers, it may be a hardware fault that software cannot fix.

Please report your model (`sysctl -n hw.model`) and macOS version either way. It helps
others.

## macOS shows "Background item added"

That is macOS reporting the new LaunchAgent. You can see or turn it off in
System Settings → General → Login Items & Extensions. If you turn it off there, it will not
start at login.
