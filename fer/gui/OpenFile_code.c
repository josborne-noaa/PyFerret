/* 
 * OpenFile_code.c
 *
 * John Osborne,
 * Jonathan Callahan
 * Apr 4th 1996
 *
 * This file contains the auxiliary functions which are included by
 * OpenFile.c.
 *
 * 96.12.12 Removed JC_II_NewDataset in OpenOK().  All synching happens in ferret_command.
 */

/* .................... Function Definitions .................... */


static void ActivateCB(wid, clientData, cbArg)
Widget wid;
XtPointer clientData, cbArg;
{
	char *valText = XmTextFieldGetString(wid);

	if (strlen(valText))
		strcpy(DSText, valText);
	else {
		XBell(XtDisplay(UxGetWidget(FerretMainWd)), 50);
		XmTextSetString(wid, DSText);
	}
}


static void InitialList()
{
     XmString motif_string;
     int i=0;

     list_mvfront(GLOBAL_DatasetNameList);

     for ( i=0; i<list_size(GLOBAL_DatasetNameList); i++, list_mvnext(GLOBAL_DatasetNameList) ) {
	  motif_string = XmStringCreateSimple(list_curr(GLOBAL_DatasetNameList));
	  XmListAddItem(UxGetWidget(scrolledList1), motif_string, 0);
     }

     XmListSelectPos(UxGetWidget(scrolledList1), 1, True);

     MaintainBtns();
}

static void MaintainBtns()
{
	if (strlen(DSText))
		XtSetSensitive(pushButton2, True);
	else
		XtSetSensitive(pushButton2, False);
}

static void ListBrowserCB(Widget wid, XtPointer client_data, XtPointer call_data)
{
	char *tempText;
	XmListCallbackStruct *cbs = (XmListCallbackStruct *) call_data;
	
	/* get text selection from list */
	XmStringGetLtoR(cbs->item, XmSTRING_DEFAULT_CHARSET, &tempText);

	/* construct DSText */
	strcpy(DSText, tempText);

	/* put this into edit text field */
	XmTextFieldSetString(UxGetWidget(textField1), DSText);
	
	MaintainBtns();
	XtFree(tempText); /* allocated with XmStringGetLtoR() */
}

static void CancelOpen()
{
  	XtDestroyWidget(UxGetWidget(OpenFile));
}

static void OpenOK()
{
	char cmd[80];

	if ( JC_String_EndsWithTag(DSText, ".cdf") || JC_String_EndsWithTag(DSText, ".nc"))
	  sprintf(cmd, "USE %s", DSText);
	else 
	  sprintf(cmd, "SET DATA %s", DSText);

	ferret_command(cmd, IGNORE_COMMAND_WIDGET);

  	XtDestroyWidget(UxGetWidget(OpenFile));
}

/*******************************************************************************
       The following are callback functions.
*******************************************************************************/
/*
static	void	destroyCB_OpenFile(
			Widget wgt, 
			XtPointer cd, 
			XtPointer cb)
{
	Widget                  UxWidget = wgt;
	XtPointer               UxClientData = cd;
	XtPointer               UxCallbackArg = cb;
	gSavedOpenFile = NULL;
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
	OpenOK();
	}
}

static	void	activateCB_pushButton3(
			Widget wgt, 
			XtPointer cd, 
			XtPointer cb)
{
	Widget                  UxWidget = wgt;
	XtPointer               UxClientData = cd;
	XtPointer               UxCallbackArg = cb;
	{
	XtDestroyWidget(UxGetWidget(OpenFile));
	}
}

static	void	activateCB_pushButton2(
			Widget wgt, 
			XtPointer cd, 
			XtPointer cb)
{
	Widget                  UxWidget = wgt;
	XtPointer               UxClientData = cd;
	XtPointer               UxCallbackArg = cb;
	{
	Open_Save_dset = create_Open_Save_dset(NO_PARENT);
	XtPopup(UxGetWidget(Open_Save_dset), no_grab);
	
	
	}
}

static	void	activateCB_textField1(
			Widget wgt, 
			XtPointer cd, 
			XtPointer cb)
{
	Widget                  UxWidget = wgt;
	XtPointer               UxClientData = cd;
	XtPointer               UxCallbackArg = cb;
	{
	ActivateCB(UxWidget, UxClientData, UxCallbackArg);
	OpenOK();
	
	}
}

static	void	losingFocusCB_textField1(
			Widget wgt, 
			XtPointer cd, 
			XtPointer cb)
{
	Widget                  UxWidget = wgt;
	XtPointer               UxClientData = cd;
	XtPointer               UxCallbackArg = cb;
	{
	ActivateCB(UxWidget, UxClientData, UxCallbackArg);
	
	}
}

static	void	singleSelectionCB_scrolledList1(
			Widget wgt, 
			XtPointer cd, 
			XtPointer cb)
{
	Widget                  UxWidget = wgt;
	XtPointer               UxClientData = cd;
	XtPointer               UxCallbackArg = cb;
	{
	ListBrowserCB(UxWidget, UxClientData, UxCallbackArg);
	}
}

static	void	defaultActionCB_scrolledList1(
			Widget wgt, 
			XtPointer cd, 
			XtPointer cb)
{
	Widget                  UxWidget = wgt;
	XtPointer               UxClientData = cd;
	XtPointer               UxCallbackArg = cb;
	{
	ListBrowserCB(UxWidget, UxClientData, UxCallbackArg);
	OpenOK();
	}
}

*/
