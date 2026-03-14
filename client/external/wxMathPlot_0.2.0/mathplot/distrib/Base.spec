# This is the spec file for wxMathPlot
# Copyright (c) 2008, Davide Rondini
# RPM bulding reference:
# http://docs.fedoraproject.org/developers-guide/ch-rpm-building.html
# http://myy.helia.fi/~karte/linux/doc/rpm-build-as-user.html
# Thanks to Matthias Saou for his explanations on http://freshrpms.net/docs/fight.html

Name: wxMathPlot
Version:
Release: 2
Vendor: wxMathPlot team
License: wxWindows
Summary: 2D plot library for wxWidgets
Group: Applications/Development
Packager: Davide Rondini
Source: %{name}-%{version}.tar.gz
# Managing wxWidgets dependecy is quite complicated
# Both Fedora and SuSE (up to 11.3) use the package wxGTK as main package for wxWidgets, but SuSE includes
# everything in one package, while Fedora divides the library into wxBase and wxGTK. So, the dependency list
# removed wxBase to increase compatibility, since in Fedora it is wxGTK depends on it and automatically installs.
# Finally, starting from 11.4, OpenSuSE changed the package name, calling it wxWidgets, forcing to put an if statement
%if 0%{?suse_version} >= 1140
Requires: wxWidgets
BuildRequires: cmake wxWidgets-devel
%else
Requires: (wxGTK or wxGTK3) (wxGTK-gl or wxGTK3-gl)
BuildRequires: cmake (wxGTK-devel or wxGTK3-devel)
%endif

%description
wxMathPlot is a library to add 2D scientific plot functionality to wxWidgets. It allows to embed inside your program a window for plotting scientific, statistical or mathematical data, with additions like legend or coordinate display in overlay.

%global debug_package %{nil}
%prep
%setup

%build

# Another difference between Fedora and other distributions:
# if using {buildroot}/usr as target path the OpenSuSE (and also CentOS?)
# copies the full path inside BUILDROOT, replicating it twice, and generating
# an error. Fedora instead works fine
# if 0{fedora}
# cmake -D CMAKE_INSTALL_PREFIX:STRING={buildroot}/usr -D GDB_DEBUG:BOOL=FALSE -D BUILD_NATIVE:BOOL=TRUE -D MATHPLOT_SHARED:STRING=TRUE -D WXMATHPLOT_BUILD_EXAMPLES:BOOL=FALSE .
# else
cmake -D CMAKE_INSTALL_PREFIX:STRING=/usr -D GDB_DEBUG:BOOL=FALSE -D MATHPLOT_SHARED:STRING=TRUE -D WXMATHPLOT_BUILD_EXAMPLES:BOOL=FALSE .
# cmake -D GDB_DEBUG:BOOL=FALSE -D MATHPLOT_SHARED:STRING=TRUE -D WXMATHPLOT_BUILD_EXAMPLES:BOOL=FALSE .
# endif
make

%install
# rm -rf %{buildroot}
%make_install

%clean
rm -rf %{buildroot}

%post -p /sbin/ldconfig
%postun -p /sbin/ldconfig

%package devel
Summary: 2D plot library for wxWidgets - Development files
Group: Applications/Development
# NOTE on arch: this will set architecture for only the devel package (which do not contain built files)
# This works on recent Fedora, but not on CentOS 5.x (And so Red Hat...), and the procedure is more complicated, explained here:
# http://lists.centos.org/pipermail/centos/2007-December/048261.html
BuildArch: noarch
%description devel
wxMathPlot is a library to add 2D scientific plot functionality to wxWidgets.
It allows to embed inside your program a window for plotting scientific, statistical or mathematical data, with additions like legend or coordinate display in overlay.
This is development package: header files and sample code.


%files
%defattr(-, root, root)
%{_libdir}/libmathplot.so

%files devel
%defattr(-, root, root)
%doc README
%doc Changelog
%{_includedir}/mathplot.h
%{_datadir}/wxMathPlot/samples/*
%{_datadir}/wxMathPlot/Doxyfile

%changelog


