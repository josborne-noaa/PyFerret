
/*******************************************************************************
	Splash.c

       Associated Header file: Splash.h
*******************************************************************************/

#include <stdio.h>
#include <Xm/Xm.h>
#include <Xm/MwmUtil.h>
#include <Xm/MenuShell.h>
#include "UxXt.h"

#include <Xm/Text.h>
#include <Xm/ScrolledW.h>
#include <Xm/PushB.h>
#include <Xm/Label.h>
#include <Xm/Form.h>
#include <X11/Shell.h>

/*******************************************************************************
       Includes, Defines, and Global variables from the Declarations Editor:
*******************************************************************************/

#include "noaa.logo.xpm"

/* prototypes */
static void InitInterface(void);
extern Pixmap GetPixmapFromData(char **inData);
extern Pixmap GetPixmapFromFile(char *inFile);

swidget gSavedSplash = NULL;
swidget Splash;

extern Boolean gFerretIsStarting, gHiRez;


static	Widget	form1;
static	Widget	ferret_version;
static	Widget	noaa_logo;
static	Widget	pushButton1;
static	Widget	author;
static	Widget	scrolledWindowText1;
static	Widget	scrolledText1;
static	swidget	UxParent;

#define CONTEXT_MACRO_ACCESS 1
#include "Splash.h"
#undef CONTEXT_MACRO_ACCESS

Widget	Splash;

/*******************************************************************************
Auxiliary code from the Declarations Editor:
*******************************************************************************/

static void InitInterface()
{
	XtVaSetValues(UxGetWidget(noaa_logo),
		XmNlabelType, XmPIXMAP,
		XmNlabelPixmap, GetPixmapFromData(noaa_logo_xpm),
		NULL);
}

/*******************************************************************************
       The following are callback functions.
*******************************************************************************/

static	void	destroyCB_Splash(
			Widget wgt, 
			XtPointer cd, 
			XtPointer cb)
{
	Widget                  UxWidget = wgt;
	XtPointer               UxClientData = cd;
	XtPointer               UxCallbackArg = cb;
	gSavedSplash = NULL;
}

static	void	popupCB_Splash(
			Widget wgt, 
			XtPointer cd, 
			XtPointer cb)
{
	Widget                  UxWidget = wgt;
	XtPointer               UxClientData = cd;
	XtPointer               UxCallbackArg = cb;
	InitInterface();
}

static	void	activateCB_pushButton1(
			Widget wgt, 
			XtPointer cd, 
			XtPointer cb)
{
	Widget                  UxWidget = wgt;
	XtPointer               UxClientData = cd;
	XtPointer               UxCallbackArg = cb;
	{
	extern swidget splash;
	
	XtPopdown(UxGetWidget(Splash));
	}
}

/*******************************************************************************
       The 'build_' function creates all the widgets
       using the resource values specified in the Property Editor.
*******************************************************************************/

static Widget	_Uxbuild_Splash()
{
	Widget		_UxParent;


	/* Creation of Splash */
	_UxParent = UxParent;
	if ( _UxParent == NULL )
	{
		_UxParent = UxTopLevel;
	}

	Splash = XtVaCreatePopupShell( "Splash",
			topLevelShellWidgetClass,
			_UxParent,
			XmNx, 391,
			XmNy, 184,
			XmNwidth, 340,
			XmNheight, 470,
			XmNtitle, "Ferret 4.4",
			XmNminWidth, 340,
			XmNallowShellResize, TRUE,
			NULL );
	XtAddCallback( Splash, XmNdestroyCallback,
		(XtCallbackProc) destroyCB_Splash,
		(XtPointer) NULL );
	XtAddCallback( Splash, XmNpopupCallback,
		(XtCallbackProc) popupCB_Splash,
		(XtPointer) NULL );



	/* Creation of form1 */
	form1 = XtVaCreateManagedWidget( "form1",
			xmFormWidgetClass,
			Splash,
			XmNheight, 470,
			XmNwidth, 340,
			NULL );


	/* Creation of ferret_version */
	ferret_version = XtVaCreateManagedWidget( "ferret_version",
			xmLabelWidgetClass,
			form1,
			RES_CONVERT( XmNlabelString, "Ferret 4.4" ),
			XmNtopOffset, 20,
			XmNtopAttachment, XmATTACH_FORM,
			XmNleftPosition, 50,
			XmNleftOffset, -100,
			XmNleftAttachment, XmATTACH_POSITION,
			XmNresizable, FALSE,
			XmNwidth, 200,
			NULL );


	/* Creation of noaa_logo */
	noaa_logo = XtVaCreateManagedWidget( "noaa_logo",
			xmLabelWidgetClass,
			form1,
			XmNx, 10,
			XmNy, 10,
			XmNlabelType, XmPIXMAP,
			NULL );


	/* Creation of pushButton1 */
	pushButton1 = XtVaCreateManagedWidget( "pushButton1",
			xmPushButtonWidgetClass,
			form1,
			RES_CONVERT( XmNlabelString, "Dismiss" ),
			XmNleftPosition, 50,
			XmNleftOffset, -50,
			XmNleftAttachment, XmATTACH_POSITION,
			XmNbottomAttachment, XmATTACH_FORM,
			XmNbottomOffset, 10,
			XmNresizable, FALSE,
			XmNtopAttachment, XmATTACH_NONE,
			XmNheight, 30,
			XmNwidth, 100,
			XmNx, 130,
			NULL );
	XtAddCallback( pushButton1, XmNactivateCallback,
		(XtCallbackProc) activateCB_pushButton1,
		(XtPointer) NULL );



	/* Creation of author */
	author = XtVaCreateManagedWidget( "author",
			xmLabelWidgetClass,
			form1,
			RES_CONVERT( XmNlabelString, "\n\
Thermal Modeling and Analysis Program (TMAP)\n\
\n\
NOAA/PMEL\n7600 Sand Point Way N.E.\n\
Seattle, WA 98115" ),
			XmNtopOffset, 70,
			XmNtopWidget, NULL,
			XmNtopAttachment, XmATTACH_FORM,
			XmNrightAttachment, XmATTACH_FORM,
			XmNrightOffset, 10,
			XmNleftAttachment, XmATTACH_FORM,
			XmNy, 70,
			XmNleftOffset, 10,
			NULL );


	/* Creation of scrolledWindowText1 */
	scrolledWindowText1 = XtVaCreateManagedWidget( "scrolledWindowText1",
			xmScrolledWindowWidgetClass,
			form1,
			XmNheight, 109,
			XmNleftOffset, 10,
			XmNrightAttachment, XmATTACH_FORM,
			XmNtopAttachment, XmATTACH_WIDGET,
			XmNtopOffset, 10,
			XmNtopWidget, author,
			XmNbottomAttachment, XmATTACH_WIDGET,
			XmNbottomOffset, 10,
			XmNbottomWidget, pushButton1,
			XmNleftAttachment, XmATTACH_FORM,
			XmNrightOffset, 10,
			XmNscrollBarDisplayPolicy, XmSTATIC,
			XmNscrollingPolicy, XmAUTOMATIC,
			XmNtopPosition, 0,
			NULL );


	/* Creation of scrolledText1 */
	scrolledText1 = XtVaCreateManagedWidget( "scrolledText1",
			xmTextWidgetClass,
			scrolledWindowText1,
			XmNwidth, 250,
			XmNheight, 90,
			XmNvalue, "\
This is the Graphical User Interface (GUI) version of Ferret.\n\
\n\
More detailed information regarding Ferret can be found at the\n\
following locations:\n\
\n\
\tFerret home page --\n\
\thttp://www.pmel.noaa.gov/ferret/home.html\n\
\n\
\tFerret release notes --\n\
\thttp://www.pmel.noaa.gov/ferret/release_notes/\n\
\tFerret GUI Users' Guide --\n\
\n\
\thttp://dread.pmel.noaa.gov/~callahan/Ferret_GUI/Users_Guide.html\n\
\n\
\n\
\n\
This is the initial release of the GUI version of Ferret and all\n\
comments, suggestions and bug reports should be made to:\n\
\n\
\thttp://dread.pmel.noaa.gov/~callahan/Ferret_GUI/GUI_response.html\n\
\n\
\n\
Note that the GUI does not have point-and-click access to all of the\n\
features of command-line Ferret.  For this reason we have provided\n\
direct access to a Ferret command-line through the Script Manager\n\
Interface under the \"Scripts\" menu.\
",
			XmNeditable, FALSE,
			NULL );



	return ( Splash );
}

/*******************************************************************************
       The following is the 'Interface function' which is the
       external entry point for creating this interface.
       This function should be called from your application or from
       a callback function.
*******************************************************************************/

Widget	create_Splash( swidget _UxUxParent )
{
	Widget                  rtrn;

	UxParent = _UxUxParent;

	{
		if (gSavedSplash == NULL) {
		rtrn = _Uxbuild_Splash();

		}
		else
			rtrn = gSavedSplash;
		
		XtPopup(UxGetWidget(rtrn), no_grab);
		
		/* set the hi rez size */
		if (gHiRez && !gSavedSplash) {
			Dimension width, height;
				
			XtVaGetValues(UxGetWidget(form1),
				XmNwidth, &width,
				XmNheight, &height,
				NULL);
			width = 1.2 * width;
			height = 1.1 * height;
				
			XtVaSetValues(UxGetWidget(form1),
				XmNwidth, width,
				XmNheight, height,
				NULL);
		}
		if (!gSavedSplash)
			gSavedSplash = rtrn;
		return(rtrn);
	}
}

/*******************************************************************************
       END OF FILE
*******************************************************************************/

