! The QTViewer - Terrain Rendering with LibMini
Copyright (c) 2012-2014 by Stefan Roettger.

Libmini is free software that displays geographic terrain data at
real-time. This is an example application of integrating libmini in
the Qt user interface. The application, the libMini QTViewer,
primarily uses libmini viewer functions but provides additional camera
control. It is free software licensed under the GPL.

Tested platforms are:
* MacOS X 10.5 and 10.6
* Debian
* Ubuntu 11.04, 12.04, 13.10 and 14.04
* Linux Mint 13
* OpenSuSe 11.4
* Windows XP and Windows 7
Other platforms may work, but are untested.

!! Prerequisites

The compilation of the libMini QTViewer requires the installation of:
 tcsh, autotools and cmake (unix)
 gnu/c++ (unix) or MSVC compiler (windows)
 svn and git (unix) or Tortoise SVN (windows)
 OpenGL (and GLUT)
 qt/qmake

The installation of OpenGL and GLUT is vendor specific: On MacOS X it
is already installed with the XCode development package, on Linux it
comes with the "mesa-dev", "X11-dev" and "free-glut3-dev" development
packages whereas on Windows it is usually installed with the MSVC IDE.

On MacOS X and Windows, it is recommended to build and install Qt from source!
On Linux, it is mostly sufficient to install a recent Qt binary package.

For example, install the Qt package on Ubuntu:
 sudo apt-get install libqt4-dev

If you install Qt from source, grab the source tar ball from:
 Qt4.7: ftp://ftp.qt.nokia.com/qt/source/qt-everywhere-opensource-src-4.7.4.tar.gz/.zip
 Qt5.3: http://download.qt-project.org/official_releases/qt/5.3/5.3.0/single/qt-everywhere-opensource-src-5.3.0.tar.gz/.zip

!! Qt Installation (Unix/Mac)

Type on the unix console in your Qt source directory:
 ./configure -opengl -release -nomake examples -nomake tests -opensource -confirm-license && make && sudo make install

After the build process has finished (go get yourself a cup of coffee),
you will be asked to enter your root password for installation of Qt.

!! Qt Installation (Windows):

Open a MSVC command prompt via
 Programs -> MSVC -> Tools -> MSVC Command Prompt
Choose an appropriate platform to describe the version of MSVC:
 MSVC 8.0 -> SET PLATFORM=win32-msvc2005
 MSVC 9.0 -> SET PLATFORM=win32-msvc2008
 MSVC 10.0 -> SET PLATFORM=win32-msvc2010
Navigate into the Qt source directory and type
 configure -platform %PLATFORM% -opengl desktop -release -nomake examples -nomake tests -opensource -confirm-license
 nmake
Add the "bin" directory in your Qt directory to your path:
 set PATH=%cd%\bin;%PATH%
Or add the "bin" directory to your PATH environment variable:
 System -> Advanced -> Environment Variables -> PATH
Check for working qmake
 qmake --version

At your option, you can configure Qt to produce static libraries with
 configure -platform %PLATFORM% -opengl desktop -release -static -no-libpng -no-sql-sqlite -nomake examples -nomake tests -opensource -confirm-license

!! Installation of Libmini Dependencies (Unix/Mac)

Get the libMini dependencies from the libMini subversion repository.
Type on the unix console in your project directory (e.g. ~/Projects):
 svn co http://libmini.googlecode.com/svn/libmini/deps deps
 (cd deps; ./build.sh)

!! Installation of Libmini Dependencies (Windows)

Precompiled static libraries for Windows MSVC 2005 are provided in the
libMini WIN32 directory. In your project directory, get the following
sub-directory via Tortoise SVN:
 http://libmini.googlecode.com/svn/libmini/WIN32

Additionally we need GDAL to be installed. In your project directory,
get GDAL via Tortoise SVN:
 http://svn.osgeo.org/gdal/tags/1.8.1/gdal
The settings of the Windows GDAL build are defined in nmake.opt.
 To build GDAL we need to override the following settings:
  MSVC_VER -> use 1600 to identify MSVC 10.0, 1500 for MSVC 9.0, etc.
  GDAL_HOME -> needs to point to the GDAL installation directory
   Usually this means that we point it to our project directory
Open a MSVC command prompt via
 Programs -> MSVC -> Tools -> MSVC Command Prompt
 Navigate into the GDAL source directory and type
  set PROJ="your project directory"
   e.g set PROJ="C:\Users\roettger\Projects"
  nmake /f makefile.vc MSVC_VER=1600 GDAL_HOME=%PROJ%
  nmake /f makefile.vc MSVC_VER=1600 GDAL_HOME=%PROJ% devinstall
  nmake /f makefile.vc MSVC_VER=1600 GDAL_HOME=%PROJ% staticlib
  copy /y gdal.lib %PROJ%\lib
  copy /y port\*.h %PROJ%\include
  copy /y gcore\*.h %PROJ%\include
  copy /y ogr\*.h %PROJ%\include

!! QTViewer Checkout and Compilation (Unix/Mac)

Type on the unix console in your project directory:
 svn co http://libmini.googlecode.com/svn/libmini/qtviewer qtviewer
 with cmake: (cd qtviewer; cmake . && make)
 with qmake: (cd qtviewer; ./build.sh)

If you installed Qt5, be sure to enable the BUILD_WITH_QT5 option:
 (cd qtviewer; cmake -DBUILD_WITH_QT5 . && make)

!! QTViewer Checkout and Compilation (Windows)

In your project directory, get the following subversion repository
via Tortoise SVN:
 http://libmini.googlecode.com/svn/libmini/qtviewer
Open a MSVC command prompt via
 Programs -> MSVC -> Tools -> MSVC Command Prompt
Navigate into the qtviewer source directory and type
 build.bat

!! Installation (Unix)

If you want to install the application as a desktop application, just
run the install script:
 ./install.sh

After installation, the viewer is available as desktop application. On
Ubuntu, for example, you can simply search for it in the start menu.

!! Usage

The application can be started without command line parameters:

 ./qtviewer

Then the application will show the entire earth on startup.

The application also accepts a list of urls as parameters.
Each url needs to point to a geotiff image layer or a tileset directory.

Usage on the unix terminal:
 ./qtviewer { <image.tif> | <tileset url> }

Then the application will zoom into the first specified argument.

!! Image Layers

An image layer can contain imagery (color layer) or elevation
information (dem layer).

The usual file format for color and dem layers is geotiff. See
http://www.gdal.org for more information on that format and additional
tools to process geotiff images.

!! Tileset URLs

A tileset url can point to a regular libGrid or VTBuilder
(vterrain.org) tileset. The tileset directory layout of each tileset
(uri or url) has to be:

 elev/    - directory of elevation tiles
 elev.ini - ini file for elevation directory
 imag/    - directory of imagery tiles
 imag.ini - ini file for imagery directory

The elev and image directories contain the tiles of the tileset stored
in the proprietary libMini DB format:
* The DB format provides mip-mapped s3tc-compressed image data.
* The DB format also provides lzw-compressed floating point elevation data.

!! User Interface

The QTViewer user interface supports drag and drop of geo-referenced
images and tileset urls.

The images are displayed as 3D thumbnails at the geo-referenced
position, whereas the tilesets are displayed in full 3D. The images
and tilesets are also displayed in a list view on the right.

Right-clicking on a layer in the list view triggers a context menu
with a variety of available options:
* A single layer or image can be opened.
** It is displayed as flat geo-referenced thumbnail.
* A new geo-referenced extent layer can be created from the layer.
* Multiple layers can be selected or unselected.
** A layer is also selected by clicking on it in the list view and
   pressing return or right-clicking on it in the 3D view.
* A single layer can be inspected.
** Info about its size and geo-reference is shown.
* The display of a layer can be turned on or off.
* The full-resolution display of a layer can be toggled.
** The full-resolution display is also toggled by right double-clicking
   on it in the 3D view.
* Multiple selected images can be resampled to a produce a compressed tileset.
** A tileset is displayed in full 3D.
** Its appearance takes both selected color and dem layers into account.
** At least a single dem layer must be selected.
** Resampling takes place in a background thread.
*** Progress is shown in percent below the list view.
*** Resampling is a computationally expensive operation and can take several minutes to hours.
*** It produces a number of tiles that are stored in the proprietary libMini DB format.
*** For more information about resampling, please see the README of the libGrid library.
** After resampling, the original flat layers are hidden and replaced by the 3D resampled tileset.
*** The tiles of the tileset are paged in and out on demand.
*** Therefore, a tileset is in principle not limited in size or extent.
* A single layer can be resampled to a produce a plain tileset.
* A single dem layer can be shaded to produce a grayscale image.
** A diffuse noon lighting technique is applied.
* Multiple layers can be cropped and merged to produce a new layer.
* Resampling and merging operations in the background can be aborted.
* A single dem layer can be modified to yield a new dem layer.
* A single dem layer can be mapped to yield a new image layer.
* Multiple layers can be merged and matched to yield a new image layer.
* A single layer can be saved in the the DB, TIF or JpegInTif format.
* All layers can be saved in a grid file.
** The layers are reopend by dragging the grid file into the list view.
* Finally, a single layer or multiple selected layers can be removed.

!! Example Data

For example, you can drag&drop a tileset data directory into the
viewer window:

Download the Hawaii or Fraenkische demo from stereofx.org/download and
drag&drop the data/HawaiiTileset or data/FrankischeTileset directory
into the viewer window. Double-clicking on the layer brings the
respective tileset into view.

You can also drag in layers from the libGrid data directory:
 http://code.google.com/p/libgrid/source/browse/libgrid/data

The best way to get that data directory is to check it out via svn:
 svn checkout http://libgrid.googlecode.com/svn/libgrid/data data

!! Usage Example

A simple usage example in 5 steps - we produce and visualize a 3D tileset
for the Island of Oahu, Hawai'i, by resampling a dem and a color layer:

* We assume that we have the libgrid data directory available.
* Step 1) Drag the "Oahu-10.tif" dem layer from the "elev" directory into the qtviewer.
* Step 2) Drag the "Oahu-25.tif" color layer from the "imag" directory into the qtviewer.
* Step 3) Right click at the list view and choose "resample all" from the context menu.
* Step 4) Wait several minutes while the resampling takes place.
* Step 5) Then zoom into the produced tileset with the middle mouse wheel to see some 3D details.

If this takes too long for you impatient guys, we can do the same with
a small island off the east coast of Oahu, Manana Island, for a total
of just 3 simple steps:

* We assume that we have the libgrid data directory available.
* Step 1) Drag the "MananaIsland.tif" dem layer from the "elev/Oahu-Islands" directory into the qtviewer.
* Step 2) Right click at the list view and choose "resample all" from the context menu.
* Step 3) Zoom into Manana Island to see some 3D details.

!! Navigation

Navigation in the 3D view is self-explanatory via mouse or touch pad gestures:

 single click               - focus on target
 single click/drag          - rotate about focus
 double click               - focus and zoom on target
 mouse wheel scroll         - pan forward/backward
 touch pad scroll           - pan left/right and zoom in/out
 shift & single click drag  - heading and pitch
 shift & double click       - zoom out
 shift & touch pad scroll   - pan left/right and forward/backward
 right click                - select picked layer
 right double click         - show picked layer in full-res

!! Extension

The QTViewer can be extended to display additional geometry (such as
buildings, GPS paths etc.) in a georeferenced ECEF context by deriving
a Renderer class from QTViewer's Viewer class. In that class, we
overload the build_ecef_geometry() method, which constructs a scene
graph using libMini's mininode class as a basis.

For example, we can add a cylinder as the north pole by specifying its
georeferenced position (mininode_coord), color (mininode_color) and
geometry (mininode_geometry_tube) and linking those node components
together as a simple scene graph. The so defined scene graph is
rendered on top of the earth and on top of any loaded libGrid or
VTBuilder tilesets:

 class Renderer: public Viewer
 {
    protected:

    mininode_group *Renderer::build_ecef_geometry()
    {
       mininode_group *scene =
          new mininode_coord(minicoord(miniv3d(0, 90*3600, 0), minicoord::MINICOORD_LLH))->
             append_child(new mininode_color(miniv3d(0.0, 0.0, 1.0)))->
             append_child(new mininode_geometry_tube(10000, 500000));

       return(scene);
    }
 };

More information about libMini's node classes can be found in the
mininodes.h header.

!! Programming Example

In this programming example we are going to put a building on a place
on earth.

The place on earth is defined as a local coordinate system at a
geo-referenced position. Possible geographic reference systems are
Lat/Lon, UTM, Mercator or ECEF. The z-axis of the local coordinate
system points upwards and the y-axis points north. The units of the
local coordinate system are meters.

The building is modeled in the local coordinate system as a scene
graph that contains two simple geometric objects, a cube as the base
and a prism as the roof.

The scene graph is constructed from a set of graph nodes as follows:

* derive from '''Viewer''' class
* overwrite build_ecef_geometry method to describe the scene as a graph
* construct scene graph to contain a single house in a local coordinate system
** at position Lat/Lon = 49 degrees latitude, 11 degrees longitude
*** via appending a coord node to the scene graph
** and define the local scale of the building
*** via appending a scale node
** and define the appearance of the building's components
*** via appending color nodes
** and define the proper positions of the components in the local coordinate system
*** via adppending translate nodes
** and add the geometry of the house to the scene graph
*** via appending cube and prism nodes

All graph nodes are derived from libMini's mininode base class.

 class Renderer: public Viewer
 {
    protected:

    mininode_group *Renderer::build_ecef_geometry()
    {
       // define local coordinate system at Lat/Lon=49/11
       mininode *house = new mininode_coord(minicoord(miniv3d(11*3600, 49*3600, 0),
                                            minicoord::MINICOORD_LLH));

       // scale up by a factor of 10000
       mininode *scale = house->append_child(new mininode_scale(10000));

       // white cube with base size 10x10 and height 5 meters
       // origin of the cube is its barycenter
       // translate up 2.5 meters
       scale->append_child(new mininode_color(miniv3d(1.0, 1.0, 1.0)))->
          append_child(new mininode_translate(miniv3d(0.0, 0.0, 2.5)))->
          append_child(new mininode_geometry_cube(10.0, 10.0, 5.0));

       // grey prism as roof with base size 10x10
       // origin of the prism is the center of its base plane
       // translate up 5 meters
       scale->append_child(new mininode_color(miniv3d(0.5, 0.5, 0.5)))->
          append_child(new mininode_translate(miniv3d(0.0, 0.0, 5.0)))->
          append_child(new mininode_geometry_prism(10.0, 10.0, 2.5));

       return(house);
    }
 };

!! Customized Geometry Nodes

In order to implement geometry nodes with special objects not covered
by the basic geometry types (boxes, tetrahedra, prisms, spheres etc.)
we derive from mininode_geom and override the following three virtual
methods:

* rendergeo: render geometric primitives
** front facing triangles are specified counter-clock-wise
** back facing triangles are culled
** normals are specified to face outward
** a head light shader is applied by default
** inward facing normals are flipped when shaded
* getbbox: provide information about the bounding box of the primitives
* shoot: compute the intersection of a ray with the geometric primitives
** by default the hit point is approximated by the intersection with the bounding sphere
** if exact hit points are required, override the shoot method and provide exact calculations

See the mininode_teapot.cpp module as an example:

 mininode_geometry_teapot::mininode_geometry_teapot()
    : mininode_geom()
    {}

 void mininode_geometry_teapot::rendergeo()
    {
    glDisable(GL_CULL_FACE);

    glVertexPointer(3,GL_FLOAT,0,teapotVertices);
    glEnableClientState(GL_VERTEX_ARRAY);
    glNormalPointer(GL_FLOAT,0,teapotNormals);
    glEnableClientState(GL_NORMAL_ARRAY);

    drawTeapotElements();

    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_NORMAL_ARRAY);

    glEnable(GL_CULL_FACE);
    }

 void mininode_geometry_teapot::getbbox(miniv3d &bboxmin,miniv3d &bboxmax) const
    {
    bboxmin=miniv3d(-3,0,-2);
    bboxmax=miniv3d(3.434,3.15,2);
    }

 double mininode_geom::shoot(const miniv3d &o,const miniv3d &d,double mindist) const
    {return(shootbsphere(o,d,mindist));}

We put the teapot at the place where it belongs to - the Utah Science
and Imaging Institute (SCI):

 mininode_ref teapot = new mininode_geometry_teapot;

 minicoord utah;
 utah.set_llh(40.767690,-111.845037); // utah science and imaging institute (sci)

 group->append_child(new mininode_coord(utah))->
    append_child(new mininode_color(miniv3d(0.9, 0.9, 0.9)))->
    append_child(new mininode_scale(10000.0))->
    append_child(new mininode_translate(miniv3d(0.0, 0.0, 0.1)))->
    append_child(new mininode_rotate(90, miniv3d(1.0, 0.0, 0.0)))->
    append_child(teapot);

!! CMake Example

The procedure of adding ecef geometry, as explained in the previous
sections, is illustrated in the CMake example (in the example.cmake
folder). It shows a rotating earth with an additional pole axis
(mininode_geometry_tube) and an equator ring (mininode_geometry_band).

This example is a good starting point to code your own geographic
applications. For example, you can show a tileset that has been
resampled with the qtviewer (or vtbuilder) just by issuing the
following commands in the MainWindow::timerEvent method:

 static bool loaded=false;
 if (!loaded)
    {
    viewerWindow->getViewer()->loadMap("url or file path to tileset");
    loaded=true;
    }
 viewerWindow->repaint();

Then you can navigate to a particular view point given in a geografic
coordinate system (Lat/Lon, UTM, Mercator, etc.):

 // define eye coordinates
 minicoord eye;
 double lat=49.5,lon=11.05, height=1000.0; // 1 km above Nuremberg airport
 eye.set_llh(lat,lon);

 // set camera to eye coordinates looking down by default
 viewerWindow->getViewer()->getCamera()->set_eye(eye);
 viewerWindow->repaint();

Or just let the earth rotate:

 // rotate camera left so that earth rotates right
 viewerWindow->getViewer()->getCamera()->move_left(10000.0);
 viewerWindow->repaint();

!! Most Simple CMake Example

The most simple usage of the qt viewer is to just derive from the
QtViewer class provided in the <mini/qt_viewer.h> header and override
the timer event and the ecef geometry construction pure virtual
methods. Source code for this use case is given in the simple.cmake
folder. To start, just have a look at the qt_viewer.h header, which
contains the complete definition of the QtViewer base class that can
be subclassed as follows:

 #include <mini/qt_viewer.h>

 class MyQtViewer: public QtViewer
 {
 public:

    MyQtViewer() {}
    ~MyQtViewer() {}

 protected:

    virtual mininode_group *build_ecef_geometry()
    {
       // return empty group node
       // append nodes at your pleasure
       mininode_group *group = new mininode_group();
       return(group);
    }

    virtual void timerEvent(QTimerEvent *)
    {
       // rotate camera left so that earth rotates right
       getCamera()->move_left(10000.0);
       repaint();
    }
 };

That's it!

!! Postcard

If you found the software useful, please send a vacation postcard to:

 Prof. Dr. Stefan Roettger
 Wassertorstr. 10
 90489 Nuernberg
 Germany

Thanks!
