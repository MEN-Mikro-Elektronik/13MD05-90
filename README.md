# 13MD05-90

This repository represents the complete MEN MDIS for Linux System Package plus lowlevel drivers.

MDIS, the MEN Driver Interface System, is a framework to develop device drivers for almost any kind of I/O hardware. A properly written driver runs on all operating systems supported by MDIS. Operating systems currently supported include Windows, VxWorks and Linux.

For more information read [MDIS5 under Linux User Manual](https://www.men.de/loadfile.php?t=2&f=21md05-90.pdf).


## Cloning the repository

Run the commands to clone 13MD05-90 and populate the submodule folder:

~~~sh
git clone --recurse-submodules https://github.com/MEN-Mikro-Elektronik/13MD05-90.git
~~~

or (ssh key is needed):

~~~sh
git clone git@github.com:MEN-Mikro-Elektronik/13MD05-90.git
git submodule update --init --recursive
~~~

After cloning the repository run the '**INSTALL**' script to copy the files to the default location **/opt/menlinux/** or pass another folder name to the script.
