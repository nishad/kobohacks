#
# FreeType 2 template for Unix-specific compiler definitions
#

# Copyright 1996-2000, 2002, 2003, 2005, 2006 by
# David Turner, Robert Wilhelm, and Werner Lemberg.
#
# This file is part of the FreeType project, and may only be used, modified,
# and distributed under the terms of the FreeType project license,
# LICENSE.TXT.  By continuing to use, modify, or distribute this file you
# indicate that you have read the license and understand and accept it
# fully.


#CC           := arm-linux-gnueabihf-gcc
CC           := clang -target arm-linux-gnueabihf
COMPILER_SEP := $(SEP)
FT_LIBTOOL_DIR ?= $(BUILD_DIR)

LIBTOOL := $(FT_LIBTOOL_DIR)/libtool


# The object file extension (for standard and static libraries).  This can be
# .o, .tco, .obj, etc., depending on the platform.
#
O  := lo
SO := o


# The executable file extension.  Although most Unix platforms use no
# extension, we copy the extension detected by autoconf.  Useful for cross
# building on Unix systems for non-Unix systems.
#
E := 


# The library file extension (for standard and static libraries).  This can
# be .a, .lib, etc., depending on the platform.
#
A  := la
SA := a


# The name of the final library file.  Note that the DOS-specific Makefile
# uses a shorter (8.3) name.
#
LIBRARY := lib$(PROJECT)


# Path inclusion flag.  Some compilers use a different flag than `-I' to
# specify an additional include path.  Examples are `/i=' or `-J'.
#
I := -I


# C flag used to define a macro before the compilation of a given source
# object.  Usually it is `-D' like in `-DDEBUG'.
#
D := -D


# The link flag used to specify a given library file on link.  Note that
# this is only used to compile the demo programs, not the library itself.
#
L := -l


# Target flag.
#
T := -o$(space)


# C flags
#
#   These should concern: debug output, optimization & warnings.
#
#   Use the ANSIFLAGS variable to define the compiler flags used to enfore
#   ANSI compliance.
#
#   We use our own FreeType configuration file.
#
CPPFLAGS := 
CFLAGS   := -mthumb -march=armv7-a -mtune=cortex-a8 -mfpu=neon -O3 -c -Wall -I/opt/local/include -DFT_CONFIG_OPTION_SYSTEM_ZLIB -I/opt/local/include/libpng16 -DFT_CONFIG_OPTION_USE_PNG -DFT_CONFIG_CONFIG_H="<ftconfig.h>" -I/opt/gcc-linaro-arm-linux-gnueabihf-4.8-2013.04-20130417_linux/arm-linux-gnueabihf/libc/usr/include/arm-linux-gnueabihf

# ANSIFLAGS: Put there the flags used to make your compiler ANSI-compliant.
#
ANSIFLAGS :=  -pedantic -ansi

# C compiler to use -- we use libtool!
#
#
CCraw := $(CC)
CC    := $(LIBTOOL) --mode=compile $(CCraw)

# Linker flags.
#
LDFLAGS := -L/opt/local/lib -lz -L/opt/local/lib -lpng16


# export symbols
#
CCraw_build  := gcc	# native CC of building system
E_BUILD      := 	# extension for exexutable on building system
EXPORTS_LIST := $(OBJ_DIR)/ftexport.sym
CCexe        := $(CCraw_build)	# used to compile `apinames' only


# Library linking
#
LINK_LIBRARY = $(LIBTOOL) --mode=link $(CCraw) -o $@ $(OBJECTS_LIST) \
                          -rpath $(libdir) -version-info $(version_info) \
                          $(LDFLAGS) -no-undefined \
                          # -export-symbols $(EXPORTS_LIST)

# EOF
