#!/bin/sh
#
# $Id: autogen.sh,v 1.2 2004/08/20 21:22:34 bazsi Exp $
#
# This script is needed to setup build environment from checked out
# source tree. 
#

set -e

(
 if [ ! -e "$pemodrepo" ]
 then
     old_pwd=$PWD
     cd "../syslog-ng-pe-modules"
     pemodrepo=$PWD
     cd $old_pwd
     echo "PE module: $pemodrepo"
 fi

 if [ ! -e "$pemodrepo" ]
 then
     echo "Unable to locate syslog-ng-pe-modules under $ZWA_ROOT/work :-(" >&2
     exit 1
 fi

 for pe_module_path in `ls -d $pemodrepo/modules/*/`; do
    pe_module_basename=`basename $pe_module_path`
    if [ -d $pe_module_path ]; then
        if [ -h modules/$pe_module_basename ] || [ -d modules/$pe_module_basename ]; then rm -rf modules/$pe_module_basename; fi
        echo "Creating symlink for module $pe_module_basename..."
        ln -s $pe_module_path modules/$pe_module_basename
    fi
 done

 petests_orig="$pemodrepo/tests"
 petests="pe-tests"
 if [ -d $petests_orig ]; then
     if [ -h $petests ] || [ -d $petests ]; then rm -rf $petests; fi
     ln -s $petests_orig $petests
 fi

 for pebin in windows-tools windows-binaries; do
    binpath=$pemodrepo/$pebin
    if [ -d $binpath ]; then
        if [ -h $pebin ] || [ -d $pebin ]; then rm -rf $pebin; fi
        ln -s $binpath $pebin
    fi
 done
)

ACLOCALPATHS=
for pth in /opt/libtool/share/aclocal /usr/local/share/aclocal; do
	if [ -d $pth ];then
		ACLOCAPATHS="$ACLOCALPATHS -I $pth"
	fi
done
# bootstrap syslog-ng itself
echo "LIBTOOLIZE"
libtoolize --force --copy
echo "ACLOCAL"
aclocal -I m4 $ACLOCALPATHS --install -Wnone
sed -i -e 's/PKG_PROG_PKG_CONFIG(\[0\.16\])/PKG_PROG_PKG_CONFIG([0.14])/g' aclocal.m4
echo "AUTOHEADER"
autoheader
echo "AUTOMAKE"
automake --foreign --add-missing --copy -Wnone
echo "AUTOCONFIG"
autoconf
find -name libtool -o -name ltmain.sh | xargs sed -i -e "s,'file format pe-i386.*\?','file format \(pei\*-i386\(\.\*architecture: i386\)\?|pe-arm-wince|pe-x86-64\)',"
sed -i -e "s, cmd //c, sh -c," ltmain.sh
