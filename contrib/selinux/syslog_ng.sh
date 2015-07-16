#!/bin/sh

set -e
DIRNAME=`dirname $0`
cd $DIRNAME
USAGE="$0"
if [ `id -u` != 0 ]; then
echo 'You must be root to run this script'
exit 1
fi

if [ $# -ne 0 ]; then
	echo -e $USAGE
	exit 1
fi

set +e
echo "Building and Loading Policy"
set -x
make -f /usr/share/selinux/devel/Makefile syslog_ng.pp || exit
/usr/sbin/semodule -i syslog_ng.pp -v

# syslog() source uses port 601 (RFC 3195)
semanage port -a -t syslogd_port_t -p tcp 601

# Fixing the file context
/sbin/restorecon -F -R -v /opt/syslog-ng/bin
/sbin/restorecon -F -R -v /opt/syslog-ng/etc
/sbin/restorecon -F -R -v /opt/syslog-ng/lib
/sbin/restorecon -F -R -v /opt/syslog-ng/var
/sbin/restorecon -F -R -v /opt/syslog-ng/libexec
/sbin/restorecon -F -R -v /opt/syslog-ng/libexec/syslog-ng-wrapper.sh
/sbin/restorecon -F -R -v /opt/syslog-ng/libexec/syslog-ng
/sbin/restorecon -F -R -v /opt/syslog-ng/sbin
/sbin/restorecon -F -R -v /opt/syslog-ng/var/run
/sbin/restorecon -F -R -v /etc/init.d/syslog-ng
/sbin/restorecon -F -R -v /etc/rc.d/init.d/syslog-ng
/sbin/restorecon -F -R -v /dev/log
/sbin/restorecon -F -R -v /opt/syslog-ng/share/include/scl/system/generate-system-source.sh
/sbin/restorecon -F -R -v /opt/syslog-ng/share/man

set +x

echo "Please restart syslog-ng. You can find more information about this in the README file."
