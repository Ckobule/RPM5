Summary:	The rpm that simply won't work.
Name: 		broken
Version:	1
Release:	1
Group: 		System Environment/Base
License:	GPL
Prefix:		/tmp
BuildRoot:	/tmp/%{name}-%{version}-%{release}

Requires:  works


%description
You wanted something more...fine!!!

%install
rm -rf $RPM_BUILD_ROOT/tmp
mkdir -p $RPM_BUILD_ROOT/tmp
touch $RPM_BUILD_ROOT/tmp/%{name}-%{version}-%{release}-%{arch}
exit 0

%pre
exit 1

%files
/tmp/%{name}-%{version}-%{release}-%{arch}
