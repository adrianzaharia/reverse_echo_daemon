Name:     reverse_echo
Version:  0.1
Release:  1
Summary:  Reverse echo on TCP
License:  GPLv2
URL:  https://github.com/adrianzaharia/reverse_echo_daemon.git

%description
reverse echo on TCP

%define NVdir   %{name}-%{version}

%prep
rm -rf %{NVdir}
git clone %{url} %{NVdir}
cd %{NVdir}

%build
make -C %{NVdir}

%install
make -C %{NVdir} install INSTALL_DIR=$RPM_BUILD_ROOT

%files
/etc/reverse_echo.conf
/usr/bin/reverse_echo_daemon
/usr/bin/reverse_echo_client
