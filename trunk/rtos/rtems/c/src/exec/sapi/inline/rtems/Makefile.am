##
## $Id: Makefile.am,v 1.2 2001-09-27 11:59:20 chris Exp $
##

AUTOMAKE_OPTIONS = foreign 1.4

if INLINE
I_FILES = extension.inl

noinst_HEADERS = $(I_FILES)

PREINSTALL_FILES += $(PROJECT_INCLUDE)/rtems \
    $(noinst_HEADERS:%=$(PROJECT_INCLUDE)/rtems/%)

$(PROJECT_INCLUDE)/rtems:
	@$(mkinstalldirs) $@
$(PROJECT_INCLUDE)/rtems/%.inl: %.inl
	$(INSTALL_DATA) $< $@
endif

all-local: $(PREINSTALL_FILES)

include $(top_srcdir)/../../../automake/local.am
