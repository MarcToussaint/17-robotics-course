NOTE: if installing on Ubuntu 16.04, please see README.1604.md

# MLR Repository

- Infos for students:  README.student.md
- Infos about the PR2: README.PR2.md

## Quick Start on Ubuntu 16.04
Install git:

    $ sudo apt-get install git

Add your ssh key to clone the repository.

    User->Settings->SSH Keys

Clone the repo:

    $ cd ~; mkdir git; cd git
    $ git clone git@animal.informatik.uni-stuttgart.de:mlr-staff/mlr.git
    
Once the repo has been cloned:

    $ cd mlr
    $ git checkout roopi
	$ ./install/INSTALL_ALL_UBUNTU_PACKAGES.sh
    $ ./install/INSTALL_ROS_PACKAGES_KINETIC
    $ ./share/bin/createMakefileLinks.sh
    $ cp share/build/config.mk.kinetic share/build/config.mk
    $ cd share/examples/Roopi/basics/
    $ make
    
## Install on Ubuntu 14.04

Install the mlr system by executing:

	./install/INSTALL_ALL_UBUNTU_PACKAGES.sh

The 'install/INSTALL' script does some more local installation of
packages. But this is usually not necessary!

To use the gof (good old-fashioned) Makefiles,

	cd share; bin/createMakefileLinks.sh

Also,

	cd share/gofMake; cp config.mk.default config.mk
	
and edit the latter to change the compile settings, e.g., (un)set dependencies.


## Install on Arch

What to do to install MLR on Arch Linux?

First install all dependencies. You need some things from AUR, so first install
yaourt or something similar to handle AUR packages.

    $ yaourt -S opencv plib ann lapack blas qhull

In order to get a working cblas library we need atlas-lapack, but be warned.
This literally will take hours of compile time (something like 5-10 h depending
on your system).

    $ yaourt -S atlas-lapack

(You can either drink a lot of coffee in the meantime or put LAPACK = 0 in your
gofMake/config.mk until it is finished...)

MLR needs a .so-file called libANN, ann provides one, but names it libann so we
link it:

    $ sudo ln -s /usr/lib/libANN.so /usr/lib/libann.so
    $ sudo ldconfig

qhull is newer in arch than in ubuntu and changes some pointer/reference things.
You need to put 

   ARCH_LINUX = 1

in your gofMake/config.mk. This fixes the qhull change.

Everything else is equal to ubuntu.


## Install troubleshooting

* If there is a segmentation fault in c_init (before main) in some
  boost library, maybe this is caused by linking to PCL in the
  src/Perception code. Fix: in share/gofMake/config.mk insert a line
  'PCL=0', which disables linking to PCL.


## QtCreator tips

* Enable Debugging helpers:
** OLD WAY:
  .gdbinit -> git/mlr/tools/qt_mlr_types.py
** NEW WAY:
  in Options -> Debugger -> GDB -> Extra Dumpers add
  'git/mlr/tools/qt_mlr_types.py'
  In this file ther must not be a 'python' and 'end' flags!

* Enable std::... pretty printing:
  Options > Debugger > GDB ... UNCHECK the 'Load system GDB pretty printers'

* set formatting:
  Options > C++ > Import...   git/mlr/tools/qt_coding_style.xml
  
* Change signal handling of gdb
  Open 'Debugger Log' window
  write gdb commands:
  handle SIGINT pass
  handle SIGINT noprint
  handle SIGINT nostop
  handle SIGINT ignore

* append source directory to all "*.includes" files:
echo "echo '/home/mtoussai/git/mlr/share/src' >> \$1" > nogit_append_path
chmod a+x nogit_append_path
find . -name '\.*\.includes' -exec ./nogit_append_path {} \;


## Getting started

The best way to learn about this code and understand what is can do it
to go through all examples. And take a look at the Doxygen
documentation
http://sully.informatik.uni-stuttgart.de:8080/job/MLR-share-doxygen/doxygen/

For the examples start with

 - examples/Core
 - examples/Gui
 - examples/Ors

All of them should work. Going through the main should be instructive
on how you could use the features. In examples/Ors some demos depend
on linking to Physx which is turned off on default.

Continue with more specialized packages:

 - examples/Optim
 - examples/Motion
 - examples/Algo
 - examples/Infer

which implement algorithmic methods.

Finally examples/Hardware contains drivers/recorders for Kinect, Cams,
G4.

examples/System is code for designing parallized modular systems
(using Core/module.h) - which I think works robustly but needs more
tools for others to be usable.

examples/relational is Tobias' relational RL code


## Documentation

Create the doxygen documentation with:

    cd share
    make doc


# WINDOWS

Large parts of the code compile with MSVC. Try this:

download https://ipvs.informatik.uni-stuttgart.de/mlr/marc/source-code/libRoboticsCourse.13b.msvc.tgz

goto  http://www.microsoft.com/en-us/download/developer-tools.aspx

search for Visual C++ 2010 Express

download and install

open the 'solution' msvc/RoboticsCourse.sln

Creating a new project is tricky! You could avoid it by copying new
code into an existing project, replacing the main.cpp (e.g. in
example\Ors\ors). But in case you need to:

When creating new projects:
-- set the MT_MSVC preprocessor flag (perhaps also MT_FREEGLUT,
   MT_ANN, MT_GL, MT_QHULL)
-- set the Debugger work directory to ..\..\share\example\Ors\ors or
   alike
-- set an Additional Includepath to ..\..\share\src
-- perhaps set the VC++ Directory 'libpath' to ..\..\msvc\freeglut\lib
-- add the created lib-files in msvc\Debug to the project (drag and
   shift-drop from others)
-- understand, that programs are by default run in msvc\Debug
-- perhaps you need to copy dlls around - (don't know how to set the
   path globally)


# Run on Virtual Machines

Setting up this software to run on a virtual machine is an option for whoever
can't make it work on his OS. The biggest issue of this approach is getting the
OpenGL rendering to work.

Instructions:

1) Install the most recent version of virtualbox for your OS.  (don't use the
package repository, use https://www.virtualbox.org/wiki/Downloads).

2) Download the ISO for the desktop-amd64 version of Ubuntu (12.04 is
recommended, more recent versions may also work) and install it on a new VM.

3) run:
sudo apt-get update
sudo apt-get upgrade

4) [Optional] If the above software updates are slow, close the VM and change
the network setting "Attached to: Bridged Adapter".

5) [Optional] Step 6 is said to require the linux-headers of the current
version.  Nonetheless, I've made everything work already before without this
step.

run:
sudo apt-get install build-essential linux-headers-$(uname -r)

6) Select Devices -> Insert Guest Additions CD Image. Then run the autorun
script in the mounted CD drive. This installs the virtualbox-guest-addition
software specific for your virtualbox version (don't use package repository
version).  Check that everything is installed correctly.

7) If there were no problems with the installation, unmount the CD drive and
reboot the VM.

8) Proceed installing the MLR code base.

Tips)

Take frequent snapshots of the VM, whenever you're in a state where it is
working correctly (this also helps during the above setup procedure). If things
break at some point, transfer the code you've written (either using some
versioning tool, or a USB stick) to the most recent snapshot of the VM and
proceed working.


# MISC

* Installing portaudio19-dev: Currently has broken links in
  ubuntu. Solution: first install libjack-jackd2-dev
