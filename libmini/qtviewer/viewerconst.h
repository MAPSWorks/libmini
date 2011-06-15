#ifndef VIEWERCONST_H
#define VIEWERCONST_H

#define VIEWER_FPS      25.0f

#define VIEWER_FOVY     60.0f
#define VIEWER_NEARP    50.0f
#define VIEWER_FARP     1000000.0f

#define VIEWER_FOGSTART 0.5f
#define VIEWER_FOGDENSITY 0.2f;

#define VIEWER_VOIDSTART 100000.0f;
#define VIEWER_ABYSSSTART -500.0f;

#define VIEWER_SCALE    100.0f
#define VIEWER_EXAGGER  3.0f
#define VIEWER_RES 1.0E4f
#define VIEWER_RELRES 0.25f
#define VIEWER_RANGE 0.01f
#define VIEWER_RELRANGE 0.25f
#define VIEWER_SEALEVEL 0.0f//-MAXFLOAT
#define VIEWER_CONTOURS 10.0f
#define VIEWER_SEABOTTOM -1000.0f

#define VIEWER_BATHYSTART 0.0f
#define VIEWER_BATHYEND -10.0f
#define VIEWER_BATHYWIDTH 256
#define VIEWER_BATHYHEIGHT 2
#define VIEWER_BATHYCOMPS 4

#define VIEWER_NPRBATHYSTART 0.0f
#define VIEWER_NPRBATHYEND 1000.0f
#define VIEWER_NPRBATHYWIDTH 1024
#define VIEWER_NPRBATHYHEIGHT 2
#define VIEWER_NPRBATHYCOMPS 4

#define VIEWER_NPRCONTOURS 100.0f

#define VIEWER_SIGNPOSTHEIGHT 100.0f
#define VIEWER_SIGNPOSTRANGE 0.25f
#define VIEWER_BRICKSIZE 100.0f
#define VIEWER_BRICKRADIUS 1000.0f
#define VIEWER_BRICKSCROLL 0.5f

#define CAMERA_HEIGHT_CEILING   45000
#define CAMERA_HEIGHT_FLOOR     100

#define SAFE_DECLARE_STRING(name, max_len)      \
        name = new char[max_len + 1];           \
        memset(name, 0, sizeof(#name));

#define SAFE_DELETE_STRING(name)                \
        if (name != NULL)              \
        delete[] name;

#endif // VIEWERCONST_H
