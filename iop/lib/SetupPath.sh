#! /bin/sh -f

OBJTYPE=ELF
#OBJTYPE=COFF

CHECKPATH=".:/usr/local/bin/:/usr/local/iop/bin/:/usr/local/sce/iop/bin:/usr/local/sce/bin/:/g/tools/bin/"

if [ "$OBJTYPE" = "ELF" ]; then
    #for elf compiler
    CHECKEXEGNU="iop-elf-:mipsel-scei-elfl-"
else
    #for ecoff compiler
    CHECKEXEGNU="iop-coff-:mipsel-scei-ecoffl-"
fi

search_gcc() {
    OLDIFS=$IFS
    IFS=":"
    for dir in $CHECKPATH
    do
        if [ $dir = "." ]; then
           for cmd in $CHECKEXEGNU
           do
	       echo "  check ${cmd}gcc" 1>&2
               if which ${cmd}gcc > /dev/null; then
	           echo "  found ${cmd}gcc" 1>&2
                   echo $cmd
                   exit 0
               fi
	   done
        else
           for cmd in $CHECKEXEGNU
           do
               echo "  check $dir${cmd}gcc" 1>&2
	       if [ -x $dir${cmd}gcc ]; then
                   echo "  found $dir${cmd}gcc" 1>&2
	           echo $dir$cmd
                   exit 0
               fi
           done
        fi
    done
    IFS=$OLDIFS
    exit 1
}

search_sce_tool() {
    dir=$1
    cmd=$2
    macro=$3
    if which ${cmd} > /dev/null; then
       echo "${macro}  = ${cmd}"
    else
        if [ -x ${dir}${cmd} ]; then
            echo "${macro}  = ${dir}${cmd}"
        else
            echo "  not found command \`${cmd}'" 1>&2
	    exit 1
        fi
    fi
}

result=`search_gcc`
if [ $? != 0 ]; then
   exit 1
fi

RESULTDIR=`dirname $result`
RESULTCMD=`basename $result`
if [ $RESULTDIR = "." ]; then
    RESULTDIR=
else
    RESULTDIR=$RESULTDIR/
fi

echo "AR      = ${RESULTDIR}${RESULTCMD}ar"
echo "AS      = ${RESULTDIR}${RESULTCMD}as"
echo "CC      = ${RESULTDIR}${RESULTCMD}gcc"
echo "GCC     = ${RESULTDIR}${RESULTCMD}gcc"
echo "LD      = ${RESULTDIR}${RESULTCMD}ld"
echo "NM      = ${RESULTDIR}${RESULTCMD}nm"
echo "SIZE    = ${RESULTDIR}${RESULTCMD}size"
echo "STRIP   = ${RESULTDIR}${RESULTCMD}strip"
echo "RANLIB  = ${RESULTDIR}${RESULTCMD}ranlib"
echo "OBJCOPY = ${RESULTDIR}${RESULTCMD}objcopy"
echo "OBJDUMP = ${RESULTDIR}${RESULTCMD}objdump"
search_sce_tool "${RESULTDIR}" iopfixup      "IFIXUP "
search_sce_tool "${RESULTDIR}" ioplibgen     "ILBGEN "
search_sce_tool "${RESULTDIR}" ioplibld      "ILBLD  "
search_sce_tool "${RESULTDIR}" ioplibdump    "ILBDUMP"
if [ "$OBJTYPE" = "ELF" ]; then
    search_sce_tool "${RESULTDIR}" bin2elf   "BIN2OBJ"
else
    search_sce_tool "${RESULTDIR}" bin2ecoff "BIN2OBJ"
fi
