# How it works

## What macOS does with the Touch Bar

The Touch Bar is driven by `/usr/libexec/TouchBarServer`, a root process. It watches input
from keyboards, trackpads and mice through the HID event system. After 60 seconds without
input it starts putting the Touch Bar to sleep:

| Time without input | Dimming step | Panel |
|---|---|---|
| under 60 s | `1` (full brightness) | on |
| 60 s | `2` (dimmed) | on, touches ignored |
| 75 s | `4` (off) | disabled |

The step values are what `DFRBrightnessClientGetDimmingStep` returns. In the system log
`TouchBarServer` names the same states `BrightnessStep: 0`, `1` and `2`.

The same `4` / disabled state is used when the display turns off, for example after locking the
screen (Ctrl+Cmd+Q) and pressing Esc.

On affected machines the right end of the panel, next to Touch ID, strobes while it is in the
`4` / disabled state.

## What touchbar-awake changes

Every 2 seconds it reads the dimming step. If the step is not `1`, it sets the step back to `1`
through the private `DFRBrightness` framework:

```c
void *client = DFRBrightnessClientCreate(kCFAllocatorDefault);
long step    = DFRBrightnessClientGetDimmingStep(client);
DFRBrightnessClientDisplaySetDimmingStep(client, 1);
```

The functions are loaded at runtime with `dlopen` / `dlsym`. No headers or extra frameworks are
needed.

`TouchBarServer` keeps the panel disabled, so the Touch Bar stays dark. With the step at `1`
instead of `4`, the flicker has not come back in our testing. When you touch the keyboard or
trackpad again, `TouchBarServer` wakes the Touch Bar as usual.

It does this whether or not the display is on. An earlier version skipped the reset while the
display was off, and the flicker came back on a locked screen.

## Watching it yourself

Follow `TouchBarServer` while you leave the Mac alone for about 90 seconds:

```bash
/usr/bin/log stream --info --predicate 'process == "TouchBarServer" AND (category == "hid" OR category == "brightness" OR category == "display")' --style compact
```

You will see roughly this (with or without touchbar-awake running):

```
[com.apple.touchbarserver:hid] HIDActivity state: 0, ...
[com.apple.touchbarserver:brightness] BrightnessStep: 1
[com.apple.touchbarserver:brightness] BrightnessStep: 2
[com.apple.touchbarserver:display] DFRDisplay: enabled: false
```

To see the brightness changes touchbar-awake makes, filter on the DFRBrightness subsystem:

```bash
/usr/bin/log stream --info --predicate 'subsystem == "com.apple.CoreBrightness.DFRBrightness"' --style compact
```

Each reset shows up as `touchbar-awake ... Set property 1 for key = DFRDimmingStep`.

Past events are available with `/usr/bin/log show --last 30m` and the same predicate.

Use the full path `/usr/bin/log`: in zsh, the default macOS shell, plain `log` is a built-in
command that does something else.
