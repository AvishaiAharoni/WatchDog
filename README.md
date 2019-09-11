# WatchDog
An app designed to make sure that another app didn't close


The watchdog is attached to a process and with the help of signals checked if the process being watched is alive and running.
If the process shuts down for any reason the watchdog uses fork and exec to resurrect the process.
