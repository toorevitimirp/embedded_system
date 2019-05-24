#!/bin/bash
 ################################################################
 # $ID: config.sh      Thu, 18 Nov 2004 21:24:34 +0800  mhfan $ #
 #                                                              #
 # Description:                                                 #
 #                                                              #
 # Maintainer:  ∑∂√¿ª‘(Meihui Fan)  <mhfan@ustc.edu>            #
 #                                                              #
 # CopyRight (c)  2004  HHTech                                  #
 #   www.hhcn.com, www.hhcn.org                                 #
 #   All rights reserved.                                       #
 #                                                              #
 # This file is free software;                                  #
 #   you are free to modify and/or redistribute it  	        #
 #   under the terms of the GNU General Public Licence (GPL).   #
 #                                                              #
 # Last modified: Thu, 18 Nov 2004 22:16:24 +0800      by mhfan #
 ################################################################

if [ -f Makefile ]; then make distclean; fi

./configure

CROSS=/opt/host/armv4l/bin/armv4l-unknown-linux-

CC=${CROSS}gcc
AR=${CROSS}ar
STRIP=${CROSS}strip

PPPD_INC=../../ppp-2.4.2/include
KERN_INC=/backup/products-cd/HHARM/arm9-edu/HHARM9-EDU/kernel/include

CFLAGS="-D__Linux__ -DDEBUG -g -I$PPPD_INC -I$KERN_INC"
#CFLAGS="$CFLAGS -I$KERN_INC/asm -I$KERN_INC/linux"
#CFLAGS="$CFLAGS -I$KERN_INC/config -I$KERN_INC/net"
LDFLAGS="--static"

export CC AR STRIP CFLAGS LDFLAGS

./configure --prefix=../build # &&
make

unset CC AR STRIP CFLAGS LDFLAGS

 #################### End Of File: config.sh ####################
