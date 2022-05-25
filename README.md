XFCE4 HotCorner Panel Plugin
==============================

Introduction
--------------

Allow users to set AfterDark-like hot corner actions.

Currently it supports the following operations on the primary display:

  - xfdashboard
  - toggle desktop
  - start screensaver
  - turn off monitor
  - run custom command

Screenshot
------------------------
 ![Screenshot](doc/screenshot.png?raw=true "Screenshot")

Prerequisite
------------------------

  - CMake
  - libwnck >= 3.14.0
  - xfce4-panel >= 4.12.0

Compile & Installation
-------------------------

```console
  git clone git://github.com/christopherkobayashi/xfce4-hotcorner-plugin.git
  cd xfce4-hotcorner-plugin
  cmake -DCMAKE_INSTALL_PREFIX=/usr .
  make install
```

Installation
----------------------

  - ArchLinux AUR: 
    - https://aur.archlinux.org/packages/xfce4-hotcorner-plugin

License
-----------

Licensed under [GNU GPL v2][0] or (at your option) any later version..

[0]: https://www.gnu.org/licenses/gpl-2.0.html
