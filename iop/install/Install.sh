#! /bin/sh
#
#  IOP include & lib install script
#
#      Copyright (C) 2001 Sony Computer Entertainment Inc.
#                        All Rights Reserved.
#
#       Version        Date            Design      Log
#  --------------------------------------------------------------------
#       1.00           2001/02/07      isii
#       1.01           2001/09/03      isii        support iopfixup.. link

if [ x$SHDEBUG = xarg ]; then
  set -x
fi

# /usr/local/sce
STDSCE=$1

# /usr/local/sce/iop
STDIOP=$2

# /usr/local/sce/iop/install
SRC=$3

# /usr/local/sce/iop/gcc/mipsel-scei-elfl
DIST=$4

# /usr/local/sce/iop/gcc/lib/gcc-lib/mipsel-scei-elfl
ALTDIST=$5

if [ x$STDSCE = x ]; then
  STDSCE=/usr/local/sce
fi
if [ x$STDIOP = x ]; then
  STDIOP=$STDSCE/iop
fi
if [ x$SRC = x ]; then
  SRC=$STDIOP/install
fi
if [ x$DIST = x ]; then
  DIST=$STDIOP/gcc/mipsel-scei-elfl
fi
if [ x$ALTDIST = x ]; then
  ALTDIST=$STDIOP/gcc/lib/gcc-lib/mipsel-scei-elfl
fi

if [ x$SHDEBUG = xarg ]; then
   set +x
   echo "STDSCE =$STDSCE"
   echo "STDIOP =$STDIOP"
   echo "SRC    =$SRC"
   echo "DIST   =$DIST"
   echo "ALTDIST=$ALTDIST"
   exit 1
fi

DATE=`date +%Y_%m_%d_%H%M%S`

# check
if [ x$SHDEBUG = xcheck ]; then
   set -x
fi
if [ ! -d $DIST/include ]; then
  echo "NOT found !!! $DIST/include"
  exit 1
fi
if [ ! -d $DIST/lib ]; then
  echo "NOT found !!! $DIST/lib"
  exit 1
fi
for ifile in iopfixup ioplibdump ioplibld ioplibgen
do
  if [ -f $STDSCE/bin/$ifile ]; then
     if [ ! -d $DIST/bin ]; then
         echo "NOT found !!! $DIST/bin"
         exit 1
     fi
  fi
done
if [ -f $STDSCE/bin/iopspecs ]; then
    if [ ! -d $ALTDIST ]; then
        echo "NOT found !!! $ALTDIST"
        exit 1
    fi
fi
if [ ! -d $SRC/include ]; then
  echo "NOT found !!! $SRC/include"
  exit 1
fi
if [ ! -d $SRC/lib ]; then
  echo "NOT found !!! $SRC/lib"
  exit 1
fi
if [ x$SHDEBUG = xcheck ]; then
   echo "check pass"
   exit 1
fi

# clean 
if [ x$SHDEBUG = xclean ]; then
  set -x
fi

# clean $STDIOP (/usr/local/sce/iop) include and lib
if [ -h $STDIOP/include ]; then
   if rm $STDIOP/include ; then
     echo " remove link $STDIOP/include"
   else
     echo "abort !" ; exit 1
   fi
fi
if [ -d $STDIOP/include ]; then
   if mv $STDIOP/include $STDIOP/include-$DATE; then
     echo " move files $STDIOP/include to $STDIOP/include-$DATE"
   else
     echo "abort !" ; exit 1
   fi
fi

if [ -h $STDIOP/lib ]; then
   if rm $STDIOP/lib; then
     echo " remove link $STDIOP/lib"
   else
     echo "abort !" ; exit 1
   fi
fi
if [ -d $STDIOP/lib ]; then
   if mv $STDIOP/lib $STDIOP/lib-$DATE; then
     echo " move files $STDIOP/lib to $STDIOP/lib-$DATE"
   else
     echo "abort !" ; exit 1
   fi
fi

# clean $DIST ($STDIOP/gcc/mipsel-scei-elfl) include and lib
if [ -f $DIST/lib/iop.ilb ]; then
    if mkdir $DIST/lib-$DATE; then
        if mv $DIST/lib/* $DIST/lib-$DATE; then
            if [ -d $DIST/lib-$DATE/ldscripts ]; then
                if mv $DIST/lib-$DATE/ldscripts $DIST/lib; then
                    :
                else
                    echo "abort !" ; exit 1
                fi
            fi
            if rm -r $DIST/lib-$DATE; then
                echo " remove files $DIST/lib/*"
            else
                echo "abort !" ; exit 1
            fi
        else
            echo "abort !" ; exit 1
	fi
    else
        echo "abort !" ; exit 1
    fi
fi
if [ -f $DIST/include/kernel.h ]; then
    if mkdir $DIST/include-$DATE; then
        if mv $DIST/include/* $DIST/include-$DATE; then
            if rm -r $DIST/include-$DATE; then
                echo " remove files $DIST/include/*"
            else
                echo "abort !" ; exit 1
            fi
        else
            echo "abort !" ; exit 1
        fi
    else
        echo "abort !" ; exit 1
    fi
fi

# clean $DIST ($STDIOP/gcc/mipsel-scei-elfl) bin
for ifile in iopfixup ioplibdump ioplibld ioplibgen
do
  if [ -f $STDSCE/bin/$ifile ]; then
      if [ -L $DIST/bin/$ifile ]; then
         echo " remove link $DIST/bin/$ifile"
         rm  $DIST/bin/$ifile 
      elif [ -f $DIST/bin/$ifile ]; then
         echo " move file $DIST/bin/$ifile to $DIST/bin/$ifile-$DATE"
         mv  $DIST/bin/$ifile  $DIST/bin/$ifile-$DATE
      fi
  fi
done

# clean iopspecs
if [ -f $STDSCE/bin/iopspecs ]; then
    if [ -L $ALTDIST/specs ]; then
       echo " remove link $ALTDIST/specs"
       rm $ALTDIST/specs
    elif [ -f $ALTDIST/specs ]; then
       echo " move file $ALTDIST/specs to $ALTDIST/specs-$DATE"
       mv $ALTDIST/specs $ALTDIST/specs-$DATE
    fi
fi

# clean all end
if [ x$SHDEBUG = xclean ]; then
   echo "clean end"
   exit 1
fi


# copy
if [ x$SHDEBUG = xcopy ]; then
   set -x
fi
echo " copy files from '$SRC/include' to '$DIST/include'"
( cd $SRC/include; tar cf - . ) | ( cd $DIST/include; tar xf - )
echo " copy files from '$SRC/lib' to '$DIST/lib'"
( cd $SRC/lib; tar cf - . ) | ( cd $DIST/lib; tar xf - )
if [ x$SHDEBUG = xcopy ]; then
   set +x
   echo "copy end"
   exit 1
fi

# link
if [ x$SHDEBUG = xlink ]; then
   set -x
fi
echo " link ./gcc/mipsel-scei-elfl/lib to $STDIOP/lib"
ln -s  ./gcc/mipsel-scei-elfl/lib      $STDIOP/lib
echo " link ./gcc/mipsel-scei-elfl/include to $STDIOP/include"
ln -s  ./gcc/mipsel-scei-elfl/include  $STDIOP/include

if [ -f $STDSCE/bin/iopfixup-elf ]; then
    echo " link ../../../../bin/iopfixup-elf  to $DIST/bin/iopfixup"
    ln -s ../../../../bin/iopfixup-elf $DIST/bin/iopfixup
elif  [ -f $STDSCE/bin/iopfixup ]; then
    echo " link ../../../../bin/iopfixup  to $DIST/bin/iopfixup"
    ln -s ../../../../bin/iopfixup $DIST/bin/iopfixup
fi

if [ -f $STDSCE/bin/ioplibld ]; then
   echo " link ../../../../bin/ioplibld  to $DIST/bin/ioplibld"
   ln -s ../../../../bin/ioplibld $DIST/bin/ioplibld
fi

if [ -f $STDSCE/bin/iopspecs ]; then
    echo " link ../../../../../bin/iopspecs to $ALTDIST/specs"
    ln -s ../../../../../bin/iopspecs $ALTDIST/specs
fi

if [ x$SHDEBUG = xlink ]; then
   set +x
   echo "link end"
   exit 1
fi
