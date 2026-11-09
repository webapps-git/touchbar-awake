NAME      := touchbar-awake
LABEL     := local.$(NAME)
PREFIX    := $(HOME)/Library/Application Support/$(NAME)
BIN       := $(PREFIX)/$(NAME)
PLIST     := $(HOME)/Library/LaunchAgents/$(LABEL).plist
LOG       := $(HOME)/Library/Logs/$(NAME).log
DOMAIN    := gui/$(shell id -u)

CFLAGS    := -O2 -Wall -Wextra
FRAMEWORKS := -framework CoreFoundation

.PHONY: all install uninstall status clean

all: $(NAME)

$(NAME): $(NAME).c
	clang $(CFLAGS) $< -o $@ $(FRAMEWORKS)

install: $(NAME)
	@launchctl bootout $(DOMAIN)/$(LABEL) 2>/dev/null || true
	mkdir -p "$(PREFIX)" "$(dir $(PLIST))"
	cp $(NAME) "$(BIN)"
	sed -e 's|@BIN@|$(BIN)|' -e 's|@LOG@|$(LOG)|' $(LABEL).plist.in > "$(PLIST)"
	plutil -lint "$(PLIST)"
	launchctl bootstrap $(DOMAIN) "$(PLIST)"
	@echo "Installed. Log: $(LOG)"

uninstall:
	@launchctl bootout $(DOMAIN)/$(LABEL) 2>/dev/null || true
	rm -f "$(PLIST)"
	rm -rf "$(PREFIX)"
	@echo "Removed. The log file stays at $(LOG)"

status:
	@launchctl print $(DOMAIN)/$(LABEL) 2>/dev/null | grep -E "state =|pid =|last exit" || echo "Not loaded"
	@tail -5 "$(LOG)" 2>/dev/null || true

clean:
	rm -f $(NAME)
