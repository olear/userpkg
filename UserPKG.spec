Summary: Unprivileged Package Manager
Name: UserPKG

Version: 1.0alpha14
Release: 1%{dist}
License: GPLv2

Group: System Environment/Base
URL: http://userpkg.dracolinux.org

Packager: Ole-André Rodlie, <olear@dracolinux.org>
Vendor: DracoLinux, http://dracolinux.org

Source: %{name}-%{version}.tar.gz
BuildRoot: %{_tmppath}/%{name}-%{version}-%{release}-root

BuildRequires: qt-devel
Requires: qt gcc-c++ make glibc-devel ncurses-devel

%description
Unprivileged Package Manager

%prep
%setup

%build
cd DracoPKG/lib
qmake-qt4 CONFIG+=release CONFIG+=libpkgsrc CONFIG+=staticlib PREFIX=$(pwd)/../build
make install
cd ../..
qmake-qt4 CONFIG+=release CONFIG+=libpkgsrc PREFIX=/usr

%install
make INSTALL_ROOT=%{buildroot} install

%clean
%{__rm} -rf %{buildroot}

%files
%defattr(-,root,root,-)
%{_bindir}/%{name}
%{_docdir}/*
/usr/share/applications/%{name}.desktop
/usr/share/icons/*

%changelog
* Thu Apr 29 2014 Ole Andre Rodlie <olear@dracolinux.org> - 0.1-1
- initial version
