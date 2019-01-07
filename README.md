XFCE4 HotCorner Panel Plugin
==============================

Introduction
--------------

This is aim to provide an easy to use xfce4 panel plugin that let users set hot corner action.

Currently it supports the following operation in primary display:

  - xfdashboard
  - start screensaver
  - turn off monitor
  - run custom command

Screenshot
------------------------

 ![Screenshot](http://i.imgur.com/575haSy.png)

Prerequisite
------------------------

  - CMake
  - xfce4-panel >= 4.12.0

Compile & Installation
-------------------------

```console
  git clone git://github.com/brianhsu/xfce4-hotcorner-plugin.git
  cd xfce4-hotcorner-plugin
  cmake -DCMAKE_INSTALL_PREFIX=/usr .
  make install
```

Installation
----------------------

  - ArchLinux AUR: 
    - https://aur4.archlinux.org/packages/xfce4-hotcorner-plugin

  - Gentoo Ebuild: 
    - https://gist.github.com/brianhsu/637a906e9a1e17ca6392

License
-----------

Licensed under [GNU GPL v2][0] or (at your option) any later version..

[0]: https://www.gnu.org/licenses/gpl-2.0.html
