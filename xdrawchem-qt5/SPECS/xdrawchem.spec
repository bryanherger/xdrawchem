Summary: XDrawChem is an application for drawing and analyzing chemical structures and reactions.
Name: xdrawchem
Version: 1.9.11
Release: 1
License: GPL
Group: Applications/Productivity
URL: http://www.woodsidelabs.com/chemistry/xdrawchem.php
Source0: https://sourceforge.net/projects/%{name}/files/%{name}/%{version}/%{name}-%{version}-%{release}.rpmsrc.tar.gz
Requires: openbabel qt5-qtbase
BuildRequires: openbabel openbabel-devel qt5-qtbase-devel
BuildRoot: /var/tmp/%{name}-buildroot

%description
XDrawChem is an application for drawing and analyzing chemical structures and reactions.

%prep
%setup -q

%build
[ "$RPM_BUILD_ROOT" != "/" ] && rm -rf $RPM_BUILD_ROOT
mkdir -p $RPM_BUILD_ROOT
qmake-qt5 PREFIX="/usr/local"
make

%install
install -D -m 0755 bin/xdrawchem $RPM_BUILD_ROOT/usr/local/bin/xdrawchem
mkdir -p $RPM_BUILD_ROOT/usr/local/share/xdrawchem
chmod 0755 $RPM_BUILD_ROOT/usr/local/share/xdrawchem
cp ring/* $RPM_BUILD_ROOT/usr/local/share/xdrawchem
mkdir -p $RPM_BUILD_ROOT/usr/local/share/xdrawchem/doc
chmod 0755 $RPM_BUILD_ROOT/usr/local/share/xdrawchem/doc
cp doc/* $RPM_BUILD_ROOT/usr/local/share/xdrawchem/doc

%clean
make clean
[ "$RPM_BUILD_ROOT" != "/" ] && rm -rf $RPM_BUILD_ROOT

%files
%defattr(-,root,root)
%doc README.txt INSTALL.txt TODO.txt COPYRIGHT.txt
/usr/local/bin/xdrawchem
/usr/local/share/xdrawchem/*

%changelog
* Sun Dec 11 2016 Bryan Herger <bherger@users.sf.net> 
- initial version of spec file

