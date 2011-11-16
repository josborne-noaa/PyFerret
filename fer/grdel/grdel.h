#ifndef GRDEL_H_
#define GRDEL_H_

#include <Python.h>

#ifdef VERBOSEDEBUG
#include <stdio.h>
extern FILE *debuglogfile;
#endif

/*
 * Code for a delegate "object" for graphics calls used by Ferret/PlotPlus.
 * Coordinates are (usually) from the bottom left corner increasing to the
 * top right corner.
 *
 * All the graphics functions in this library simply call the appropriate
 * function(s)/method(s) of the graphics library/object being used for that
 * window.
 */

/*
 * Opaque general graphics delegate object type.
 * Valid objects are not NULL.
 */
typedef void * grdelType;

/*
 * Alias of int type to clarify when the value should be considered
 * a boolean type.  Non-zero is considered "true", "yes", or "success".
 * Zero is considered "false", "no", or "failure".
 */
typedef int grdelBool;

/*
 * Global string for error messages.  When a function returns an error
 * value, some explanatory message should be assigned to this string.
 */
extern char grdelerrmsg[2048];

/*
 * Fortran interface for retrieving the error message.
 */
void fgderrmsg_(char *errmsg, int *errmsglen);

/*
 * Prototypes of some Fortran functions
 */
void fgd_bkgcolor_(int *windowid, int *colorindex);
void fgd_getdpi_(int *windowid, float *dpix, float *dpiy);
void fgd_gswkvp_(int *windowid, float *xmin, float *xmax,
                                float *ymin, float *ymax);
void fgd_hide_all_windows_(void);

/*
 * "Window" refers to the full canvas; however, no drawing, except possibly
 * clearing the window, is done on the full window.  Instead, a "View" of
 * the Window is "begun", and drawing is done in that View of the Window.
 */

grdelType grdelWindowCreate(const char *engine, int enginelen, const char *title,
               int titlelen, float width, float height, grdelBool visible);
PyObject *grdelWindowVerify(grdelType window);
grdelBool grdelWindowDelete(grdelType window);
grdelBool grdelWindowClear(grdelType window, grdelType fillcolor);
grdelBool grdelWindowSetSize(grdelType window, float width, float height);
grdelBool grdelWindowSetVisible(grdelType window, grdelBool visible);
grdelBool grdelWindowSave(grdelType window, const char *filename,
                          int filenamelen, const char *fileformat,
                          int formatlen, int transparentbkg);
grdelBool grdelWindowDpi(grdelType window, float *dpix, float *dpiy);

/*
 * Fortran interfaces for the Window functions.
 */
void fgdwincreate_(void **window, char *engine, int *enginelen, char *title,
               int *titlelen, float *width, float *height, int *visible);
void fgdwindelete_(int *success, void **window);
void fgdwinclear_(int *success, void **window, void **fillcolor);
void fgdwinsetsize_(int *success, void **window, float *width, float *height);
void fgdwinsetvis_(int *success, void **window, int *visible);
void fgdwinsave_(int *success, void **window, char *filename, int *namelen,
                 char *fileformat, int *formatlen, int *tranparentbkg);
void fgdwindpi(int *success, void **window, float *dpix, float *dpiy);

/*
 * A "View" refers to a rectangular subsection of the Window (possibly the
 * full Window) which has its own coordinate system.  Drawing is performed
 * using the View's coordinates.  When complete, the View is "ended".  Only
 * one view can be active at a time, so switching between views requires
 * ending one view and beginning another view.
 */

grdelBool grdelWindowViewBegin(grdelType window,
                               float leftfrac, float bottomfrac,
                               float rightfrac, float topfrac,
                               float leftcoord, float bottomcoord,
                               float rightcoord, float topcoord);
grdelBool grdelWindowViewEnd(grdelType window);

/*
 * Fortran interfaces for the Window View functions.
 */
void fgdviewbegin_(int *success, void **window,
                   float *leftfrac, float *bottomfrac,
                   float *rightfrac, float *topfrac,
                   float *leftcoord, float *bottomcoord,
                   float *rightcoord, float *topcoord);
void fgdviewend_(int *success, void **window);

/*
 * All Color, Font, Pens, Brush, or Symbol objects can only be used
 * in the Window from which they were created.
 */

grdelType grdelColor(grdelType window, float redfrac,
               float greenfrac, float bluefrac, float opaquefrac);
PyObject *grdelColorVerify(grdelType color, grdelType window);
grdelBool grdelColorDelete(grdelType color);

grdelType grdelFont(grdelType window, const char *familyname,
               int familynamelen, float fontsize, grdelBool italic,
               grdelBool bold, grdelBool underlined);
PyObject *grdelFontVerify(grdelType font, grdelType window);
grdelBool grdelFontDelete(grdelType font);

grdelType grdelPen(grdelType window, grdelType color,
               float width, const char *style, int stylelen,
               const char *capstyle, int capstylelen,
               const char *joinstyle, int joinstylelen);
PyObject *grdelPenVerify(grdelType pen, grdelType window);
grdelBool grdelPenDelete(grdelType pen);

grdelType grdelBrush(grdelType window,  grdelType color,
               const char *style, int stylelen);
PyObject *grdelBrushVerify(grdelType brush, grdelType window);
grdelBool grdelBrushDelete(grdelType brush);

grdelType grdelSymbol(grdelType window, const char *symbolname,
               int symbolnamelen);
PyObject *grdelSymbolVerify(grdelType symbol, grdelType window);
grdelBool grdelSymbolDelete(grdelType symbol);

/*
 * Fortran interfaces for the Color, Font, Pen, Brush, and Symbol functions.
 */
void fgdcolor_(void **color, void **window, float *redfrac,
               float *greenfrac, float *bluefrac, float *opaquefrac);
void fgdcolordel_(int *success, void **color);

void fgdfont_(void **font, void **window, char *familyname, int *namelen,
               float *fontsize, int *italic, int *bold, int *underlined);
void fgdfontdel_(int *success, void **font);

void fgdpen_(void **pen, void **window, void **color, float *width,
               char *style, int *stylelen, char *capstyle, int *capstylelen,
               char *joinstyle, int *joinstylelen);
void fgdpendel_(int *success, void **pen);

void fgdbrush_(void **brush, void **window, void **color,
               char *style, int *stylelen);
void fgdbrushdel_(int *success, void **brush);

void fgdsymbol_(void **symbol, void **window, char *symbolname, int *namelen);
void fgdsymboldel_(int *success, void **symbol);

/*
 * Drawing commands
 */

grdelBool grdelDrawMultiline(grdelType window, const float ptsx[],
               const float ptsy[], int numpts, grdelType pen);
grdelBool grdelDrawPoints(grdelType window, const float ptsx[],
               const float ptsy[], int numpts, grdelType symbol,
               grdelType color, float ptsize);
grdelBool grdelDrawPolygon(grdelType window, const float ptsx[],
               const float ptsy[], int numpts, grdelType brush,
               grdelType pen);
grdelBool grdelDrawRectangle(grdelType window, float left, float bottom,
               float right, float top, grdelType brush, grdelType pen);
grdelBool grdelDrawMulticolorRectangle(grdelType window,
               float left, float bottom, float right, float top,
               int numrows, int numcols, const grdelType colors[]);
grdelBool grdelDrawText(grdelType window, const char *text, int textlen,
               float startx, float starty, grdelType font, grdelType color,
               float rotate);

/*
 * Fortran interfaces for the drawing commands.
 */
void fgddrawmultiline_(int *success, void **window, float ptsx[],
               float ptsy[], int *numpts, void **pen);
void fgddrawpoints_(int *success, void **window, float ptsx[],
               float ptsy[], int *numpts, void **symbol,
               void **color, float *ptsize);
void fgddrawpolygon_(int *success, void **window, float ptsx[],
               float ptsy[], int *numpts, void **brush, void **pen);
void fgddrawrect_(int *success, void **window, float *left, float *bottom,
               float *right, float *top, void **brush, void **pen);
void fgddrawmulticolor_(int *success, void **window, float *left,
               float *bottom, float *right, float *top, int *numrows,
               int *numcols, void *colors[]);
void fgddrawtext_(int *success, void **window, char *text, int *textlen,
               float *startx, float *starty, void **font, void **color,
               float *rotate);

#endif /* GRDEL_H_ */
