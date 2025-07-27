# Piduino Blink Daemon

This example shows how to make a daemon that lets you blinking a led.
The program is a modified version of the Blink example. We add the reading of 
the command line arguments in order to modify the number of the led pin 
(-p option), the duration in milliseconds in high state (-H option, default 500) 
and the duration in the low state (-L option, default 500).

We start by building the Blinkd project with Codelite, we will take care to 
choose the Release configuration.

Once the compilation is done, copy the program to `/usr/local/bin` and the 
service script in `/etc/init.d` :

    $ sudo install -m0755 Release/Blinkd /usr/local/bin
    $ sudo install -m0755 blinkd /etc/init.d

After we can do a test on the command line, first, check that the absence of 
parameter causes an output on error :

    $ sudo Blinkd 
    Usage: Blinkd -p pin_number [-H high_delay] [-L low_delay]

Then check that the led connected to pin 0, flashes well :

    $ sudo Blinkd -p 0 -L 50 -H 500
    ^C
    everything was closed.
    Have a nice day !

Then you have to register the `blinkd` service :

    $ sudo systemctl enable blinkd
    blinkd.service is not a native service, redirecting to systemd-sysv-install.
    Executing: /lib/systemd/systemd-sysv-install enable blinkd

Then start the service :

    $ sudo systemctl start blinkd

We can see the status of the service :

    $ sudo systemctl status blinkd
    ● blinkd.service - LSB: blinkd sysv init script
       Loaded: loaded (/etc/init.d/blinkd; generated; vendor preset: enabled)
       Active: active (running) since Sun 2019-02-10 19:19:43 CET; 12s ago
         Docs: man:systemd-sysv-generator(8)
      Process: 5631 ExecStart=/etc/init.d/blinkd start (code=exited, status=0/SUCCESS)
        Tasks: 1 (limit: 4915)
       CGroup: /system.slice/blinkd.service
               └─5636 /usr/local/bin/Blinkd -D -p 0

    févr. 10 19:19:42 nanopineocore2 systemd[1]: Starting LSB: blinkd sysv init script...
    févr. 10 19:19:43 nanopineocore2 blinkd[5631]: Starting Blinking Led server: blinkd.
    févr. 10 19:19:43 nanopineocore2 systemd[1]: Started LSB: blinkd sysv init script.

The service being enabled, we can restart the Pi board and check that the 
service starts by checking that the led flashes.

We can stop the service with the stop command :

    $ sudo systemctl stop blinkd

But it will reboot the next boot of the system, to prevent it, it must disable the service :

    $ sudo systemctl disable blinkd
    blinkd.service is not a native service, redirecting to systemd-sysv-install.
    Executing: /lib/systemd/systemd-sysv-install disable blinkd
    insserv: warning: current start runlevel(s) (empty) of script `blinkd' overrides LSB defaults (2 3 4 5).
    insserv: warning: current stop runlevel(s) (0 1 2 3 4 5 6) of script `blinkd' overrides LSB defaults (0 1 6).







    


    $ sudo systemctl status blinkd
    ● blinkd.service - LSB: blinkd sysv init script
       Loaded: loaded (/etc/init.d/blinkd; generated; vendor preset: enabled)
       Active: inactive (dead) since Sun 2019-02-10 19:22:43 CET; 23s ago
         Docs: man:systemd-sysv-generator(8)
      Process: 5665 ExecStop=/etc/init.d/blinkd stop (code=exited, status=0/SUCCESS)
      Process: 5631 ExecStart=/etc/init.d/blinkd start (code=exited, status=0/SUCCESS)

    févr. 10 19:19:42 nanopineocore2 systemd[1]: Starting LSB: blinkd sysv init script...
    févr. 10 19:19:43 nanopineocore2 blinkd[5631]: Starting Blinking Led server: blinkd.
    févr. 10 19:19:43 nanopineocore2 systemd[1]: Started LSB: blinkd sysv init script.
    févr. 10 19:22:43 nanopineocore2 systemd[1]: Stopping LSB: blinkd sysv init script...
    févr. 10 19:22:43 nanopineocore2 blinkd[5665]: Stopping Blinking Led server: blinkd.
    févr. 10 19:22:43 nanopineocore2 systemd[1]: Stopped LSB: blinkd sysv init script.
