Argonaut
========

A spatial directory manager.

Spatial?
--------

Things that make it spatial:

* The same directory appears in the same place on your screen in the same
  dimensions consistently.

  A consequence of this is that directories must open in new windows.

* A window is a directory, and a directory is a window.

  A consequence of this is that each directory can only be open once. When
  combined with virtual desktops the most straightforward way to think about
  the desired functionality is: the existing window closes and a new one opens.

Setup
-----

Depends on GTK+ 3, GModule 2, and GIO 2.

To develop:

  ./autogen.sh
  ./configure --datadir=$PWD
  make

Copyright
---------

Copyright 2011, 2014 Mike Burns and Matt Horan
