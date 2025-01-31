#######################################################################
# Copyright (c) 2017, Jim Sukha
#
# Use of this source code is governed by a BSD-style license that can
# be found in the LICENSE file.
#######################################################################

#######################################################################
#  Copyright (C) 2012-15 Intel Corporation
#  All rights reserved.
#
#  Redistribution and use in source and binary forms, with or without
#  modification, are permitted provided that the following conditions
#  are met:
#
#  *  Redistributions of source code must retain the above copyright
#     notice, this list of conditions and the following disclaimer.
#  *  Redistributions in binary form must reproduce the above copyright
#     notice, this list of conditions and the following disclaimer in
#     the documentation and/or other materials provided with the
#     distribution.
#  *  Neither the name of Intel Corporation nor the names of its
#     contributors may be used to endorse or promote products derived
#     from this software without specific prior written permission.
#
#  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
#  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
#  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
#  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
#  HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
#  INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
#  BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
#  OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
#  AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
#  LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY
#  WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
#  POSSIBILITY OF SUCH DAMAGE.
#######################################################################

# Sets various flags and variables in Makefile, depending on the
# platform that we are running on.

# Expects $(CILKPUB_DIR) and $(TMP_FOLDER) to be defined.

# OpenCilk build
CC=clang
CXX=clang++
LDFLAGS += -fopencilk
CXXFLAGS += -fopencilk

INCLUDE_FLAGS+= -I$(CILKPUB_DIR)
CFLAGS = -Wall
CXXFLAGS+= -Wall

ifeq ($(DEBUG), 1)
   CFLAGS += -g -O0
   CXXFLAGS += -g -O0
else
   CFLAGS += -g -O3
   CXXFLAGS += -g -O3
endif

MKDIR_CMD=mkdir -p
RM_CMD=rm -rf
# Separator for multiple commands.
AND_SEP=;
# Separator for subdirectories.  Watch out for extra spaces.
SLASH=/

CILKPUB_OS="OpenCilk"
