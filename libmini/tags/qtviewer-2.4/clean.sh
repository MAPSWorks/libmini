#!/bin/tcsh -f
# unix clean script

echo cleaning libmini...
(cd mini; make clean)
echo cleaning libgrid...
(cd grid; make clean)
echo cleaning libsquish...
(cd squish; make clean)
echo cleaning qtviewer...
make clean
