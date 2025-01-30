#
# Copyright (C) 1999 Sony Computer Entertainment Inc., All Rights Reserved.
#
#

SUBDIRS = ee gamed

all:
	(for dir in $(SUBDIRS); do\
		cd $$dir; $(MAKE) all; cd ..; done)

install:
	(for dir in $(SUBDIRS); do\
		cd $$dir; $(MAKE) install; cd ..; done)

clean: 
	(for dir in $(SUBDIRS); do\
		cd $$dir; $(MAKE) clean; cd ..; done)
