#
#****************************************************************************#
#                                                                            #
#                    H E A D E R   I N F O R M A T I O N                     #
#                                                                            #
#****************************************************************************#

# Project Name                   : OpenRISC Debug Proxy
# File Name                      : Makefile
# Prepared By                    : jb
# Project Start                  : 2008-10-01

#$$COPYRIGHT NOTICE#
#****************************************************************************#
#                                                                            #
#                      C O P Y R I G H T   N O T I C E                       #
#                                                                            #
#****************************************************************************#

#  This library is free software; you can redistribute it and/or
#  modify it under the terms of the GNU Lesser General Public
#  License as published by the Free Software Foundation; 
#  version 2.1 of the License, a copy of which is available from
#  http://www.gnu.org/licenses/old-licenses/lgpl-2.1.txt.
#
#  This library is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
#  Lesser General Public License for more details.
#
#  You should have received a copy of the GNU Lesser General Public
#  License along with this library; if not, write to the Free Software
#  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301
#  USA.
#

#$$CHANGE HISTORY#
#****************************************************************************#
#                                                                            #
#                         C H A N G E  H I S T O R Y                         #
#                                                                            #
#****************************************************************************#

# Date		Version	Description
#------------------------------------------------------------------------
# 090301	0.1.0	Makefile for OpenRISC debug proxy.             	jb
#  2 Apr 09	0.1.1	Jeremy Bennett. Added static target
#  1 May 09	0.1.3	Added specific make target for VPI		jb
# 090828        0.1.4   Added ftdi_sio driver reload rule               jb

#DBGCPPFLAGS=-DDEBUG_GDB=1
#DBGCPPFLAGS=-DDEBUG_USB_DRVR_FUNCS=1
#DBGCPPFLAGS=-DDEBUG_USB_DRVR_FUNCS=1 -DDEBUG_GDB=1 -DDEBUG_CMDS=1

# Common Flags
COMMON_CPPFLAGS = -I./includes
COMMON_CXXFLAGS = -O3
COMMON_LDFLAGS  = 

# Flags for static library
STATIC_LIBDIR   = lib
STATIC_LIB      = libftd2xx.a.0.4.16
STATIC_LDFLAGS  = $(COMMON_LDFLAGS) $(STATIC_LIBDIR)/$(STATIC_LIB) \
                  -lpthread -ldl
# Defines to enable certain endpoint handling functions to be used
USB_FLAGS	   = -D USB_ENDPOINT_ENABLED=1
VPI_FLAGS	   = -D VPI_ENDPOINT_ENABLED=1

#Determine whether we're on Cygwin
ifndef OSTYPE   
        # Basically we're interested in finding out if we're
        # in a cygwin environement, so let's find out this way
        # Note: Typically in a bash environment, $OSTYPE is set
        # but sometimes make can't figure it out. So we'll do it
        # this way:
        ifeq (CYGWIN, $(findstring CYGWIN, $(shell uname)))
                OSTYPE=cygwin
        endif
endif

ifeq ($(OSTYPE),cygwin)
	OR_DEBUG_PROXY_SRC = src/or_debug_proxy.c \
			     src/gdb.c \
			     src/usb_functions.c \
			     src/win_usb_driver_calls.c \
			     src/vpi_functions.c	
	CXX                = gcc
	CPPFLAGS           = $(COMMON_CPPFLAGS) -I./obj -DCYGWIN_COMPILE=1 $(DBGCPPFLAGS)
	CXXFLAGS           = $(COMMON_CXXFLAGS)
	DYNAMIC_LDFLAGS    = $(COMMON_LDFLAGS)
	EXE                = .exe
else
	OR_DEBUG_PROXY_SRC = src/or_debug_proxy.c \
			     src/gdb.c
	OR_DEBUG_PROXY_USB_SRC = src/FT2232c.cpp \
			     src/FT2232cMpsseJtag.cpp \
			     src/usb_functions.c \
			     src/linux_usb_driver_calls.c
	OR_DEBUG_PROXY_VPI_SRC = src/vpi_functions.c
	CXX                = g++
	CPPFLAGS           = $(COMMON_CPPFLAGS) $(DBGCPPFLAGS)
	CXXFLAGS           = $(COMMON_CXXFLAGS)
	DYNAMIC_LDFLAGS    = $(COMMON_LDFLAGS) -lftd2xx
	EXE                =
endif

APP             = or_debug_proxy
APP_DYNAMIC = $(APP)$(EXE)
APP_STATIC  = $(APP)_static$(EXE)
APP_VPI     = $(APP)_vpi$(EXE)

# -----------------------------------------------------------------------------
# Build dynamic and static targets from scratch
.PHONY: all
all: clean $(APP_DYNAMIC)

static: clean $(APP_STATIC)

vpi: clean $(APP_VPI)

# Dynamic target
$(APP_DYNAMIC): $(OR_DEBUG_PROXY_SRC) $(OR_DEBUG_PROXY_USB_SRC)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) $(USB_FLAGS) $+ $(DYNAMIC_LDFLAGS) -o $@

# Static target
$(APP_STATIC): $(OR_DEBUG_PROXY_SRC) $(OR_DEBUG_PROXY_USB_SRC) $(STATIC_LIBDIR)/$(STATIC_LIB)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) $(USB_FLAGS) $+ $(STATIC_LDFLAGS) -o $@
	cp $(APP_STATIC) $(APP)

# VPI target only
$(APP_VPI): $(OR_DEBUG_PROXY_SRC) $(OR_DEBUG_PROXY_VPI_SRC)
	$(CXX) $(CPPFLAGS) $(VPI_FLAGS) $(CXXFLAGS) $+ -o $@

# -----------------------------------------------------------------------------
# Target for checking the static lib is in the place it should be
$(STATIC_LIBDIR)/$(STATIC_LIB):
	@echo
	@echo "The static library, $(STATIC_LIB), is missing."
	@echo "Please download the driver to build a statically linked version"
	@echo "of this app. For Linux, try:"
	@echo "\twget http://www.ftdichip.com/Drivers/D2XX/Linux/libftd2xx0.4.16.tar.gz"
	@echo "\ttar xzf libftd2xx0.4.16.tar.gz"
	@echo "\tcp libftd2xx0.4.16/static_lib/libftd2xx.a.0.4.16 lib"
	@echo
	@exit 1
# -----------------------------------------------------------------------------
# Tidy up
clean:
	$(RM) $(APP_DYNAMIC)
	$(RM) $(APP_STATIC)
	$(RM) $(APP_VPI)
	find ./ -name "*~" | xargs $(RM)

ifneq ($(OSTYPE),cygwin)
reset_driver:
	@echo; echo "    Re-installing FTDI serial IO drivers"echo
	@echo "    This will require root privileges."
	sudo modprobe -r ftdi_sio
	sudo modprobe ftdi_sio vendor=0x0403 product=0x6010
	@echo
endif
