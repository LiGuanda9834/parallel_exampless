SUBDIRS = group p2p

all:
	for DIR in $(SUBDIRS); do (cd $$DIR && $(MAKE) $@) || exit 1; done
	
clean:
	for DIR in $(SUBDIRS); do (cd $$DIR && $(MAKE) $@) || exit 1; done

cleanall:
	for DIR in $(SUBDIRS); do (cd $$DIR && $(MAKE) $@) || exit 1; done
