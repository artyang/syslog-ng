#!/bin/bash

EL_FC=
EL_TE=
OS_VERSION=
INSTALL_PATH="/opt/syslog-ng"
# Ports 514/udp, 6514/udp and 6514/tcp are allowed by default
SYSLOG_NG_TCP_PORTS="601"
SYSLOG_NG_UDP_PORTS="601"
INPUT=

query_install_path() {
	echo -n "Please enter your installation path for Syslog-ng PE: [${INSTALL_PATH}] "
	read INPUT
}


verify_input() {
	INPUT="${INPUT:-${INSTALL_PATH}}"
	echo -n "You have entered '${INPUT}'. Is this correct? [y/N] "
	read ACCEPT
	if [ "x${ACCEPT}x" != "xyx" ]; then return 0; fi
	if [ ! -d "${INPUT}" ]; then echo "The directory you entered does not exist!" ; return 0 ; else return 1; fi
}


extract_version_string() {
	sed -n 's:^[a-zA-Z ]\+\([0-9]\+\.[0-9]\+\)\(.[0-9]\+\)\?[a-zA-Z ()]\+$:\1:p'
}


detect_os_version() {
	echo "Detecting RHEL version..."
	if [ -x "/usr/bin/lsb_release" ]; then
		if lsb_release -i | grep -qvE "RedHat|CentOS"; then
			echo "You don't seem to be running a supported Linux distribution!" >&2
			exit 1
		fi
		OS_VERSION=$( lsb_release -r | cut -f 2 )
	else
		# The package redhat-lsb-core is most likely not installed...
		if [ -f "/etc/redhat-release" ]; then
			OS_VERSION=$( extract_version_string < "/etc/redhat-release" )
		else
			echo "You don't seem to be running a supported OS!" >&2
			exit 1
		fi
	fi
}


omit_allowed_tcp_ports() {
	sed -e 's:^601::g'
}


omit_allowed_udp_ports() {
	sed -e 's:^601::g'
}


omit_allowed_ports() {
	SYSLOG_NG_TCP_PORTS=$( omit_allowed_tcp_ports <<<"${SYSLOG_NG_TCP_PORTS}" )
	SYSLOG_NG_UDP_PORTS=$( omit_allowed_udp_ports <<<"${SYSLOG_NG_UDP_PORTS}" )
}


setup_vars() {
	echo "Detected RHEL ${OS_VERSION}."
	case "${OS_VERSION}" in
		5.*)
			
			EL_FC="syslog_ng.el5.fc.in"
			EL_TE="syslog_ng.el5.te.in"
			;;
		6.*)
			EL_FC="syslog_ng.el6.fc.in"
			
			local MINORVER 
			MINORVER=$( cut -d. -f 2 <<<"${OS_VERSION}" )
			if [ "${MINORVER}" -lt 5 ]; then
				EL_TE="syslog_ng.el6.0to4.te.in"
			else
				EL_TE="syslog_ng.el67.te.in"
				    
				# 601/tcp and 601/udp are allowed by default on RHEL6.5+, so there is no need to enable them
				omit_allowed_ports
			fi
			;;
		7.*)
			EL_FC="syslog_ng.el7.fc.in"
			EL_TE="syslog_ng.el67.te.in"
			
			# 601/tcp and 601/udp are allowed by default on RHEL7, so there is no need to enable them
			omit_allowed_ports
			;;
		*)
			echo "You don't seem to be running a supported version of RHEL!" >&2
			exit 1
			;;
	esac
}


substitute_install_path() {
	sed -e "s:^\\\$PATH\\\$:${INSTALL_PATH}:g" "src/root_unsafe/${EL_FC}"
	sed -e "s:^\\\$PATH\\\$:${INSTALL_PATH}:g" "src/root_safe/${EL_FC}"
}


omit_install_path() {
	sed -e "s:^\\\$PATH\\\$::g" "src/root_safe/${EL_FC}"
}


prepare_files() {
	echo "Using '${INSTALL_PATH}'..." 
	if [ "${INSTALL_PATH}" != "/" ]; then
		
		substitute_install_path > "syslog_ng.fc"
	else
		omit_install_path > "syslog_ng.fc"
	fi
	cp "src/${EL_TE}" "syslog_ng.te"
}


build_module() {
	echo "Building and Loading Policy"
	make -f /usr/share/selinux/devel/Makefile syslog_ng.pp || exit 1
}


install_module() {
	/usr/sbin/semodule -i syslog_ng.pp -v || exit 1

	# set up syslog-ng specific ports
	for port in ${SYSLOG_NG_TCP_PORTS}; do semanage port -a -t syslogd_port_t -p tcp ${port}; done
	for port in ${SYSLOG_NG_UDP_PORTS}; do semanage port -a -t syslogd_port_t -p udp ${port}; done
	
	# Fixing the file context
	/sbin/restorecon -F -Rv "${INSTALL_PATH}"
	/sbin/restorecon -F -Rv /etc/init.d/syslog-ng
	/sbin/restorecon -F -Rv /etc/rc.d/init.d/syslog-ng
	/sbin/restorecon -F -Rv /dev/log
	
	echo -e "\nPlease restart syslog-ng. You can find more information about this in the README file."
}


remove_module() {
	if semodule -l | grep -q syslog_ng; then
		echo -n "Removing Syslog-ng SELinux policy module... "
		
		semodule --remove=syslog_ng
		
		# unconfigure syslog-ng specific ports
		for port in ${SYSLOG_NG_TCP_PORTS}; do semanage port -d -t syslogd_port_t -p tcp ${port}; done
		for port in ${SYSLOG_NG_UDP_PORTS}; do semanage port -d -t syslogd_port_t -p udp ${port}; done
		
		[ -f syslog_ng.pp ] && rm -f syslog_ng.pp
		[ -f syslog_ng.te ] && rm -f syslog_ng.te
		[ -f syslog_ng.fc ] && rm -f syslog_ng.fc
		[ -f syslog_ng.if ] && rm -f syslog_ng.if
		[ -d tmp ] && rm -Rf tmp
		
		echo "done."
	else
		echo "No installed Syslog-ng SELinux policy module was found. Nothing to do..."
	fi
}


DIRNAME=$( dirname ${0} )
cd ${DIRNAME}
USAGE="$0\t[ --remove | --help ]\n\n$0:\tA tool for building and managing the SELinux policy for the\n\t\tdefault syslog-ng installation."

if [ ${#} -ge 1 ]; then
	case "$1" in
		--remove)
			detect_os_version
			setup_vars
			remove_module
			exit 0
			;;
#		--help)
		*)
			echo -e ${USAGE}
			exit 1
			;;
	esac
fi

if [ $( id -u ) != 0 ]; then
	echo 'You must be root to run this script'
	exit 1
fi

query_install_path
while verify_input; do
	query_install_path
done
INSTALL_PATH="${INPUT}"

detect_os_version
setup_vars
prepare_files
build_module
install_module

