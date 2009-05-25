##
## $Id: Makefile.am,v 1.2 2001-09-27 12:01:11 chris Exp $
##

AUTOMAKE_OPTIONS = foreign 1.4

H_FILES = bsp.h coverhd.h

$(PROJECT_INCLUDE):
	$(mkinstalldirs) $@

$(PROJECT_INCLUDE)/%.h: %.h
	$(INSTALL_DATA) $< $@

PREINSTALL_FILES += $(PROJECT_INCLUDE) $(H_FILES:%.h=$(PROJECT_INCLUDE)/%.h)

all-local: $(PREINSTALL_FILES)

EXTRA_DIST = bsp.h coverhd.h

include $(top_srcdir)/../../../../../../automake/local.am
