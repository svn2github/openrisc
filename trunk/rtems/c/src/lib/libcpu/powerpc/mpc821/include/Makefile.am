##
## $Id: Makefile.am,v 1.2 2001-09-27 12:01:26 chris Exp $
##

AUTOMAKE_OPTIONS = foreign 1.4

H_FILES = console.h mpc821.h
noinst_HEADERS = $(H_FILES)

TMPINSTALL_FILES += $(PROJECT_INCLUDE)/mpc821 \
    $(H_FILES:%=$(PROJECT_INCLUDE)/mpc821/%)

$(PROJECT_INCLUDE)/mpc821:
	$(mkinstalldirs) $@

$(PROJECT_INCLUDE)/mpc821/%.h: %.h
	$(INSTALL_DATA) $< $@

all-local: $(TMPINSTALL_FILES)

include $(top_srcdir)/../../../../../automake/local.am
