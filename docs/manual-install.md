# Manual install

`make install` does the steps below. Use them if you want to install by hand or put the files
somewhere else.

## 1. Build

```bash
clang -O2 -Wall -Wextra touchbar-awake.c -o touchbar-awake -framework CoreFoundation -framework CoreGraphics
```

Try it in the foreground first (Ctrl+C to stop):

```bash
./touchbar-awake
```

Leave the Mac alone for about 75 seconds. You should see:

```
touchbar-awake: started, checking every 2 s
2026-11-09 14:05:48 reset dimming step: step 2 -> 1
2026-11-09 14:06:02 reset dimming step: step 4 -> 1
```

## 2. Copy the program

```bash
mkdir -p ~/Library/Application\ Support/touchbar-awake
cp touchbar-awake ~/Library/Application\ Support/touchbar-awake/
```

## 3. Create the LaunchAgent

Save this as `~/Library/LaunchAgents/local.touchbar-awake.plist`, replacing `YOUR_USER` with
your user name (`whoami`). launchd does not expand `~` or `$HOME`, so the paths must be absolute.

```xml
<?xml version="1.0" encoding="UTF-8"?>
<!DOCTYPE plist PUBLIC "-//Apple//DTD PLIST 1.0//EN" "http://www.apple.com/DTDs/PropertyList-1.0.dtd">
<plist version="1.0">
<dict>
    <key>Label</key>
    <string>local.touchbar-awake</string>
    <key>ProgramArguments</key>
    <array>
        <string>/Users/YOUR_USER/Library/Application Support/touchbar-awake/touchbar-awake</string>
        <string>2</string>
    </array>
    <key>RunAtLoad</key>
    <true/>
    <key>KeepAlive</key>
    <dict>
        <key>Crashed</key>
        <true/>
    </dict>
    <key>LimitLoadToSessionType</key>
    <string>Aqua</string>
    <key>ProcessType</key>
    <string>Background</string>
    <key>StandardOutPath</key>
    <string>/Users/YOUR_USER/Library/Logs/touchbar-awake.log</string>
    <key>StandardErrorPath</key>
    <string>/Users/YOUR_USER/Library/Logs/touchbar-awake.log</string>
</dict>
</plist>
```

What the keys do:

- `RunAtLoad` starts it at login.
- `KeepAlive` → `Crashed` restarts it if it crashes, but not if it exits with an error (for
  example on a Mac without a Touch Bar).
- `LimitLoadToSessionType` → `Aqua` runs it only in a graphical login session, where the
  display checks work.
- The second `ProgramArguments` entry is the check interval in seconds.

Check the file:

```bash
plutil -lint ~/Library/LaunchAgents/local.touchbar-awake.plist
```

## 4. Start it

```bash
launchctl bootstrap gui/$(id -u) ~/Library/LaunchAgents/local.touchbar-awake.plist
```

## Remove it

```bash
launchctl bootout gui/$(id -u)/local.touchbar-awake
```

```bash
rm ~/Library/LaunchAgents/local.touchbar-awake.plist
```

```bash
rm -r ~/Library/Application\ Support/touchbar-awake
```
