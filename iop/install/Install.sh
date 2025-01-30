#! /bin/sh
#
#  IOP include & lib install script
#
#      Copyright (C) 2001,2002 Sony Computer Entertainment Inc.
#                        All Rights Reserved.
#
#       Version        Date            Design      Log
#  --------------------------------------------------------------------
#       1.00           2001/02/07      isii
#       1.01           2001/09/03      isii        support iopfixup.. link
#       1.02           2002/07/29      isii        support iop-gcc muti-version
#       1.03           2003/01/20      isii        header&lib files link
#       1.04           2003/06/20      isii        --cleanonly option

# usage:
#   Install.sh [--cleanonly] [ /usr/local/sce [ /usr/local/sce/iop [/usr/local/sce/iop/install] ] ]
#

if [ x$SHDEBUG = xarg ]; then
  set -x
fi

if [ x$1 = "x--cleanonly" ]; then
    CLEANONLY="y"
    shift
else
    CLEANONLY="n"
fi

# /usr/local/sce
STDSCE=$1

# /usr/local/sce/iop
STDIOP=$2

# /usr/local/sce/iop/install
SRC=$3

if [ x$STDSCE = x ]; then
  STDSCE=/usr/local/sce
fi
if [ x$STDIOP = x ]; then
  STDIOP=$STDSCE/iop
fi
if [ x$SRC = x ]; then
  SRC=$STDIOP/install
fi

for i in $STDIOP/gcc $STDIOP/gcc-?.?.?
do
    if test -d $i -a ! -h $i ; then
        GCCLIST="$GCCLIST "`basename $i`
        GCCNEWEST=`basename $i`
    fi
done

if [ x$GCCNEWEST = x ];then
    GCCCMDS=
else
    GCCCMDS=`( cd $STDIOP/$GCCNEWEST/bin ; echo iop-* | sed -e 's/iop-elf-[^ ]*//g' -e 's/iop-coff-[^ ]*//g' -e 's/iop-strip//g' -e 's/iop-[*]//' )`
fi

if [ x$SHDEBUG = xarg ]; then
    set +x
    echo "STDSCE =$STDSCE"
    echo "STDIOP =$STDIOP"
    echo "SRC    =$SRC"
    echo "GCCLIST=$GCCLIST"
    echo "GCCCMDS=$GCCCMDS"
    exit 1
fi

DATE=`date +%Y_%m_%d_%H%M%S`

# check
if [ x$SHDEBUG = xcheck ]; then
    set -x
fi
for GCCDIR in $GCCLIST
do
    DIST=$STDIOP/$GCCDIR/mipsel-scei-elfl
    ALTDIST=$STDIOP/$GCCDIR/lib/gcc-lib/mipsel-scei-elfl

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
done

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

if [ -x $STDIOP/install/iop-xxx.sh ]; then
    for i in $GCCCMDS
    do
        if [ -h $STDSCE/bin/$i -o -f $STDSCE/bin/$i ]; then
            if rm $STDSCE/bin/$i ; then
                echo " remove file $STDIOP/bin/$i"
            else
                echo "abort !" ; exit 1
            fi
        fi
    done
fi

# clean $DIST ($STDIOP/gcc/mipsel-scei-elfl) include and lib
for GCCDIR in $GCCLIST
do
  DIST=$STDIOP/$GCCDIR/mipsel-scei-elfl
  ALTDIST=$STDIOP/$GCCDIR/lib/gcc-lib/mipsel-scei-elfl
  if [ -f $DIST/lib/iop.ilb -o -h $DIST/lib/iop.ilb ]; then
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
  if [ -f $DIST/include/kernel.h -o -h $DIST/include/kernel.h ]; then
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
  for ifile in iopfixup ioplibdump ioplibld ioplibgen iopcdtorsgen
  do
    if [ -f $STDSCE/bin/$ifile ]; then
        if [ -h $DIST/bin/$ifile ]; then
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
      if [ -h $ALTDIST/specs ]; then
         echo " remove link $ALTDIST/specs"
         rm $ALTDIST/specs
      elif [ -f $ALTDIST/specs ]; then
         echo " move file $ALTDIST/specs to $ALTDIST/specs-$DATE"
         mv $ALTDIST/specs $ALTDIST/specs-$DATE
      fi
  fi
done

# clean all end
if [ x$SHDEBUG = xclean ]; then
    echo "clean end"
    exit 1
fi

# if --cleanonly option
if [ $CLEANONLY = "y" ] ;then
    exit 0
fi

# link or copy
if [ x$SHDEBUG = xcopy ]; then
    set -x
fi
for GCCDIR in $GCCLIST
do
    DIST=$STDIOP/$GCCDIR/mipsel-scei-elfl
    ALTDIST=$STDIOP/$GCCDIR/lib/gcc-lib/mipsel-scei-elfl
    if [ -x $STDIOP/$GCCDIR/bin/iop-gcc ]; then
        echo " link files from '$STDIOP/include' to '$DIST/include'"
        for i in `( cd $SRC/include; echo * )`
        do
            ln -s $STDIOP/include/$i $DIST/include/$i
        done
        echo " link files from '$STDIOP/lib' to '$DIST/lib'"
        for i in `( cd $SRC/lib; echo * )`
        do
            ln -s $STDIOP/lib/$i $DIST/lib/$i
        done
    fi
    if [ -x $STDIOP/$GCCDIR/bin/gcov ]; then
        if [ -h $STDSCE/bin/gcov ]; then
            rm -f $STDSCE/bin/gcov
        fi
        if [ ! -f $STDSCE/bin/gcov ]; then
            echo " link $STDIOP/$GCCDIR/bin/gcov to $STDSCE/bin/gcov"
            ln -s $STDIOP/$GCCDIR/bin/gcov $STDSCE/bin/gcov
        fi
    fi
done

if [ -x $STDIOP/install/iop-xxx.sh ]; then
    for i in $GCCCMDS
    do
        echo " copy $STDIOP/install/iop-xxx.sh to $STDSCE/bin/$i"
        cp -p $STDIOP/install/iop-xxx.sh $STDSCE/bin/$i
    done
fi

if [ x$SHDEBUG = xcopy ]; then
    set +x
    echo "copy end"
    exit 1
fi

# link
if [ x$SHDEBUG = xlink ]; then
    set -x
fi

echo " link $STDIOP/install/lib to $STDIOP/lib"
ln -s  $STDIOP/install/lib    $STDIOP/lib
echo " link $STDIOP/install/include to $STDIOP/include"
ln -s  $STDIOP/install/include  $STDIOP/include

for GCCDIR in $GCCLIST
do
    DIST=$STDIOP/$GCCDIR/mipsel-scei-elfl
    ALTDIST=$STDIOP/$GCCDIR/lib/gcc-lib/mipsel-scei-elfl
    if [ -x $STDIOP/$GCCDIR/bin/iop-gcc ]; then
        if [ -f $STDSCE/bin/iopfixup-elf ]; then
            echo " link $STDSCE/bin/iopfixup-elf  to $DIST/bin/iopfixup"
            ln -s $STDSCE/bin/iopfixup-elf $DIST/bin/iopfixup
        elif  [ -f $STDSCE/bin/iopfixup ]; then
            echo " link $STDSCE/bin/iopfixup  to $DIST/bin/iopfixup"
            ln -s $STDSCE/bin/iopfixup $DIST/bin/iopfixup
        fi
  
        if [ -f $STDSCE/bin/ioplibld ]; then
            echo " link $STDSCE/bin/ioplibld  to $DIST/bin/ioplibld"
            ln -s $STDSCE/bin/ioplibld $DIST/bin/ioplibld
        fi

        if [ -f $STDSCE/bin/iopcdtorsgen ]; then
            echo " link $STDSCE/bin/iopcdtorsgen  to $DIST/bin/iopcdtorsgen"
            ln -s $STDSCE/bin/iopcdtorsgen $DIST/bin/iopcdtorsgen
        fi

        VERTMP=`echo $GCCDIR | sed -e "s/gcc//"`
        TRYLIST=
        while [ x$VERTMP != x ]
        do
            TRYLIST="$TRYLIST iopspecs$VERTMP"
            VERTMP=`echo $VERTMP | sed -e "s/[.-][0-9]*$//"`
        done
        TRYLIST="$TRYLIST iopspecs"
        for TRY in $TRYLIST
        do
            if [ -f $STDSCE/bin/$TRY ]; then
                echo " link $STDSCE/bin/$TRY to $ALTDIST/specs"
                ln -s $STDSCE/bin/$TRY  $ALTDIST/specs
                break
            fi
        done
    fi
done

if [ x$SHDEBUG = xlink ]; then
    set +x
    echo "link end"
    exit 1
fi

exit 0
