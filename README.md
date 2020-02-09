
This is a test server to play with capabilties, apparmor and selinux. Git-clone the repo to the location and follow the instructions below.
```
mkdir /app
cd /app
git clone <URL>
```

# building
To build the server, install the dependent libraries and programs and run make. This creates the file ./server.
```
sudo apt-get install libcap-dev
make
```

# install
The program runas as user app in dir /app/server
```
useradd -u 1337 -d /app/server -s /usr/sbin/nologin app
install -D ./server /app/server
setcap 'cap_net_bind_service=+ep' /app/server
```

# running
The program binds on port 80. As you connect to it via netcat and send data, it triggers a vulnerability code residing in attack.c. The attack code opens a port and runs /bin/sh via execve.
```
./server
```

# capabilties
The capabilties are set via setcap (see install).

# apparmor
```
cp apparmor-profile /etc/apparmor.d/app.server
apparmor_parser -R /etc/apparmor.d/app.server
aa-audit /app/server # or aa-enforce /app/server
tail -f /var/log/syslog
```
