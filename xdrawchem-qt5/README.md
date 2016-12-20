XDrawchem, Qt5 version

Requires OpenBabel (2.2 or later, get openbabel and openbabel-devel if installing packages!) and Qt (5.6 or later, again get all devel packages!)

This code builds and is mostly functional.  It should compile with "qmake" (or "qmake-qt5" if required to select version 5) followed by "make"

You'll probably need to copy manually the contents of the ring folder to /usr/local/share/xdrawchem
Copy the doc folder into the above path (as /usr/local/share/xdrawchem/doc)

Once fully tested, it will become the main branch, with the qt3 and qt4 versions being deprecated.

