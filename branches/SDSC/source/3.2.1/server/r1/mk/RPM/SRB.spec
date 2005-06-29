%define srbHome /opt/srb
%define srbdevHome /home/srbdev
%define wantPasswd 1
%define srbPasswd ssrrbbeemmee

Summary:	The Storage Resource Broker
Name:		SRB
Version:	3.2.1
Release:	1
Copyright:	Copyright 2004, SDSC/UCSD, All Rights Reserved
Group:		Applications/File
Source:		%{name}-%{version}.tar.gz
URL:		http://www.npaci.edu/DICE/SRB/
Packager:	SDSC Roman Olschanowsky
BuildRoot:	/tmp/SRB-root
%description
The Storage Resource Broker is a distributed file system (Data Grid),
based on a client-server architecture.

It replicates, syncs, archives, and provides a way to access files
and computers based on their attributes rather than just their names
or physical locations.




%package MCAT_Server
#####################################################################
Summary:	The SRB MCAT enabled Server and Client
Group:		Applications/File
%description MCAT_Server
The Storage Resource Broker is a distributed file system (Data Grid),
based on a client-server architecture.

It replicates, syncs, archives, and provides a way to access files
and computers based on their attributes rather than just their names
or physical locations.

This is the MCAT enabled Server portion, there should be only one MCAT
enabled Server running per SRB Zone (data Grid). It usually lives
on the same machine where the MCAT database is housed. It also has all
of the administrative binaries needed to "sys admin" the SRB data grid.

This package also installs the SRB Client.




%package Slave_Server
#####################################################################
Summary:        The SRB Slave Server and Client
Group:          Applications/File
%description Slave_Server
The Storage Resource Broker is a distributed file system (Data Grid),
based on a client-server architecture.

It replicates, syncs, archives, and provides a way to access files
and computers based on their attributes rather than just their names
or physical locations.

This is the Slave Server portion, Slave Servers make up the rest of the
SRB Zone (including the one MCAT enabled Server per Zone needed).

This package also installs the SRB Client.




%package Client
#####################################################################
Summary:        The SRB Client
Group:          Applications/File
%description Client
The Storage Resource Broker is a distributed file system (Data Grid),
based on a client-server architecture.

It replicates, syncs, archives, and provides a way to access files
and computers based on their attributes rather than just their names
or physical locations.

This is the client portion, once a SRB data grid has been setup, users
can begin using the system to ingest and share files with the community.




%prep
#####################################################################
tar xzvf $RPM_SOURCE_DIR/%{name}-%{version}.tar.gz




%build
#####################################################################
cd %{name}-%{version}
./configure
gmake




%install
#####################################################################
cd %{name}-%{version}

# install root files
install -m 755 -D mk/RPM/srb $RPM_BUILD_ROOT/etc/rc.d/init.d/srb
install -m 755 -D mk/RPM/srb.sh $RPM_BUILD_ROOT/etc/profile.d/srb.sh
mkdir -p $RPM_BUILD_ROOT/usr/share/man/man1
mkdir -p $RPM_BUILD_ROOT/usr/local/bin
cp utilities/bin/S* $RPM_BUILD_ROOT/usr/local/bin
cp utilities/man/man1/S* $RPM_BUILD_ROOT/usr/share/man/man1

#install srb files
mkdir -p $RPM_BUILD_ROOT%{srbHome}
cp -r $RPM_BUILD_DIR/%{name}-%{version} $RPM_BUILD_ROOT%{srbHome}
cp -r bin $RPM_BUILD_ROOT%{srbHome}
cp -r COPYRIGHT $RPM_BUILD_ROOT%{srbHome}
cp -r data $RPM_BUILD_ROOT%{srbHome}
cp -r mk/RPM/.srb $RPM_BUILD_ROOT%{srbHome}

#install srbdev files
mkdir -p $RPM_BUILD_ROOT%{srbdevHome}
cp -r mk/RPM/.srbdev $RPM_BUILD_ROOT%{srbdevHome}/.srb




%clean
#####################################################################
rm -rf $RPM_BUILD_DIR/%{name}-%{version}
rm -rf $RPM_BUILD_ROOT




%pre MCAT_Server
#####################################################################
if [ -f /usr/sbin/useradd.real ]; then
    USERADD=/usr/sbin/useradd.real
else
    USERADD=/usr/sbin/useradd
fi

id srb > /dev/null 2>&1
if [ $? = 1 ]
then
    $USERADD -c "SRB Master/Server Account" -d %{srbHome} srb
    if [ %{wantPasswd} = 1 ]
    then
         echo "%{srbPasswd}" | passwd --stdin srb
    fi
fi

id srbdev > /dev/null 2>&1
if [ $? = 1 ]
then
    $USERADD -c "SRB Developer Account" -d %{srbdevHome} srbdev
    if [ %{wantPasswd} = 1 ]
    then
         echo "%{srbPasswd}" | passwd --stdin srbdev
    fi
fi

if [ "$1" = 2 ]
then
    /etc/rc.d/init.d/srb stop
fi




%pre Slave_Server
#####################################################################
if [ -f /usr/sbin/useradd.real ]; then
    USERADD=/usr/sbin/useradd.real
else
    USERADD=/usr/sbin/useradd
fi

id srb > /dev/null 2>&1
if [ $? = 1 ]
then
    $USERADD -c "SRB Master/Server Account" -d %{srbHome} srb
    if [ %{wantPasswd} = 1 ]
    then
         echo "%{srbPasswd}" | passwd --stdin srb
    fi
fi

id srbdev > /dev/null 2>&1
if [ $? = 1 ]
then
    $USERADD -c "SRB Developer Account" -d %{srbdevHome} srbdev
    if [ %{wantPasswd} = 1 ]
    then
         echo "%{srbPasswd}" | passwd --stdin srbdev
    fi
fi

if [ "$1" = 2 ]
then
    /etc/rc.d/init.d/srb stop
fi




%preun MCAT_Server
#####################################################################
if [ "$1" = 0 ]
then
    /etc/rc.d/init.d/srb stop
    /sbin/chkconfig --del srb
    rm -f %{srbHome}/data/srbLog*
fi




%preun Slave_Server
#####################################################################
if [ "$1" = 0 ]
then
    /etc/rc.d/init.d/srb stop
    /sbin/chkconfig --del srb
    rm -f %{srbHome}/data/srbLog*
fi




%post MCAT_Server
#####################################################################
chmod 755 /etc/rc.d/init.d/srb
/sbin/chkconfig --add srb
/etc/rc.d/init.d/srb start




%post Slave_Server
#####################################################################
chmod 755 /etc/rc.d/init.d/srb
/sbin/chkconfig --add srb
/etc/rc.d/init.d/srb start




%files MCAT_Server
#####################################################################
/etc/rc.d/init.d/srb
/etc/profile.d/srb.sh
/usr/local/bin
/usr/share/man/man1
%defattr(-,srb,srb)
%{srbHome}/bin
%{srbHome}/COPYRIGHT
%{srbHome}/%{name}-%{version}
%config %{srbHome}/data
%config %{srbHome}/.srb
%defattr(-,srbdev,srbdev)
%config %{srbdevHome}/.srb




%files Slave_Server
#####################################################################
/etc/rc.d/init.d/srb
/etc/profile.d/srb.sh
/usr/local/bin
/usr/share/man/man1
%defattr(-,srb,srb)
%{srbHome}/bin
%{srbHome}/COPYRIGHT
%config %{srbHome}/data
%config %{srbHome}/.srb
%defattr(-,srbdev,srbdev)
%config %{srbdevHome}/.srb




%files Client
#####################################################################
/usr/local/bin
/usr/share/man/man1

