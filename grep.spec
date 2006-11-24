%define beta %nil
%define rel 1
Summary: The GNU versions of grep pattern matching utilities.
Name: grep
Version: 2.5.2
%if "%{beta}" != ""
Release: 0.%{beta}.%{rel}ark
%else
Release: %{rel}ark
%endif
License: GPL
Group: Applications/Text
Source: ftp://ftp.gnu.org/pub/gnu/grep/grep-%{version}%{beta}.tar.lz
Prereq: /sbin/install-info
Buildroot: %{_tmppath}/%{name}-%{version}-root
Requires: pcre
Buildrequires: pcre-devel

%description
The GNU versions of commonly used grep utilities.  Grep searches
through textual input for lines which contain a match to a specified
pattern and then prints the matching lines.  GNU's grep utilities
include grep, egrep and fgrep.

You should install grep on your system, because it is a very useful
utility for searching through text.

%prep
%setup -q -n %{name}-%{version}%{beta}

%build
[ ! -e configure ] && ./autogen.sh
%configure --without-included-regex
make %?_smp_mflags

%install
rm -rf ${RPM_BUILD_ROOT}
#makeinstall LDFLAGS=-s prefix=${RPM_BUILD_ROOT}%{_prefix} exec_prefix=${RPM_BUILD_ROOT}
make install DESTDIR="$RPM_BUILD_ROOT"
%ifos Linux
mkdir -p $RPM_BUILD_ROOT/bin
mv $RPM_BUILD_ROOT%{_bindir}/* $RPM_BUILD_ROOT/bin
rm -rf $RPM_BUILD_ROOT%{_bindir}
%endif

%find_lang %name

%clean
rm -rf ${RPM_BUILD_ROOT}

%post
[ -e %{_infodir}/grep.info.* ] && /sbin/install-info --quiet --info-dir=%{_infodir} %{_infodir}/grep.info.* || :

%preun
if [ $1 = 0 ]; then
	[ -e %{_infodir}/grep.info.* ] && /sbin/install-info --quiet --info-dir=%{_infodir} --delete %{_infodir}/grep.info.*
fi

%files -f %{name}.lang
%defattr(-,root,root)
%doc ABOUT-NLS AUTHORS THANKS TODO NEWS README ChangeLog

%ifos Linux
/bin/*
%else
%{_bindir}/*
%endif
%{_infodir}/*.info*
%{_mandir}/*/*

%changelog
* Sat Nov 25 2006 Bernhard Rosenkraenzer <bero@arklinux.org> 2.5.2-1ark
- 2.5.2

* Mon Nov 29 2004 Bernhard Rosenkraenzer <bero@arklinux.org> 2.5.1a-1ark
- 2.5.1a
