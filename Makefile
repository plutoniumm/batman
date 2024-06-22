EXECUTABLE=batman
PLIST=batman.plist
INSTALLDIR=/usr/local/bin
PLISTFULL=/Library/LaunchDaemons/$(PLIST)

all: batman.c
	rm ./$(EXECUTABLE)
	gcc batman.c utils.c -framework ApplicationServices -framework Carbon -o $(EXECUTABLE)

install: all
	mkdir -p $(INSTALLDIR)
	cp $(EXECUTABLE) $(INSTALLDIR)

uninstall:
	launchctl unload $(PLISTFULL)
	rm $(INSTALLDIR)/$(EXECUTABLE)
	rm -f $(PLISTFULL)

startup: install
	cp $(PLIST) $(PLISTFULL)
	launchctl load -w $(PLISTFULL)

load:
	launchctl load $(PLISTFULL)

unload:
	launchctl unload $(PLISTFULL)

clean:
	rm $(EXECUTABLE)

kill:
	killall $(EXECUTABLE)
	kill -9 $(shell ps aux | grep $(EXECUTABLE) | grep -v grep | awk '{print $$2}')