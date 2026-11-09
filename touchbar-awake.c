// touchbar-awake: stops the Touch Bar flicker next to Touch ID on MacBook Pro.
//
// When the Mac is idle, or the display is off, TouchBarServer sets the Touch Bar dimming step
// to "off" and disables the panel. On affected machines the right edge of the panel then
// strobes. This program watches the dimming step and resets it to full brightness. The panel
// itself stays off; it just stops flickering.
//
// Uses the private DFRBrightness framework, so a macOS update may break it.
//
// Build: make
// Usage: touchbar-awake [seconds_between_checks]   (default 2)
#include <CoreFoundation/CoreFoundation.h>
#include <dlfcn.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#define DFR_PATH "/System/Library/PrivateFrameworks/DFRBrightness.framework/Versions/A/DFRBrightness"
#define STEP_FULL 1 // measured: 1 = full brightness, 2 = dimmed, 4 = off

typedef void *(*client_create_fn)(CFAllocatorRef);
typedef long (*client_get_step_fn)(void *);
typedef long (*client_set_step_fn)(void *, long);

static void log_line(const char *msg, long before, long after) {
    char ts[32];
    time_t now = time(NULL);
    strftime(ts, sizeof ts, "%Y-%m-%d %H:%M:%S", localtime(&now));
    printf("%s %s: step %ld -> %ld\n", ts, msg, before, after);
    fflush(stdout);
}

int main(int argc, char **argv) {
    int interval = argc > 1 ? atoi(argv[1]) : 2;
    if (interval < 1) interval = 2;

    void *lib = dlopen(DFR_PATH, RTLD_NOW);
    if (!lib) { fprintf(stderr, "DFRBrightness not found (no Touch Bar?): %s\n", dlerror()); return 1; }
    client_create_fn create = (client_create_fn)dlsym(lib, "DFRBrightnessClientCreate");
    client_get_step_fn get_step = (client_get_step_fn)dlsym(lib, "DFRBrightnessClientGetDimmingStep");
    client_set_step_fn set_step = (client_set_step_fn)dlsym(lib, "DFRBrightnessClientDisplaySetDimmingStep");
    if (!create || !get_step || !set_step) { fprintf(stderr, "DFRBrightness functions not found\n"); return 1; }

    void *client = create(kCFAllocatorDefault);
    if (!client) { fprintf(stderr, "Could not create DFRBrightness client\n"); return 1; }

    printf("touchbar-awake: started, checking every %d s\n", interval);
    fflush(stdout);
    for (;;) {
        long step = get_step(client);
        if (step != STEP_FULL) {
            set_step(client, STEP_FULL);
            log_line("reset dimming step", step, get_step(client));
        }
        sleep(interval);
    }
}
