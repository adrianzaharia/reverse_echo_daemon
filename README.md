# reverse_echo_daemon

RPM BUILD: rpmbuild -ba reverse_echo.spec

  The Makefile will create 2 binary files reverse_echo_daemon and reverse_echo client.
  
 Daemon usage: reverse_echo_daemon [-d] [--config CONFIGFILE]
  -d - starts reverse_echo_daemon as daemon.
  --config CONFIGFILE - reads the config file from the specified filed (default /etc/reverse_echo.conf).
  
  Client usage: reverse_echo_client
  The client will start in a shell. 
  Available commands:
    send - send message to daemon
    connect - connect to daemon (connect SERVERIP PORT)
    help
    exit

Config file structure (empty lines and lines starting with '#' are ignored):
      #Server port
      PORT 3538

      #Server ip
      SERVER_IP 127.0.0.1
  
  
  The daemon will bind to the ip address read from the config file and will start listening for clients.
  For each client connected the daemon will start a thread in which it will process the data (reverse the message received).
  
  The client will connect to the daemon using the connect command and will send messages using send command.
  After sending the message the client will wait for the response from daemon.
  
  TODO:
  1. daemon - destroy thread for a disconnected client.
  2. client - add discconect command.
  3. client - add thread for reading data from daemon instead of waiting for it after send
