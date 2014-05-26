Summary: Unprivileged Package Manager
Name: UserPKG

Version: 20140525
Release: 2%{dist}
License: GPLv2

Group: System Environment/Base
URL: http://www.dracolinux.org

Packager: Ole Andre Rodlie, <olear@dracolinux.org>
Vendor: DracoLinux, http://dracolinux.org

Source: %{name}-%{version}.tar.gz
BuildRoot: %{_tmppath}/%{name}-%{version}-%{release}-root

BuildRequires: qt-devel
Requires: qt DracoPKG

%description
Unprivileged Package Manager

%prep
%setup

%build
qmake-qt4 CONFIG-=debug CONFIG+=release PREFIX=/usr LIBDIR=%{_libdir}
make

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
