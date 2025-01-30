#! /bin/sh
#
#  IOP binutils/gcc selecter
#
#      Copyright (C) 2002 Sony Computer Entertainment Inc.
#                        All Rights Reserved.
#
#       Version        Date            Design      Log
#  --------------------------------------------------------------------
#       1.00           2002/07/29      isii

PREFIX=/usr/local/sce/iop
CMD=`basename $0`
GCCBASEDIR=
GCCTRY=anywhere
if test x$IOPGCC_VERSION != x ; then
  GCCTRY=$PREFIX/gcc-$IOPGCC_VERSION/bin
  if test -d $GCCTRY -a -x $GCCTRY/$CMD ; then
    GCCBASEDIR=gcc-$IOPGCC_VERSION
  else
     GCCTRY=$PREFIX/$IOPGCC_VERSION/bin
     if test -d $GCCTRY -a -x $GCCTRY/$CMD ; then
        GCCBASEDIR=$IOPGCC_VERSION
     fi
  fi
else
  for i in $PREFIX/gcc $PREFIX/gcc-?.?.?
  do
    if test -d $i -a ! -L $i ; then
       GCCTRY=$PREFIX/`basename $i`/bin
       if test -x $GCCTRY/$CMD ; then
          GCCBASEDIR=`basename $i`
       fi
    fi
  done
fi

if test x$GCCBASEDIR != x ; then
  exec $PREFIX/$GCCBASEDIR/bin/$CMD "$@"
else
  echo Not found '"'$CMD'"' in $GCCTRY
  exit 1
fi

