XFCE4 HotCorner Panel Plugin
==============================

Introduction
--------------

This is aim to provide an easy to use xfce4 panel plugin that let users set hot corner action.

Currently it supports the following operation in primary display:

  - xfdashboard
  - toggle desktop
  - start screensaver
  - turn off monitor

Prerequisite
------------------------

  - CMake
  - XFCE 4.6

Compile & Installation
-------------------------

```console
  git clone git@github.com:brianhsu/xfce4-hotcorner-plugin.git
  cd xfce4-hotcorner-plugin
  cmake -DCMAKE_INSTALL_PREFIX=/usr .
  make install
```

