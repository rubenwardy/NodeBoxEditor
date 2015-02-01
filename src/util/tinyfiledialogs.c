/*
tinyfiledialogs.c
Unique code file of tiny file dialogs library - tinyfd
created [November 9, 2014]
Copyright (c) 2014 - 2015 Guillaume Vareille http://ysengrin.com
http://tinyfiledialogs.sourceforge.net

tiny file dialogs - tinyfd - version 1.7.0 [January 30, 2015] zlib licence.
Cross-platform dialogs in C/C++ WINDOWS OSX GNOME KDE SOLARIS CONSOLE
Tested with C & C++ compilers
on Visual Studio 2013 OSX Linux Freebsd Illumos Solaris.

A single C file (add it to your project) with 6 modal function calls:
- open file dialog (& multiple files)
- save file dialog
- select folder dialog
- message box (& question)
- input box
- color picker.

Conceived as a fully independent complement to GLUT, GLUI, SDL, UNITY3D
or any GUI-less program, there NO MAIN LOOP nor init.
It also provides CONSOLE dialogs on unix.

On Windows native code creates the dialogs (mostly).
On UNIX it tries successive command line calls:
- zenity
- kdialog
- applescript
- python 2 / tkinter
- dialog.
The same executable can run across desktops and distributions.

- License -

This software is provided 'as-is', without any express or implied
warranty.  In no event will the authors be held liable for any damages
arising from the use of this software.

Permission is granted to anyone to use this software for any purpose,
including commercial applications, and to alter it and redistribute it
freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not
claim that you wrote the original software.  If you use this software
in a product, an acknowledgment in the product documentation would be
appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be
misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
*/


#ifdef _WIN32
 #include <Windows.h>
 #include <Shlobj.h>
#else
 #include <string.h>
 #include <limits.h>
 #include <unistd.h>
 #include <dirent.h> /* on old system try <sys/dir.h> instead */
 #include <stdlib.h> /* for freebsd */
#endif /* _WIN32 */

#include <stdio.h>

#include "tinyfiledialogs.h" /* not needed */

#define MAX_PATH_OR_CMD 1024 /* _MAX_PATH or MAX_PATH */
#define MAX_MULTIPLE 32

int tinyfd_forceConsole = 0 ; /* for UNIX only: 0 (default) or 1 */
/* 1 forces all dialogs into console mode even when the X server is present */
/* can be modified at run time */

static char * getPathWithoutFinalSlash(
	char * const aoDestination, /* make sure it is allocated, use _MAX_PATH */
	char const * const aSource) /* aoDestination and aSource can be the same */
{
	char const * lTmp ;
	if ( aSource )
	{
		lTmp = strrchr(aSource, (int)* "/");
		if (!lTmp)
		{
			lTmp = strrchr(aSource, (int)* "\\");
		}
		if (lTmp)
		{
			strncpy(aoDestination, aSource, lTmp - aSource);
			aoDestination[lTmp - aSource] = '\0';
		}
		else
		{
			* aoDestination = '\0';
		}
	}
	else
	{
		* aoDestination = '\0';
	}
	return aoDestination;
}


static char * getLastName(
	char * const aoDestination, /* make sure it is allocated */
	char const * const aSource)
{
	/* copy the last name after '/' or '\' */
	char const * lTmp ;
	if ( aSource )
	{
		lTmp = strrchr(aSource, (int)* "/");
		if (!lTmp)
		{
			lTmp = strrchr(aSource, (int)* "\\");
		}
		if (lTmp)
		{
			strcpy(aoDestination, lTmp + 1);
		}
		else
		{
			strcpy(aoDestination, aSource);
		}
	}
	else
	{
		* aoDestination = '\0';
	}
	return aoDestination;
}


static void Hex2RGB( char const aHexRGB [ 8 ] ,
					 unsigned char aoResultRGB [ 3 ] )
{
	char lColorChannel [ 8 ] ;
	strcpy(lColorChannel, aHexRGB ) ;
	aoResultRGB[2] = (unsigned char)strtoul(lColorChannel + 5, NULL, 16);
	lColorChannel[5] = '\0';
	aoResultRGB[1] = (unsigned char)strtoul(lColorChannel + 3, NULL, 16);
	lColorChannel[3] = '\0';
	aoResultRGB[0] = (unsigned char)strtoul(lColorChannel + 1, NULL, 16);
	/* printf("%d %d %d\n", aoResultRGB[0], aoResultRGB[1], aoResultRGB[2]);//*/
}

static void RGB2Hex( unsigned char const aRGB [ 3 ] ,
					 char aoResultHexRGB [ 8 ] )
{
	sprintf(aoResultHexRGB, "#%02hhx%02hhx%02hhx", aRGB[0], aRGB[1], aRGB[2]);
	/* printf("aoResultHexRGB %s\n", aoResultHexRGB); //*/
}


#ifdef _WIN32


/* returns 0 for cancel/no , 1 for ok/yes */
int tinyfd_messageBox (
    char const * const aTitle , /* NULL or "" */
    char const * const aMessage , /* NULL or ""  may contain \n and \t */
    char const * const aDialogType , /* "ok" "okcancel" "yesno" */
    char const * const aIconType , /* "info" "warning" "error" "question" */
    int const aDefaultButton ) /* 0 for cancel/no , 1 for ok/yes */
{
	int lBoxReturnValue;
    UINT aCode ;

    if ( ! strcmp( "warning" , aIconType ) )
    {
        aCode = MB_ICONWARNING ;
    }
	else if (!strcmp("error", aIconType))
    {
        aCode = MB_ICONERROR ;
    }
	else if (!strcmp("question", aIconType))
    {
        aCode = MB_ICONQUESTION ;
    }
    else
    {
        aCode = MB_ICONINFORMATION ;
    }

    if ( ! strcmp( "okcancel" , aDialogType ) )
    {
        aCode += MB_OKCANCEL ;
		if ( ! aDefaultButton )
		{
			aCode += MB_DEFBUTTON2 ;
		}
    }
    else if ( ! strcmp( "yesno" , aDialogType ) )
    {
        aCode += MB_YESNO ;
		if ( ! aDefaultButton )
		{
			aCode += MB_DEFBUTTON2 ;
		}
    }
    else
    {
        aCode += MB_OK ;
    }


	lBoxReturnValue = MessageBox(NULL, aMessage, aTitle, aCode);
	if ( ( ! strcmp("ok", aDialogType) )
	  || (lBoxReturnValue == IDOK)
	  || (lBoxReturnValue == IDYES) )
	{
		return 1 ;
	}
	else
	{
		return 0 ;
	}
}


char const * tinyfd_saveFileDialog (
    char const * const aTitle , /* NULL or "" */
    char const * const aDefaultPathAndFile , /* NULL or "" */
    int const aNumOfFileFilters , /* 0 */
    char const * const * const aFileFilters ) /* NULL or {"*.jpg","*.png"} */
{
    static char lBuff [ MAX_PATH_OR_CMD ] ;
	char lDirname [ MAX_PATH_OR_CMD ] ;
	char lFileFilters[MAX_PATH_OR_CMD] = "";
	char lString[MAX_PATH_OR_CMD] ;
    int i ;
    char * p;
	OPENFILENAME ofn ;

    getPathWithoutFinalSlash(lDirname, aDefaultPathAndFile);
	getLastName(lBuff, aDefaultPathAndFile);

    if (aNumOfFileFilters > 0)
    {
        strcat(lFileFilters, aFileFilters[0]);
        for (i = 1; i < aNumOfFileFilters; i++)
        {
            strcat(lFileFilters, ";");
            strcat(lFileFilters, aFileFilters[i]);
        }
        strcat(lFileFilters, "\n");
        strcpy(lString, lFileFilters);
        strcat(lFileFilters, lString);
        strcat(lFileFilters, "All Files\n*.*\n");
        p = lFileFilters;
        while ((p = strchr(p, '\n')) != NULL)
        {
            *p = '\0';
            p ++ ;
        }
    }

	ofn.lStructSize     = sizeof(OPENFILENAME) ;
	ofn.hwndOwner       = 0 ;
	ofn.hInstance       = 0 ;
	ofn.lpstrFilter     = lFileFilters ;
	ofn.lpstrCustomFilter = NULL ;
	ofn.nMaxCustFilter  = 0 ;
	ofn.nFilterIndex    = 0 ;
	ofn.lpstrFile		= lBuff;
	ofn.nMaxFile        = MAX_PATH_OR_CMD ;
	ofn.lpstrFileTitle  = NULL ;
	ofn.nMaxFileTitle   = _MAX_FNAME + _MAX_EXT ;
	ofn.lpstrInitialDir = lDirname;
	ofn.lpstrTitle      = aTitle ;
	ofn.Flags           = OFN_OVERWRITEPROMPT ;
	ofn.nFileOffset     = 0 ;
	ofn.nFileExtension  = 0 ;
	ofn.lpstrDefExt     = NULL ;
	ofn.lCustData       = 0L ;
	ofn.lpfnHook        = NULL ;
	ofn.lpTemplateName  = NULL ;

	if ( GetSaveFileName ( & ofn ) == 0 )
    {
		return NULL ;
    }
	else
    {
		return lBuff ;
	}
}


/* in case of multiple files, the separator is | */
char const * tinyfd_openFileDialog (
    char const * const aTitle , /*  NULL or "" */
    char const * const aDefaultPathAndFile , /*  NULL or "" */
    int const aNumOfFileFilters , /* 0 */
    char const * const * const aFileFilters , /* NULL or {"*.jpg","*.png"} */
    int aAllowMultipleSelects ) /* 0 or 1 */
{
	static char lBuff[MAX_MULTIPLE * MAX_PATH_OR_CMD];
	char lDirname [ MAX_PATH_OR_CMD ] ;
	char lFileFilters[MAX_PATH_OR_CMD] = "";
	char lTempString[MAX_PATH_OR_CMD] ;
	char * lPointers[MAX_MULTIPLE];
	size_t lLengths[MAX_MULTIPLE];
	int i , j ;
	char * p;
	OPENFILENAME ofn;
    size_t lBuffLen ;

    getPathWithoutFinalSlash(lDirname, aDefaultPathAndFile);
	getLastName(lBuff, aDefaultPathAndFile);

	if (aNumOfFileFilters > 0)
	{
		strcat(lFileFilters, aFileFilters[0]);
		for (i = 1; i < aNumOfFileFilters; i++)
		{
			strcat(lFileFilters, ";");
			strcat(lFileFilters, aFileFilters[i]);
		}
		strcat(lFileFilters, "\n");
		strcpy(lTempString, lFileFilters);
		strcat(lFileFilters, lTempString);
		strcat(lFileFilters, "All Files\n*.*\n");
		p = lFileFilters;
		while ((p = strchr(p, '\n')) != NULL)
		{
			*p = '\0';
			p ++ ;
		}
	}

	ofn.lStructSize     = sizeof ( OPENFILENAME ) ;
	ofn.hwndOwner       = 0 ;
	ofn.hInstance       = 0 ;
	ofn.lpstrFilter		= lFileFilters;
	ofn.lpstrCustomFilter = NULL ;
	ofn.nMaxCustFilter  = 0 ;
	ofn.nFilterIndex    = 0 ;
	ofn.lpstrFile		= lBuff ;
	ofn.nMaxFile        = MAX_PATH_OR_CMD ;
	ofn.lpstrFileTitle  = NULL ;
	ofn.nMaxFileTitle   = _MAX_FNAME + _MAX_EXT ;
	ofn.lpstrInitialDir = lDirname ;
	ofn.lpstrTitle      = aTitle ;
	ofn.Flags			= OFN_EXPLORER ;
	ofn.nFileOffset     = 0 ;
	ofn.nFileExtension  = 0 ;
	ofn.lpstrDefExt     = NULL ;
	ofn.lCustData       = 0L ;
	ofn.lpfnHook        = NULL ;
	ofn.lpTemplateName  = NULL ;

	if ( aAllowMultipleSelects )
	{
		ofn.Flags |= OFN_ALLOWMULTISELECT;
	}

	if ( GetOpenFileName ( & ofn ) == 0 )
    {
		return NULL ;
    }
	else
    {
        lBuffLen = strlen(lBuff) ;
        lPointers[0] = lBuff + lBuffLen + 1 ;
		if ( !aAllowMultipleSelects || (lPointers[0][0] == '\0')  )
            return lBuff ;

        i = 0 ;
        do
        {
			lLengths[i] = strlen(lPointers[i]);
            lPointers[i+1] = lPointers[i] + lLengths[i] + 1 ;
            i ++ ;
        }
        while ( lPointers[i][0] != '\0' );
		i--;
        p = lBuff + sizeof(lBuff) - 1 ;
        * p = '\0';
        for ( j = i ; j >=0 ; j-- )
        {
            p -= lLengths[j];
			memcpy(p, lPointers[j], lLengths[j]);
			p--;
            *p = '\\';
            p -= lBuffLen ;
			memcpy(p, lBuff, lBuffLen);
            p--;
            *p = '|';
        }
		p++;
		return p ;
	}
}


char const * tinyfd_selectFolderDialog (
	char const * const aTitle , /*  NULL or "" */
	char const * const aDefaultPath ) /* NULL or "" */
{
	static char lBuff [ MAX_PATH_OR_CMD ] ;
	BROWSEINFO bInfo ;
	LPITEMIDLIST lpItem ;

	/* we can't use aDefaultPath */
	bInfo.hwndOwner = 0 ;
	bInfo.pidlRoot = NULL ;
	bInfo.pszDisplayName = lBuff ;
	bInfo.lpszTitle = aTitle ;
	bInfo.ulFlags = 0 ;
	bInfo.lpfn = NULL ;
	bInfo.lParam = 0 ;
	bInfo.iImage = -1 ;

	lpItem = SHBrowseForFolder ( & bInfo ) ;
	if ( lpItem )
	{
		SHGetPathFromIDList ( lpItem , lBuff ) ;
	}

	return lBuff ;
}


/* on cancel it returns aDefaultInput */
char const * tinyfd_inputBox(
	char const * const aTitle , /* NULL or "" */
	char const * const aMessage , /* NULL or ""  may NOT contain \n nor \t */
	char const * const aDefaultInput ) /* "" */
{
	static char lBuff[MAX_PATH_OR_CMD];
	char lDialogString[MAX_PATH_OR_CMD];
	FILE * lIn ;
	int lResult;
	lBuff[0]='\0';

	sprintf(lDialogString, "%s\\AppData\\Local\\Temp\\tinyfdInputBox.vbs",
			getenv("USERPROFILE"));
	lIn = fopen(lDialogString, "w");
	if (!lIn)
	{
		return NULL;
	}
	strcpy(lDialogString, "Dim result:result=InputBox(\"");
	//strcpy(lDialogString, "Dim result:result=InputBox(\"");
	if (aMessage && strlen(aMessage))
	{
		strcat(lDialogString, aMessage);
	}
	strcat(lDialogString, "\",\"");
	if (aTitle && strlen(aTitle))
	{
		strcat(lDialogString, aTitle);
	}
	strcat(lDialogString, "\",\"");
	if ( aDefaultInput && strlen(aDefaultInput) )
	{
		strcat(lDialogString, aDefaultInput);
	}
	strcat(lDialogString, "\"):If IsEmpty(result) then:WScript.Echo 1");
	strcat(lDialogString, ":Else: WScript.Echo \"0\" & result : End If");
	fputs(lDialogString, lIn);

	fclose(lIn);

	strcpy(lDialogString, "C:\\Users\\fantasio\\Desktop\\password.hta");


	strcpy(lDialogString, "powershell -WindowStyle Hidden -Command \"\
cscript.exe %USERPROFILE%\\AppData\\Local\\Temp\\tinyfdInputBox.vbs\"");

//strcpy(lDialogString, "cscript.exe %USERPROFILE%\\AppData\\Local\\Temp\\tinyfdInputBox.vbs");

/*
strcpy(lDialogString, "mshta vbscript:Execute(\"resizeTo 0,0:moveTo -1000,0:\
dim result:result=InputBox(\"\"message\"\", \"\"title\"\", \"\"input\"\"):\
Set objFSO=CreateObject(\"\"Scripting.FileSystemObject\"\"):\
Set objFile=objFSO.CreateTextFile(\
\"\"%USERPROFILE%\\AppData\\Local\\Temp\\tinyfdInputBox.txt\"\",True):\
objFile.Write result & vbCrLf:objFile.Close:window.close\") ");
*/

	/* printf ( "lDialogString: %s\n" , lDialogString ) ; //*/
	if ( ! ( lIn = _popen ( lDialogString , "r" ) ) )
	{
		return NULL ;
	}
	while ( fgets ( lBuff , sizeof ( lBuff ) , lIn ) != NULL )
	{}
	_pclose ( lIn ) ;
	if ( lBuff[ strlen ( lBuff ) -1 ] == '\n' )
	{
		lBuff[ strlen ( lBuff ) -1 ] = '\0' ;
	}
	sprintf(lDialogString, "%s\\AppData\\Local\\Temp\\tinyfdInputBox.vbs",
			getenv("USERPROFILE"));
	remove(lDialogString);
	/* printf ( "lBuff: %s\n" , lBuff ) ; //*/
	lResult = strncmp(lBuff, "0", 1) ? 0 : 1;
	/* printf ( "lResult: %d \n" , lResult ) ; //*/
	if (!lResult)
	{
		if (aDefaultInput)
		{
			strcpy(lBuff + 1, aDefaultInput);
		}
		else
		{
			lBuff[1] = '\0';
		}
	}
	/* printf ( "lBuff+1: %s\n" , lBuff+1 ) ; //*/
	return lBuff + 1;
}


/* returns the hexcolor as a string "#FF0000" */
/* aoResultRGB also contains the result */
/* aDefaultRGB is used only if aDefaultHexRGB is NULL */
/* aDefaultRGB and aoResultRGB can be the same array */
char const * tinyfd_colorChooser(
	char const * const aTitle, /* NULL or "" */
	char const * const aDefaultHexRGB, /* NULL or "#FF0000"*/
	unsigned char aDefaultRGB[3], /* { 0 , 255 , 255 } */
	unsigned char aoResultRGB[3]) /* { 0 , 0 , 0 } */
{
	static CHOOSECOLOR cc;
	static COLORREF crCustColors[16];
	static char lResultHexRGB[8];
	int lRet;

	if ( aDefaultHexRGB )
	{
		Hex2RGB(aDefaultHexRGB, aDefaultRGB);
	}

	/* we can't use aTitle */
	cc.lStructSize = sizeof ( CHOOSECOLOR ) ;
	cc.hwndOwner = NULL ;
	cc.hInstance = NULL ;
	cc.rgbResult = RGB(aDefaultRGB[0], aDefaultRGB[1], aDefaultRGB[2]);
	cc.lpCustColors = crCustColors;
	cc.Flags = CC_RGBINIT | CC_FULLOPEN;
	cc.lCustData = 0;
	cc.lpfnHook = NULL;
	cc.lpTemplateName = NULL;

	lRet = ChooseColor(&cc);

	if ( ! lRet )
	{
		return NULL;
	}

	aoResultRGB[0] = GetRValue(cc.rgbResult);
	aoResultRGB[1] = GetGValue(cc.rgbResult);
	aoResultRGB[2] = GetBValue(cc.rgbResult);

	RGB2Hex(aoResultRGB, lResultHexRGB);

	return lResultHexRGB;
}


#else /* unix */

static char gPython2Name[16];

static char gTitle[] = "missing software !" ;

static char gMessage[] = "tiny file dialogs on UNIX needs\n\t\
zenity (version 3 for the color chooser)\nor\tkdialog\nor\t\
python 2 with tkinter\nor\tdialog (display in console)\n" ;

static int graphicMode()
{
	return ( ! tinyfd_forceConsole ) && getenv ( "DISPLAY" ) ;
}


static int detectPresence ( char const * const aExecutable )
{
	FILE * lIn ;
    char lBuff [ MAX_PATH_OR_CMD ] ;
    char lTestedString [ MAX_PATH_OR_CMD ] = "which " ;
    strcat ( lTestedString , aExecutable ) ;
    lIn = popen ( lTestedString , "r" ) ;
    if ( ( fgets ( lBuff , sizeof ( lBuff ) , lIn ) != NULL )
        && ( ! strchr ( lBuff , ':' ) ) )
    {	/* present */
    	pclose ( lIn ) ;
    	return 1 ;
    }
    else
    {
    	pclose ( lIn ) ;
    	return 0 ;
    }
}


static int tryCommand ( char const * const aCommand )
{
    char lBuff [ MAX_PATH_OR_CMD ] ;
    FILE * lIn = popen ( aCommand , "r" ) ;
    if ( fgets ( lBuff , sizeof ( lBuff ) , lIn ) == NULL )
    {	/* present */
    	pclose ( lIn ) ;
    	return 1 ;
    }
    else
    {
    	pclose ( lIn ) ;
    	return 0 ;
    }

}


static int whiptailPresent ( )
{
    static int lWhiptailPresent = -1 ;
    if ( lWhiptailPresent < 0 )
    {
        lWhiptailPresent = detectPresence ( "whiptail" ) ;
    }
    return lWhiptailPresent && isatty ( 1 ) ; /* verify console presence */
}


static int dialogPresent ( )
{
    static int lDialogPresent = -1 ;

    if ( lDialogPresent < 0 )
    {
        lDialogPresent = detectPresence ( "dialog" ) ;
    }
    return lDialogPresent && isatty ( 1 ) ; /* verify console presence */
}


static int xmessagePresent ( )
{
    static int lXmessagePresent = -1 ;
    if ( lXmessagePresent < 0 )
    {
        lXmessagePresent = detectPresence("xmessage") ;
    }
    return lXmessagePresent && graphicMode ( ) ;
}


static int notifysendPresent ( )
{
    static int lNotifysendPresent = -1 ;
    if ( lNotifysendPresent < 0 )
    {
        lNotifysendPresent = detectPresence("notify-send") ;
    }
    return lNotifysendPresent && graphicMode ( ) ;
}


static int osascriptPresent ( )
{
    static int lOsascriptPresent = -1 ;
    if ( lOsascriptPresent < 0 )
    {
        lOsascriptPresent = detectPresence ( "osascript" ) ;
    }
	return lOsascriptPresent && graphicMode ( ) ;
}


static int kdialogPresent ( )
{
	static int lKdialogPresent = -1 ;
	if ( lKdialogPresent < 0 )
	{
		lKdialogPresent = detectPresence("kdialog") ;
	}
	return lKdialogPresent && graphicMode ( ) ;
}


static int zenityPresent ( )
{
	static int lZenityPresent = -1 ;
	if ( lZenityPresent < 0 )
	{
		lZenityPresent = detectPresence("zenity") ;
	}
	return lZenityPresent && graphicMode ( ) ;
}


static int zenity3Present ( )
{
    static int lZenity3Present = -1 ;
    char lBuff [ MAX_PATH_OR_CMD ] ;
    FILE * lIn ;

	if ( lZenity3Present < 0 )
	{
		if ( ! zenityPresent() )
		{
			lZenity3Present = 0 ;
		}
	 	else
		{
			lIn = popen ( "zenity --version" , "r" ) ;
			if ( ( fgets ( lBuff , sizeof ( lBuff ) , lIn ) != NULL )
			  && ( atoi(lBuff) >= 3 )
			  && ( atoi(strtok(lBuff,".")+1) >= 0 ) )
			{
				lZenity3Present = 1 ;
			}
			else
			{
				lZenity3Present = 0 ;
			}
			pclose ( lIn ) ;
		}
	}
    return lZenity3Present && graphicMode ( ) ;
}


static int tkinter2Present ( )
{
    static int lTkinter2Present = -1 ;
	char lPythonCommand[256];
	char lPythonParams[256] =
"-c \"try:\n\timport Tkinter;\nexcept:\n\tprint(0);\"";
	int i;

    if ( lTkinter2Present < 0 )
    {
		strcpy(gPython2Name , "python" ) ;
		sprintf ( lPythonCommand , "%s %s" , gPython2Name , lPythonParams ) ;
	    lTkinter2Present = tryCommand(lPythonCommand);
        if ( ! lTkinter2Present )
	    {
			strcpy(gPython2Name , "python2" ) ;
			if ( detectPresence(gPython2Name) )
			{
sprintf ( lPythonCommand , "%s %s" , gPython2Name , lPythonParams ) ;
				lTkinter2Present = tryCommand(lPythonCommand);
			}
			else
			{
				for ( i = 9 ; i >= 0 ; i -- )
				{
					sprintf ( gPython2Name , "python2.%d" , i ) ;
					if ( detectPresence(gPython2Name) )
					{
sprintf ( lPythonCommand , "%s %s" , gPython2Name , lPythonParams ) ;
						lTkinter2Present = tryCommand(lPythonCommand);
						break ;
					}
				}
			}
	    }
    }
    /* printf ("gPython2Name %s\n", gPython2Name) ; //*/
    return lTkinter2Present && graphicMode ( ) ;
}


static int preferPython ( )
{
	static int lsPreferPython = -1 ;
	int lpreferPython = 0 ; /* set to 1 to prefer python, 0 otherwise */
	if ( lsPreferPython < 0 )
	{
		lsPreferPython = lpreferPython && tkinter2Present ( ) ;
	}
	return lsPreferPython ;
}


static void replaceSubStr ( char const * const aSource ,
						    char const * const aOldSubStr ,
						    char const * const aNewSubStr ,
						    char * const aoDestination )
{
	char const * pOccurence ;
	char const * p ;
	char const * lNewSubStr = "" ;

	if ( ! aSource )
	{
		* aoDestination = '\0' ;
		return ;
	}
	if ( ! aOldSubStr )
	{
		strcpy ( aoDestination , aSource ) ;
		return ;
	}
	if ( aNewSubStr )
	{
		lNewSubStr = aNewSubStr ;
	}
	p = aSource ;
	int lOldSubLen = strlen ( aOldSubStr ) ;
	* aoDestination = '\0' ;
	while ( ( pOccurence = strstr ( p , aOldSubStr ) ) != NULL )
	{
		strncat ( aoDestination , p , pOccurence - p ) ;
		strcat ( aoDestination , lNewSubStr ) ;
		p = pOccurence + lOldSubLen ;
	}
	strcat ( aoDestination , p ) ;
}


/* returns 0 for cancel/no , 1 for ok/yes */
int tinyfd_messageBox (
    char const * const aTitle , /* NULL or "" */
    char const * const aMessage , /* NULL or ""  may contain \n and \t */
    char const * const aDialogType , /* "ok" "okcancel" "yesno"*/
    char const * const aIconType , /* "info" "warning" "error" "question" */
    int const aDefaultButton ) /* 0 for cancel/no , 1 for ok/yes */
{
    char lBuff [ MAX_PATH_OR_CMD ] ;
    char lDialogString [ MAX_PATH_OR_CMD ] ;
    FILE * lIn ;
    int lResult ;
    lBuff[0]='\0';

    if ( zenityPresent() )
    {
        strcpy ( lDialogString , "zenity --" ) ;
        if ( ! strcmp( "okcancel" , aDialogType ) )
        {
            strcat ( lDialogString ,
            		"question --ok-label=Ok --cancel-label=Cancel" ) ;
        }
        else if ( ! strcmp( "yesno" , aDialogType ) )
        {
            strcat ( lDialogString , "question" ) ;
        }
        else if ( ! strcmp( "error" , aIconType ) )
		{
            strcat ( lDialogString , "error" ) ;
        }
        else if ( ! strcmp( "warning" , aIconType ) )
		{
            strcat ( lDialogString , "warning" ) ;
        }
        else
		{
            strcat ( lDialogString , "info" ) ;
        }
		if ( aTitle && strlen(aTitle) )
		{
			strcat(lDialogString, " --title=\"") ;
			strcat(lDialogString, aTitle) ;
			strcat(lDialogString, "\"") ;
		}
		if ( aMessage && strlen(aMessage) )
		{
			strcat(lDialogString, " --text=\"") ;
			strcat(lDialogString, aMessage) ;
			strcat(lDialogString, "\"") ;
		}
		if ( zenity3Present ( ) )
		{
			strcat ( lDialogString , " --icon-name=dialog-" ) ;
			if ( ! strcmp( "question" , aIconType )
			  || ! strcmp( "error" , aIconType )
			  || ! strcmp( "warning" , aIconType ) )
			{
				strcat ( lDialogString , aIconType ) ;
			}
			else
			{
				strcat ( lDialogString , "info" ) ;
			}
		}
        strcat ( lDialogString , ";echo $?" ) ;
    }
	else if ( kdialogPresent() )
	{
		strcpy ( lDialogString , "kdialog --" ) ;
		if ( ( ! strcmp( "okcancel" , aDialogType ) )
		  || ( ! strcmp( "yesno" , aDialogType ) ) )
		{
			if ( ( ! strcmp( "warning" , aIconType ) )
			  || ( ! strcmp( "error" , aIconType ) ) )
			{
				strcat ( lDialogString , "warning" ) ;
			}
			strcat ( lDialogString , "yesno" ) ;
		}
		else if ( ! strcmp( "error" , aIconType ) )
		{
			strcat ( lDialogString , "error" ) ;
		}
		else if ( ! strcmp( "warning" , aIconType ) )
		{
			strcat ( lDialogString , "sorry" ) ;
		}
		else
		{
			strcat ( lDialogString , "msgbox" ) ;
		}
		strcat ( lDialogString , " \"" ) ;
		if ( aMessage )
		{
			strcat ( lDialogString , aMessage ) ;
		}
		strcat ( lDialogString , "\"" ) ;
		if ( ! strcmp( "okcancel" , aDialogType ) )
		{
			strcat ( lDialogString ,
				" --yes-label Ok --no-label Cancel" ) ;
		}
		if ( aTitle && strlen(aTitle) )
		{
			strcat(lDialogString, " --title \"") ;
			strcat(lDialogString, aTitle) ;
			strcat(lDialogString, "\"") ;
		}
		strcat ( lDialogString , ";echo $?" ) ;
	}
    else if ( osascriptPresent ( ) )
    {
		strcpy ( lDialogString , "osascript -e 'try' -e 'display dialog \"") ;
	    if ( aMessage && strlen(aMessage) )
	    {
			strcat(lDialogString, aMessage) ;
	    }
		strcat(lDialogString, "\" ") ;
	    if ( aTitle && strlen(aTitle) )
	    {
			strcat(lDialogString, "with title \"") ;
			strcat(lDialogString, aTitle) ;
			strcat(lDialogString, "\" ") ;
	    }
		strcat(lDialogString, "with icon ") ;
		if ( ! strcmp( "error" , aIconType ) )
		{
			strcat(lDialogString, "stop " ) ;
		}
		else if ( ! strcmp( "warning" , aIconType ) )
		{
			strcat(lDialogString, "caution " ) ;
		}
		else /* question or info */
		{
			strcat(lDialogString, "note " ) ;
		}
		if ( ! strcmp( "okcancel" , aDialogType ) )
		{
			if ( ! aDefaultButton )
			{
				strcat ( lDialogString ,"default button \"Cancel\" " ) ;
				strcat ( lDialogString ,"cancel button \"OK\"" ) ;
			}
		}
		else if ( ! strcmp( "yesno" , aDialogType ) )
		{
			strcat ( lDialogString ,"buttons {\"No\", \"Yes\"} " ) ;
			if (aDefaultButton)
			{
				strcat ( lDialogString ,"default button \"Yes\" " ) ;
				strcat ( lDialogString ,"cancel button \"No\"" ) ;
			}
			else
			{
				strcat ( lDialogString ,"default button \"No\" " ) ;
				strcat ( lDialogString ,"cancel button \"Yes\"" ) ;
			}
		}
		else
		{
			strcat ( lDialogString ,"buttons {\"OK\"} " ) ;
			strcat ( lDialogString ,"default button \"OK\" " ) ;
		}
		strcat(lDialogString, "' ") ;
		strcat(lDialogString, "-e '0' " );
		strcat(lDialogString, "-e 'on error number -128' " ) ;
		strcat(lDialogString, "-e '1' " );
		strcat(lDialogString, "-e 'end try'") ;
	}
    else if ( tkinter2Present ( ) )
    {
        strcpy ( lDialogString , gPython2Name ) ;
        if ( ! isatty ( 1 ) )
        {
            strcat ( lDialogString , " -i" ) ;  /* for osx without console */
        }

		strcat ( lDialogString ,
" -c \"import Tkinter,tkMessageBox;root=Tkinter.Tk();root.withdraw();");

		if ( osascriptPresent ( ) )
		{
			strcat ( lDialogString ,
"import os;os.system('''/usr/bin/osascript -e 'tell app \\\"Finder\\\" to set \
frontmost of process \\\"Python\\\" to true' ''');");
		}

		strcat ( lDialogString ,"res=tkMessageBox." ) ;
        if ( ! strcmp( "okcancel" , aDialogType ) )
        {
            strcat ( lDialogString , "askokcancel(" ) ;
            if ( aDefaultButton )
			{
				strcat ( lDialogString , "default=tkMessageBox.OK," ) ;
			}
			else
			{
				strcat ( lDialogString , "default=tkMessageBox.CANCEL," ) ;
			}
        }
        else if ( ! strcmp( "yesno" , aDialogType ) )
        {
            strcat ( lDialogString , "askyesno(" ) ;
            if ( aDefaultButton )
			{
				strcat ( lDialogString , "default=tkMessageBox.YES," ) ;
			}
			else
			{
				strcat ( lDialogString , "default=tkMessageBox.NO," ) ;
			}
        }
        else
        {
            strcat ( lDialogString , "showinfo(" ) ;
        }
        strcat ( lDialogString , "icon='" ) ;
        if ( ! strcmp( "question" , aIconType )
          || ! strcmp( "error" , aIconType )
          || ! strcmp( "warning" , aIconType ) )
        {
            strcat ( lDialogString , aIconType ) ;
        }
        else
        {
            strcat ( lDialogString , "info" ) ;
        }
		strcat(lDialogString, "',") ;
	    if ( aTitle && strlen(aTitle) )
	    {
			strcat(lDialogString, "title='") ;
			strcat(lDialogString, aTitle) ;
			strcat(lDialogString, "',") ;
	    }
		if ( aMessage && strlen(aMessage) )
		{
			replaceSubStr ( aMessage , "\n" , "\\n" , lBuff ) ;
			strcat(lDialogString, "message='") ;
			strcat(lDialogString, lBuff) ;
			strcat(lDialogString, "'") ;
			lBuff[0]='\0';
		}
		strcat(lDialogString, ");\n\
if res==False :\n\tprint 1\n\
else :\n\tprint 0\n\"" ) ;
    }
	else if ( dialogPresent ( ) || whiptailPresent ( ) )
	{
		if ( dialogPresent ( ) )
		{
			strcpy ( lDialogString , "(dialog " ) ;
		}
		else
		{
			strcpy ( lDialogString , "(whiptail " ) ;
		}
 		if ( aTitle && strlen(aTitle) )
		{
			strcat(lDialogString, "--title \"") ;
			strcat(lDialogString, aTitle) ;
			strcat(lDialogString, "\" ") ;
		}
		if ( ! strcmp( "okcancel" , aDialogType ) )
		{
			strcat ( lDialogString ,
					"--yes-label \"Ok\" --no-label \"Cancel\" --yesno " ) ;
			if ( ! aDefaultButton )
			{
				strcat ( lDialogString , "--defaultno " ) ;
			}
		}
		else if ( ! strcmp( "yesno" , aDialogType ) )
		{
			strcat ( lDialogString , "--yesno" ) ;
			if ( ! aDefaultButton )
			{
				strcat ( lDialogString , "--defaultno " ) ;
			}
		}
		else
		{
			strcat ( lDialogString , "--msgbox" ) ;

		}
		strcat ( lDialogString , " \"" ) ;
		if ( aMessage && strlen(aMessage) )
		{
			strcat(lDialogString, aMessage) ;
		}
		strcat(lDialogString,
			"\" 10 60 >/dev/tty) 2>&1 ; echo $? ; clear >/dev/tty") ;
	}
	else if ( notifysendPresent ( ) )
	{
		strcpy ( lDialogString , "notify-send \"" ) ;
		strcat(lDialogString, gTitle) ;
		strcat ( lDialogString , "\n" ) ;
		strcat(lDialogString, gMessage) ;
		strcat ( lDialogString , "\"" ) ;
	}
	else if ( xmessagePresent ( ) )
	{
		strcpy ( lDialogString , "xmessage -center \"");
		strcat(lDialogString, gTitle) ;
		strcat ( lDialogString , "\n" ) ;
		strcat(lDialogString, gMessage) ;
		strcat ( lDialogString , "\"" ) ;
	}
	else
	{
		printf ("\n%s\n\n", gTitle);
		printf ("%s\n", gMessage);
		return 0 ;
	}

	/* printf ( "lDialogString: %s\n" , lDialogString ) ; //*/
    if ( ! ( lIn = popen ( lDialogString , "r" ) ) )
    {
        return 0 ;
    }
	while ( fgets ( lBuff , sizeof ( lBuff ) , lIn ) != NULL )
	{}
	pclose ( lIn ) ;
	/* printf ( "lBuff: %s len: %lu \n" , lBuff , strlen(lBuff) ) ; //*/
    if ( lBuff[ strlen ( lBuff ) -1 ] == '\n' )
    {
    	lBuff[ strlen ( lBuff ) -1 ] = '\0' ;
    }
	/* printf ( "lBuff1: %s len: %lu \n" , lBuff , strlen(lBuff) ) ; //*/
    lResult =  strcmp ( lBuff , "0" ) ? 0 : 1 ;
	/* printf ( "lResult: %d\n" , lResult ) ; //*/
    return lResult ;
}


char const * tinyfd_saveFileDialog (
    char const * const aTitle , /* NULL or "" */
    char const * const aDefaultPathAndFile , /* NULL or "" */
    int const aNumOfFileFilters , /* 0 */
    char const * const * const aFileFilters ) /* NULL or {"*.jpg","*.png"} */
{
    static char lBuff [ MAX_PATH_OR_CMD ] ;
    char lDialogString [ MAX_PATH_OR_CMD ] ;
    char lString [ MAX_PATH_OR_CMD ] ;
	int i ;
    DIR * lDir ;
    FILE * lIn ;
	lBuff[0]='\0';

    if ( zenityPresent() )
    {
        printf("using zenity for saveFileDialog\n");
		strcpy ( lDialogString ,
				"zenity --file-selection --save --confirm-overwrite" ) ;
		if ( aTitle && strlen(aTitle) )
		{
			strcat(lDialogString, " --title=\"") ;
			strcat(lDialogString, aTitle) ;
			strcat(lDialogString, "\"") ;
		}
		if ( aDefaultPathAndFile && strlen(aDefaultPathAndFile) )
		{
			strcat(lDialogString, " --filename=\"") ;
			strcat(lDialogString, aDefaultPathAndFile) ;
			strcat(lDialogString, "\"") ;
		}
		if ( aNumOfFileFilters > 0 )
		{
			for ( i = 0 ; i < aNumOfFileFilters ; i ++ )
			{
                strcat ( lDialogString , " --file-filter='" ) ;
				strcat ( lDialogString , aFileFilters [ i ] ) ;
				strcat ( lDialogString , "'" ) ;
			}
			strcat ( lDialogString , " --file-filter='All files | *'" ) ;
		}
    }
    else if ( kdialogPresent() )
    {
        printf("using kdialog for saveFileDialog\n");
		strcpy ( lDialogString , "kdialog --getsavefilename" ) ;
        if ( aDefaultPathAndFile && strlen(aDefaultPathAndFile) )
        {
			strcat(lDialogString, " \"") ;
			strcat(lDialogString, aDefaultPathAndFile ) ;
			strcat(lDialogString , "\"" ) ;
		}
		else
		{
			strcat(lDialogString, " :" ) ;
		}
	    if ( aNumOfFileFilters > 0 )
	    {
			strcat(lDialogString , " \"" ) ;
			for ( i = 0 ; i < aNumOfFileFilters ; i ++ )
			{
				strcat ( lDialogString , aFileFilters [ i ] ) ;
				strcat ( lDialogString , " " ) ;
			}
			strcat ( lDialogString , "\"" ) ;
	    }
	    if ( aTitle && strlen(aTitle) )
	    {
			strcat(lDialogString, " --title \"") ;
			strcat(lDialogString, aTitle) ;
			strcat(lDialogString, "\"") ;
	    }
    }
    else if ( osascriptPresent ( ) )
    {
        printf("using osascript for saveFileDialog\n");
		strcpy ( lDialogString ,
				"osascript -e 'POSIX path of ( choose file name " );
	    if ( aTitle && strlen(aTitle) )
	    {
			strcat(lDialogString, "with prompt \"") ;
			strcat(lDialogString, aTitle) ;
			strcat(lDialogString, "\" ") ;
	    }
		getPathWithoutFinalSlash ( lString , aDefaultPathAndFile ) ;
		if ( strlen(lString) )
		{
			strcat(lDialogString, "default location \"") ;
			strcat(lDialogString, lString ) ;
			strcat(lDialogString , "\" " ) ;
		}
		getLastName ( lString , aDefaultPathAndFile ) ;
		if ( strlen(lString) )
		{
			strcat(lDialogString, "default name \"") ;
			strcat(lDialogString, lString ) ;
			strcat(lDialogString , "\" " ) ;
		}
		strcat ( lDialogString , ")'" ) ;
    }
    else if ( tkinter2Present ( ) )
    {
        printf("using tkinter for saveFileDialog\n");
		strcpy ( lDialogString , gPython2Name ) ;
		if ( ! isatty ( 1 ) )
		{
			strcat ( lDialogString , " -i" ) ; /* for osx without console */
		}
	    strcat ( lDialogString ,
" -c \"import Tkinter,tkFileDialog;root=Tkinter.Tk();root.withdraw();");

    	if ( osascriptPresent ( ) )
    	{
			strcat ( lDialogString ,
"import os;os.system('''/usr/bin/osascript -e 'tell app \\\"Finder\\\" to set \
frontmost of process \\\"Python\\\" to true' ''');");
		}

		strcat ( lDialogString , "print tkFileDialog.asksaveasfilename(");
		if ( aTitle && strlen(aTitle) )
		{
			strcat(lDialogString, "title='") ;
			strcat(lDialogString, aTitle) ;
			strcat(lDialogString, "',") ;
		}
	    if ( aDefaultPathAndFile && strlen(aDefaultPathAndFile) )
	    {
			getPathWithoutFinalSlash ( lString , aDefaultPathAndFile ) ;
			if ( strlen(lString) )
			{
				strcat(lDialogString, "initialdir='") ;
				strcat(lDialogString, lString ) ;
				strcat(lDialogString , "'," ) ;
			}
			getLastName ( lString , aDefaultPathAndFile ) ;
			if ( strlen(lString) )
			{
				strcat(lDialogString, "initialfile='") ;
				strcat(lDialogString, lString ) ;
				strcat(lDialogString , "'," ) ;
			}
		}
	    if ( ( aNumOfFileFilters > 1 )
		  || ( ( aNumOfFileFilters == 1 ) /* test because poor osx behaviour */
	            && ( aFileFilters[0][strlen(aFileFilters[0])-1] != '*' ) ) )
	    {
			strcat(lDialogString , "filetypes=(" ) ;
	        for ( i = 0 ; i < aNumOfFileFilters ; i ++ )
	        {
	            strcat ( lDialogString , "('','" ) ;
	            strcat ( lDialogString , aFileFilters [ i ] ) ;
	            strcat ( lDialogString , "')," ) ;
	        }
	        strcat ( lDialogString , "('All files','*'))" ) ;
	    }
		strcat ( lDialogString , ")\"" ) ;
	}
	else if ( dialogPresent ( ) )
	{
        printf("using dialog for saveFileDialog\n");
		strcpy ( lDialogString , "(dialog " ) ;
 		if ( aTitle && strlen(aTitle) )
		{
			strcat(lDialogString, "--title \"") ;
			strcat(lDialogString, aTitle) ;
			strcat(lDialogString, "\" ") ;
		}
		strcat ( lDialogString , "--fselect \"" ) ;
		if ( aDefaultPathAndFile && strlen(aDefaultPathAndFile) )
		{
			strcat(lDialogString, aDefaultPathAndFile) ;
		}
		strcat(lDialogString, "\" 0 60 >/dev/tty) 2>&1 ; clear >/dev/tty") ;
	}
	else
	{
		tinyfd_messageBox (gTitle, gMessage,"ok","error",1 ) ;
		return NULL ;
	}
	/* printf ( "lDialogString: %s\n" , lDialogString ) ; //*/
    if ( ! ( lIn = popen ( lDialogString , "r" ) ) )
    {
        return NULL ;
    }
    while ( fgets ( lBuff , sizeof ( lBuff ) , lIn ) != NULL )
    {}
    pclose ( lIn ) ;
    if ( lBuff[ strlen ( lBuff ) -1 ] == '\n' )
    {
    	lBuff[ strlen ( lBuff ) -1 ] = '\0' ;
    }
	/* printf ( "lBuff: %s\n" , lBuff ) ; //*/
    getPathWithoutFinalSlash ( lString , lBuff ) ;
    if ( strlen ( lString ) > 0 )
    {
        lDir = opendir ( lString ) ;
        if ( ! lDir )
        {
            return NULL ;
        }
        closedir ( lDir ) ;
    }
    return lBuff ;
}


/* in case of multiple files, the separator is | */
char const * tinyfd_openFileDialog (
    char const * const aTitle , /* NULL or "" */
    char const * const aDefaultPathAndFile , /* NULL or "" */
    int const aNumOfFileFilters , /* 0 */
    char const * const * const aFileFilters , /* NULL or {"*.jpg","*.png"} */
    int aAllowMultipleSelects ) /* 0 or 1 */
{
    static char lBuff [ MAX_MULTIPLE * MAX_PATH_OR_CMD ] ;
    char lDialogString [ MAX_PATH_OR_CMD ] ;
	char lString [ MAX_PATH_OR_CMD ] ;
	int i ;
    FILE * lIn ;
	char * p ;
	int lWasKdialog = 0 ;
    lBuff[0]='\0';

    if ( zenityPresent() )
    {
        strcpy ( lDialogString ,"zenity --file-selection" ) ;
		if ( aAllowMultipleSelects )
		{
			strcat ( lDialogString , " --multiple" ) ;
		}
		if ( aTitle && strlen(aTitle) )
		{
			strcat(lDialogString, " --title=\"") ;
			strcat(lDialogString, aTitle) ;
			strcat(lDialogString, "\"") ;
		}
		if ( aDefaultPathAndFile && strlen(aDefaultPathAndFile) )
		{
			strcat(lDialogString, " --filename=\"") ;
			strcat(lDialogString, aDefaultPathAndFile) ;
			strcat(lDialogString, "\"") ;
		}
        if ( aNumOfFileFilters > 0 )
        {
	        strcat ( lDialogString , " --file-filter='" ) ;
            for ( i = 0 ; i < aNumOfFileFilters ; i ++ )
            {
                strcat ( lDialogString , aFileFilters [ i ] ) ;
                strcat ( lDialogString , " " ) ;
            }
 	        strcat ( lDialogString , "' --file-filter='All files | *'" ) ;
        }
    }
	else if ( kdialogPresent() )
	{
		lWasKdialog = 1 ;
		strcpy ( lDialogString , "kdialog --getopenfilename" ) ;
		if ( aDefaultPathAndFile && strlen(aDefaultPathAndFile) )
		{
			strcat(lDialogString, " \"") ;
			strcat(lDialogString, aDefaultPathAndFile ) ;
			strcat(lDialogString , "\"" ) ;
		}
		else
		{
			strcat(lDialogString, " :" ) ;
		}
		if ( aNumOfFileFilters > 0 )
		{
			strcat(lDialogString , " \"" ) ;
			for ( i = 0 ; i < aNumOfFileFilters ; i ++ )
			{
				strcat ( lDialogString , aFileFilters [ i ] ) ;
				strcat ( lDialogString , " " ) ;
			}
			strcat ( lDialogString , "\"" ) ;
		}
		if ( aAllowMultipleSelects )
		{
			strcat ( lDialogString , " --multiple --separate-output" ) ;
		}
		if ( aTitle && strlen(aTitle) )
		{
			strcat(lDialogString, " --title \"") ;
			strcat(lDialogString, aTitle) ;
			strcat(lDialogString, "\"") ;
		}
	}
    else if ( osascriptPresent ( ) )
    {
		strcpy ( lDialogString , "osascript -e '" );
	    if ( ! aAllowMultipleSelects )
	    {
			strcat ( lDialogString , "POSIX path of ( " );
		}
		else
		{
			strcat ( lDialogString , "set mylist to " );
		}
		strcat ( lDialogString , "choose file " );
	    if ( aTitle && strlen(aTitle) )
	    {
			strcat(lDialogString, "with prompt \"") ;
			strcat(lDialogString, aTitle) ;
			strcat(lDialogString, "\" ") ;
	    }
		getPathWithoutFinalSlash ( lString , aDefaultPathAndFile ) ;
		if ( strlen(lString) )
		{
			strcat(lDialogString, "default location \"") ;
			strcat(lDialogString, lString ) ;
			strcat(lDialogString , "\" " ) ;
		}
		if ( aNumOfFileFilters > 0 )
		{
			strcat(lDialogString , "of type {\"" );
			strcat ( lDialogString , aFileFilters [ 0 ] + 2 ) ;
			strcat ( lDialogString , "\"" ) ;
			for ( i = 1 ; i < aNumOfFileFilters ; i ++ )
			{
				strcat ( lDialogString , ",\"" ) ;
				strcat ( lDialogString , aFileFilters [ i ] + 2) ;
				strcat ( lDialogString , "\"" ) ;
			}
			strcat ( lDialogString , "} " ) ;
		}
		if ( aAllowMultipleSelects )
		{
			strcat ( lDialogString , "multiple selections allowed true ' " ) ;
			strcat ( lDialogString ,
					"-e 'set mystring to POSIX path of item 1 of mylist' " );
			strcat ( lDialogString ,
					"-e 'repeat with  i from 2 to the count of mylist' " );
			strcat ( lDialogString , "-e 'set mystring to mystring & \"|\"' " );
			strcat ( lDialogString ,
			"-e 'set mystring to mystring & POSIX path of item i of mylist' " );
			strcat ( lDialogString , "-e 'end repeat' " );
			strcat ( lDialogString , "-e 'mystring'" );
		}
		else
		{
			strcat ( lDialogString , ")'" ) ;
		}
    }
    else if ( tkinter2Present ( ) )
    {
		strcpy ( lDialogString , gPython2Name ) ;
		if ( ! isatty ( 1 ) )
		{
			strcat ( lDialogString , " -i" ) ; /* for osx without console */
		}
        strcat ( lDialogString ,
" -c \"import Tkinter,tkFileDialog;root=Tkinter.Tk();root.withdraw();");

    	if ( osascriptPresent ( ) )
    	{
			strcat ( lDialogString ,
"import os;os.system('''/usr/bin/osascript -e 'tell app \\\"Finder\\\" to set \
frontmost of process \\\"Python\\\" to true' ''');");
		}
		strcat ( lDialogString , "lFiles=tkFileDialog.askopenfilename(");
        if ( aAllowMultipleSelects )
        {
	        strcat ( lDialogString , "multiple=1," ) ;
        }
	    if ( aTitle && strlen(aTitle) )
	    {
			strcat(lDialogString, "title='") ;
			strcat(lDialogString, aTitle) ;
			strcat(lDialogString, "',") ;
	    }
        if ( aDefaultPathAndFile && strlen(aDefaultPathAndFile) )
        {
			getPathWithoutFinalSlash ( lString , aDefaultPathAndFile ) ;
			if ( strlen(lString) )
			{
				strcat(lDialogString, "initialdir='") ;
				strcat(lDialogString, lString ) ;
				strcat(lDialogString , "'," ) ;
			}
			getLastName ( lString , aDefaultPathAndFile ) ;
			if ( strlen(lString) )
			{
				strcat(lDialogString, "initialfile='") ;
				strcat(lDialogString, lString ) ;
				strcat(lDialogString , "'," ) ;
			}
		}
        if ( ( aNumOfFileFilters > 1 )
          || ( ( aNumOfFileFilters == 1 ) /* test because poor osx behaviour */
                && ( aFileFilters[0][strlen(aFileFilters[0])-1] != '*' ) ) )
        {
			strcat(lDialogString , "filetypes=(" ) ;
            for ( i = 0 ; i < aNumOfFileFilters ; i ++ )
            {
                strcat ( lDialogString , "('','" ) ;
                strcat ( lDialogString , aFileFilters [ i ] ) ;
                strcat ( lDialogString , "')," ) ;
            }
            strcat ( lDialogString , "('All files','*'))" ) ;
        }
		strcat ( lDialogString , ");\
\nif not isinstance(lFiles, tuple):\n\tprint lFiles\nelse:\
\n\tlFilesString=''\n\tfor lFile in lFiles:\n\t\tlFilesString+=str(lFile)+'|'\
\n\tprint lFilesString[:-1]\n\"" ) ;
    }
	else if ( dialogPresent ( ) )
	{
		strcpy ( lDialogString , "(dialog " ) ;
 		if ( aTitle && strlen(aTitle) )
		{
			strcat(lDialogString, "--title \"") ;
			strcat(lDialogString, aTitle) ;
			strcat(lDialogString, "\" ") ;
		}
		strcat ( lDialogString , "--fselect \"" ) ;
		if ( aDefaultPathAndFile && strlen(aDefaultPathAndFile) )
		{
			strcat(lDialogString, aDefaultPathAndFile) ;
		}
		strcat(lDialogString, "\" 0 60  >/dev/tty) 2>&1 ; clear >/dev/tty") ;
	}
	else
	{
		tinyfd_messageBox (gTitle, gMessage,"ok","error",1 ) ;
		return NULL ;
	}
    /* printf ( "lDialogString: %s\n" , lDialogString ) ; //*/
    if ( ! ( lIn = popen ( lDialogString , "r" ) ) )
    {
        return NULL ;
    }
	lBuff[0]='\0';
	p=lBuff;
	while ( fgets ( p , sizeof ( lBuff ) , lIn ) != NULL )
	{
		p += strlen ( p );
	}
    pclose ( lIn ) ;
    if ( lBuff[ strlen ( lBuff ) -1 ] == '\n' )
    {
    	lBuff[ strlen ( lBuff ) -1 ] = '\0' ;
    }
    /* printf ( "lBuff: %s\n" , lBuff ) ; //*/
	if ( lWasKdialog && aAllowMultipleSelects )
	{
		p = lBuff ;
		while ( ( p = strchr ( p , '\n' ) ) )
			* p = '|' ;
	}
	/* printf ( "lBuff2: %s\n" , lBuff ) ; //*/
	if ( ! aAllowMultipleSelects )
	{
		lIn = fopen( lBuff , "r" ) ;
		if ( ! lIn )
		{
			return NULL ;
		}
		fclose ( lIn ) ;
	}
	/* printf ( "lBuff3: %s\n" , lBuff ) ; //*/
	return lBuff ;
}


char const * tinyfd_selectFolderDialog (
	char const * const aTitle , /* "" */
	char const * const aDefaultPath ) /* "" */
{
	static char lBuff [ MAX_PATH_OR_CMD ] ;
    char lDialogString [ MAX_PATH_OR_CMD ] ;
    DIR * lDir ;
	FILE * lIn ;
    lBuff[0]='\0';

	if ( zenityPresent() )
	{
        strcpy ( lDialogString , "zenity --file-selection --directory" ) ;
		if ( aTitle && strlen(aTitle) )
		{
			strcat(lDialogString, " --title=\"") ;
			strcat(lDialogString, aTitle) ;
			strcat(lDialogString, "\"") ;
		}
		if ( aDefaultPath && strlen(aDefaultPath) )
		{
			strcat(lDialogString, " --filename=\"") ;
			strcat(lDialogString, aDefaultPath) ;
			strcat(lDialogString, "\"") ;
		}
	}
	else if ( kdialogPresent() )
	{
		strcpy ( lDialogString , "kdialog --getexistingdirectory" ) ;
		if ( aDefaultPath && strlen(aDefaultPath) )
		{
			strcat(lDialogString, " \"") ;
			strcat(lDialogString, aDefaultPath ) ;
			strcat(lDialogString , "\"" ) ;
		}
		else
		{
			strcat(lDialogString, " :" ) ;
		}
		if ( aTitle && strlen(aTitle) )
		{
			strcat(lDialogString, " --title \"") ;
			strcat(lDialogString, aTitle) ;
			strcat(lDialogString, "\"") ;
		}
	}
    else if ( osascriptPresent ( ) )
    {
		strcpy ( lDialogString ,"osascript -e 'POSIX path of ( choose folder ");
	    if ( aTitle && strlen(aTitle) )
	    {
			strcat(lDialogString, "with prompt \"") ;
			strcat(lDialogString, aTitle) ;
			strcat(lDialogString, "\" ") ;
	    }
		if ( aDefaultPath && strlen(aDefaultPath) )
		{
			strcat(lDialogString, "default location \"") ;
			strcat(lDialogString, aDefaultPath ) ;
			strcat(lDialogString , "\" " ) ;
		}
		strcat ( lDialogString , ")'" ) ;
    }
	else if ( tkinter2Present ( ) )
	{
		strcpy ( lDialogString , gPython2Name ) ;
		if ( ! isatty ( 1 ) )
		{
			strcat ( lDialogString , " -i" ) ; /* for osx without console */
		}
        strcat ( lDialogString ,
" -c \"import Tkinter,tkFileDialog;root=Tkinter.Tk();root.withdraw();");

    	if ( osascriptPresent ( ) )
    	{
			strcat ( lDialogString ,
"import os;os.system('''/usr/bin/osascript -e 'tell app \\\"Finder\\\" to set \
frontmost of process \\\"Python\\\" to true' ''');");
		}

		strcat ( lDialogString , "print tkFileDialog.askdirectory(");
	    if ( aTitle && strlen(aTitle) )
	    {
			strcat(lDialogString, "title='") ;
			strcat(lDialogString, aTitle) ;
			strcat(lDialogString, "',") ;
	    }
        if ( aDefaultPath && strlen(aDefaultPath) )
        {
				strcat(lDialogString, "initialdir='") ;
				strcat(lDialogString, aDefaultPath ) ;
				strcat(lDialogString , "'" ) ;
		}
		strcat ( lDialogString , ")\"" ) ;
	}
	else if ( dialogPresent ( ) )
	{
		strcpy ( lDialogString , "(dialog " ) ;
 		if ( aTitle && strlen(aTitle) )
		{
			strcat(lDialogString, "--title \"") ;
			strcat(lDialogString, aTitle) ;
			strcat(lDialogString, "\" ") ;
		}
		strcat ( lDialogString , "--dselect \"" ) ;
		if ( aDefaultPath && strlen(aDefaultPath) )
		{
			strcat(lDialogString, aDefaultPath) ;
		}
		strcat(lDialogString, "\" 0 60 >/dev/tty) 2>&1 ; clear >/dev/tty") ;
	}
	else
	{
		tinyfd_messageBox (gTitle, gMessage,"ok","error",1 ) ;
		return NULL ;
	}
    /* printf ( "lDialogString: %s\n" , lDialogString ) ; //*/
    if ( ! ( lIn = popen ( lDialogString , "r" ) ) )
    {
        return NULL ;
    }
	while ( fgets ( lBuff , sizeof ( lBuff ) , lIn ) != NULL )
	{}
	pclose ( lIn ) ;
    if ( lBuff[ strlen ( lBuff ) -1 ] == '\n' )
    {
    	lBuff[ strlen ( lBuff ) -1 ] = '\0' ;
    }
	/* printf ( "lBuff: %s\n" , lBuff ) ; //*/
	if ( strlen ( lBuff ) > 0 )
	{
		lDir = opendir ( lBuff ) ;
		if ( ! lDir )
		{
			return NULL ;
		}
		closedir ( lDir ) ;
	}
	return lBuff ;
}


/* on cancel it returns aDefaultInput */
char const * tinyfd_inputBox(
	char const * const aTitle , /* NULL or "" */
	char const * const aMessage , /* NULL or ""  may NOT contain \n nor \t */
	char const * const aDefaultInput ) /* "" */
{
	static char lBuff[MAX_PATH_OR_CMD];
	char lDialogString[MAX_PATH_OR_CMD];
	FILE * lIn ;
	int lResult ;
	lBuff[0]='\0';

	if ( zenityPresent() && !preferPython() )
	{
		strcpy ( lDialogString , "szAnswer=$(zenity --entry" ) ;
		if ( aTitle && strlen(aTitle) )
		{
			strcat(lDialogString, " --title=\"") ;
			strcat(lDialogString, aTitle) ;
			strcat(lDialogString, "\"") ;
		}
		if ( aMessage && strlen(aMessage) )
		{
			strcat(lDialogString, " --text=\"") ;
			strcat(lDialogString, aMessage) ;
			strcat(lDialogString, "\"") ;
		}
		if ( aDefaultInput )
		{
			if ( strlen(aDefaultInput) )
			{
				strcat(lDialogString, " --entry-text=\"") ;
				strcat(lDialogString, aDefaultInput) ;
				strcat(lDialogString, "\"") ;
			}
		}
		else
		{
			strcat(lDialogString, " --hide-text") ;
		}
		strcat ( lDialogString , ");echo $?$szAnswer" ) ;
	}
	else if ( kdialogPresent() && !preferPython())
	{
		strcpy ( lDialogString , "szAnswer=$(kdialog" ) ;
		if ( ! aDefaultInput )
		{
			strcat(lDialogString, " --password ") ;
		}
		else
		{
			strcat(lDialogString, " --inputbox ") ;

		}
		strcat(lDialogString, "\"") ;
		if ( aMessage && strlen(aMessage) )

		{
			strcat(lDialogString, aMessage ) ;
		}
		strcat(lDialogString , "\" \"" ) ;
		if ( aDefaultInput && strlen(aDefaultInput) )
		{
			strcat(lDialogString, aDefaultInput ) ;
		}
		strcat(lDialogString , "\"" ) ;
		if ( aTitle && strlen(aTitle) )
		{
			strcat(lDialogString, " --title \"") ;
			strcat(lDialogString, aTitle) ;
			strcat(lDialogString, "\"") ;
		}
		strcat ( lDialogString , ");echo $?$szAnswer" ) ;
	}
    else if ( osascriptPresent ( ) && !preferPython() )
    {
		strcpy ( lDialogString , "osascript -e 'try' -e 'display dialog \"") ;
	    if ( aMessage && strlen(aMessage) )
	    {
			strcat(lDialogString, aMessage) ;
	    }
		strcat(lDialogString, "\" ") ;
		strcat(lDialogString, "default answer \"") ;
		if ( aDefaultInput && strlen(aDefaultInput) )
		{
			strcat(lDialogString, aDefaultInput) ;
		}
		strcat(lDialogString, "\" ") ;
		if ( ! aDefaultInput )
		{
			strcat(lDialogString, "hidden answer true ") ;
		}
		if ( aTitle && strlen(aTitle) )
	    {
			strcat(lDialogString, "with title \"") ;
			strcat(lDialogString, aTitle) ;
			strcat(lDialogString, "\" ") ;
	    }
		strcat(lDialogString, "with icon note' ") ;
		strcat(lDialogString, "-e '\"0\" & text returned of result' " );
		strcat(lDialogString, "-e 'on error number -128' " ) ;
		strcat(lDialogString, "-e '1' " );
		strcat(lDialogString, "-e 'end try'") ;
	}
	else if ( tkinter2Present ( ) )
	{
		strcpy ( lDialogString , gPython2Name ) ;
		if ( ! isatty ( 1 ) )
		{
			strcat ( lDialogString , " -i" ) ;  /* for osx without console */
		}

		strcat ( lDialogString ,
" -c \"import Tkinter,tkSimpleDialog;root=Tkinter.Tk();root.withdraw();");

		if ( osascriptPresent ( ) )
		{
			strcat ( lDialogString ,
"import os;os.system('''/usr/bin/osascript -e 'tell app \\\"Finder\\\" to set \
frontmost of process \\\"Python\\\" to true' ''');");
		}

		strcat ( lDialogString ,"res=tkSimpleDialog.askstring(" ) ;
		if ( aTitle && strlen(aTitle) )
		{
			strcat(lDialogString, "title='") ;
			strcat(lDialogString, aTitle) ;
			strcat(lDialogString, "',") ;
		}
		if ( aMessage && strlen(aMessage) )
		{
			replaceSubStr ( aMessage , "\n" , "\\n" , lBuff ) ;
			strcat(lDialogString, "prompt='") ;
			strcat(lDialogString, lBuff) ;
			strcat(lDialogString, "',") ;
			lBuff[0]='\0';
		}
		if ( aDefaultInput )
		{
			if ( strlen(aDefaultInput) )
			{
				strcat(lDialogString, "initialvalue='") ;
				strcat(lDialogString, aDefaultInput) ;
				strcat(lDialogString, "',") ;
			}
		}
		else
		{
			strcat(lDialogString, "show='*'") ;
		}
		strcat(lDialogString, ");\nif res is None :\n\tprint 1");
		strcat(lDialogString, "'\nelse :\n\tprint '0'+res\n\"" ) ;
	}
	else if ( dialogPresent ( ) || whiptailPresent ( ) )
	{
		if ( dialogPresent ( ) )
		{
			strcpy ( lDialogString , "(dialog " ) ;
		}
		else
		{
			strcpy ( lDialogString , "(whiptail " ) ;
		}
		if ( aTitle && strlen(aTitle) )
		{
			strcat(lDialogString, "--title \"") ;
			strcat(lDialogString, aTitle) ;
			strcat(lDialogString, "\" ") ;
		}
		if ( ! aDefaultInput )
		{
			strcat ( lDialogString , "--passwordbox" ) ;
		}
		else
		{
			strcat ( lDialogString , "--inputbox" ) ;
		}
		strcat ( lDialogString , " \"" ) ;
		if ( aMessage && strlen(aMessage) )
		{
			strcat(lDialogString, aMessage) ;
		}
		strcat(lDialogString,"\" 10 60 ") ;
		if ( aDefaultInput && strlen(aDefaultInput) )
		{
			strcat(lDialogString, "\"") ;
			strcat(lDialogString, aDefaultInput) ;
			strcat(lDialogString, "\" ") ;
		}
		strcat(lDialogString,
			">/dev/tty ) 2>/tmp/tinyfdInputBox.txt; tinyfdBool=$? ; tinyfdRes=$(cat /tmp/tinyfdInputBox.txt);rm /tmp/tinyfdInputBox.txt;echo $tinyfdBool$tinyfdRes;clear >/dev/tty") ;
	}

	/* printf ( "lDialogString: %s\n" , lDialogString ) ; //*/
	if ( ! ( lIn = popen ( lDialogString , "r" ) ) )
	{
		return NULL ;
	}
	while ( fgets ( lBuff , sizeof ( lBuff ) , lIn ) != NULL )
	{}
	pclose ( lIn ) ;
	/* printf ( "len Buff: %lu\n" , strlen(lBuff) ) ; //*/
	/* printf ( "lBuff0: %s\n" , lBuff ) ; //*/
	if ( lBuff[ strlen ( lBuff ) -1 ] == '\n' )
	{
		lBuff[ strlen ( lBuff ) -1 ] = '\0' ;
	}
	/* printf ( "lBuff1: %s len: %lu \n" , lBuff , strlen(lBuff) ) ; //*/
    lResult =  strncmp ( lBuff , "0" , 1) ? 0 : 1 ;
	/* printf ( "lResult: %d \n" , lResult ) ; //*/
    if ( ! lResult )
    {
		if ( aDefaultInput )
		{
			strcpy(lBuff+1,aDefaultInput);
		}
		else
		{
			lBuff[1] = '\0';
		}
	}
	/* printf ( "lBuff+1: %s\n" , lBuff+1 ) ; //*/
	return lBuff+1 ;
}


/* returns the hexcolor as a string "#FF0000" */
/* aoResultRGB also contains the result */
/* aDefaultRGB is used only if aDefaultHexRGB is NULL */
/* aDefaultRGB and aoResultRGB can be the same array */
char const * tinyfd_colorChooser(
	char const * const aTitle , /* NULL or "" */
	char const * const aDefaultHexRGB , /* NULL or "#FF0000"*/
	unsigned char aDefaultRGB[3] , /* { 0 , 255 , 255 } */
	unsigned char aoResultRGB[3] ) /* { 0 , 0 , 0 } */
{
	static char lBuff [ 16 ] ;
	char lTmp [ 16 ] ;
    char lDialogString [ MAX_PATH_OR_CMD ] ;
	char lDefaultHexRGB[8];
	char * lpDefaultHexRGB;
 	FILE * lIn ;
	int lWasZenity3 = 0 ;
	int lWasOsascript = 0 ;

	lBuff[0]='\0';

	if ( aDefaultHexRGB )
	{
		Hex2RGB ( aDefaultHexRGB , aDefaultRGB ) ;
		lpDefaultHexRGB = (char *) aDefaultHexRGB ;
	}
	else
	{
		RGB2Hex( aDefaultRGB , lDefaultHexRGB ) ;
		lpDefaultHexRGB = (char *) lDefaultHexRGB ;
	}

    if ( zenity3Present() )
	{
		lWasZenity3 = 1 ;
        sprintf ( lDialogString ,
"zenity --color-selection --show-palette --color=%s" , lpDefaultHexRGB ) ;
		if ( aTitle && strlen(aTitle) )
		{
			strcat(lDialogString, " --title=\"") ;
			strcat(lDialogString, aTitle) ;
			strcat(lDialogString, "\"") ;
		}
	}
	else if ( kdialogPresent() )
	{
        sprintf ( lDialogString ,
"kdialog --getcolor --default '%s'" , lpDefaultHexRGB ) ;
		if ( aTitle && strlen(aTitle) )
		{
			strcat(lDialogString, " --title \"") ;
			strcat(lDialogString, aTitle) ;
			strcat(lDialogString, "\"") ;
		}
	}
	else if ( osascriptPresent ( ) )
    {
    	lWasOsascript = 1 ;
		strcpy ( lDialogString , "osascript -e 'tell app (path to frontmost \
application as Unicode text) to set mycolor to choose color default color {");
		sprintf(lTmp, "%d", 256 * aDefaultRGB[0] ) ;
		strcat(lDialogString, lTmp ) ;
		strcat(lDialogString, "," ) ;
		sprintf(lTmp, "%d", 256 * aDefaultRGB[1] ) ;
		strcat(lDialogString, lTmp ) ;
		strcat(lDialogString, "," ) ;
		sprintf(lTmp, "%d", 256 * aDefaultRGB[2] ) ;
		strcat(lDialogString, lTmp ) ;
		strcat(lDialogString, "}' " ) ;
		strcat ( lDialogString ,
"-e 'set mystring to ((item 1 of mycolor)/256 as integer) as string' " );
		strcat ( lDialogString ,
"-e 'repeat with i from 2 to the count of mycolor' " );
		strcat ( lDialogString ,
"-e 'set mystring to mystring & \" \" & \
((item i of mycolor)/256 as integer) as string' " );
		strcat ( lDialogString , "-e 'end repeat' " );
		strcat ( lDialogString , "-e 'mystring'");
    }
	else if ( tkinter2Present ( ) )
	{
		strcpy ( lDialogString , gPython2Name ) ;
		if ( ! isatty ( 1 ) )
		{
			strcat ( lDialogString , " -i" ) ;  /* for osx without console */
		}

		strcat ( lDialogString ,
" -c \"import Tkinter,tkColorChooser;root=Tkinter.Tk();root.withdraw();");

		if ( osascriptPresent ( ) )
		{
			strcat ( lDialogString ,
"import os;os.system('''osascript -e 'tell app \\\"Finder\\\" to set \
frontmost of process \\\"Python\\\" to true' ''');");
		}

		strcat ( lDialogString , "res=tkColorChooser.askcolor(color='" ) ;
		strcat(lDialogString, lpDefaultHexRGB ) ;
		strcat(lDialogString, "'") ;
	    if ( aTitle && strlen(aTitle) )
	    {
			strcat(lDialogString, ",title='") ;
			strcat(lDialogString, aTitle) ;
			strcat(lDialogString, "'") ;
	    }
		strcat ( lDialogString , ");\
\nif res[1] is not None:\n\tprint res[1]\"" ) ;
	}
	else if ( zenityPresent() || dialogPresent ( ) || whiptailPresent ( ) )
	{
		tinyfd_inputBox(aTitle,
				"Enter hex rgb color\nie: #f5ca20",lpDefaultHexRGB);
	}
	else
	{
		tinyfd_messageBox (gTitle, gMessage,"ok","error",1 ) ;
		return NULL ;
	}
    /* printf ( "lDialogString: %s\n" , lDialogString ) ; //*/
    if ( ! ( lIn = popen ( lDialogString , "r" ) ) )
    {
        return NULL ;
    }
	while ( fgets ( lBuff , sizeof ( lBuff ) , lIn ) != NULL )
	{
	}
	pclose ( lIn ) ;
    if ( ! strlen ( lBuff ) )
    {
        return NULL ;
    }
	/* printf ( "len Buff: %lu\n" , strlen(lBuff) ) ; //*/
	/* printf ( "lBuff0: %s\n" , lBuff ) ; //*/
    if ( lBuff[ strlen ( lBuff ) -1 ] == '\n' )
    {
    	lBuff[ strlen ( lBuff ) -1 ] = '\0' ;
    }
    if ( lWasZenity3 )
    {
        lBuff[3]=lBuff[5];
        lBuff[4]=lBuff[6];
        lBuff[5]=lBuff[9];
        lBuff[6]=lBuff[10];
        lBuff[7]='\0';
        Hex2RGB(lBuff,aoResultRGB);
    }
    else if ( lWasOsascript )
    {
    	sscanf(lBuff,"%hhu %hhu %hhu",
			   & aoResultRGB[0], & aoResultRGB[1],& aoResultRGB[2]);
    	RGB2Hex(aoResultRGB,lBuff);
    }
    else
    {
		Hex2RGB(lBuff,aoResultRGB);
	}
	/* printf("%d %d %d\n", aoResultRGB[0],aoResultRGB[1],aoResultRGB[2]); //*/
	/* printf ( "lBuff: %s\n" , lBuff ) ; //*/
	return lBuff ;
}


#endif /* _WIN32 */
