/*
tinyfiledialogs.h
optional unique header file of the tiny file dialogs library - tinyfd
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
or any GUI-less program, there is NO MAIN LOOP nor init.
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

#ifndef TINYFILEDIALOGS_H
#define TINYFILEDIALOGS_H

/*
if tinydialogs.c is compiled with a C++ compiler
rather than with a C compiler, you need to comment out:
extern "C" {
and the corresponding closing bracket:
}
*/

#ifdef	__cplusplus
extern "C" {
#endif /* __cplusplus */

extern int tinyfd_forceConsole ; /* for UNIX only: 0 (default) or 1 */
/* 1 forces all dialogs into console mode even when the X server is present */
/* can be modified at run time */

int tinyfd_messageBox (
    char const * const aTitle , /* "" */
    char const * const aMessage , /* "" may contain \n and \t */
    char const * const aDialogType , /* "ok" "okcancel" "yesno" */
    char const * const aIconType , /* "info" "warning" "error" "question" */
    int const aDefaultButton ) ; /* 0 for cancel/no , 1 for ok/yes */
/* returns 0 for cancel/no , 1 for ok/yes */

char const * tinyfd_saveFileDialog (
    char const * const aTitle , /* "" */
    char const * const aDefaultPathAndFile , /* "" */
    int const aNumOfFileFilters , /* 0 */
    char const * const * const aFileFilters ) ; /* NULL or {"*.txt"} */

char const * tinyfd_openFileDialog (
    char const * const aTitle , /* "" */
    char const * const aDefaultPathAndFile , /* "" */
    int const aNumOfFileFilters , /* 0 */
    char const * const * const aFileFilters , /* NULL or {"*.jpg","*.png"} */
    int aAllowMultipleSelects ) ; /* 0 or 1 */
/* in case of multiple files, the separator is | */

char const * tinyfd_selectFolderDialog (
	char const * const aTitle , /* "" */
    char const * const aDefaultPath ) ; /* "" */

char const * tinyfd_inputBox(
	char const * const aTitle , /* "" */
	char const * const aMessage , /* "" may NOT contain \n nor \t */
	char const * const aDefaultInput ) ; /* "" */
/* on cancel it returns aDefaultInput */

char const * tinyfd_colorChooser(
	char const * const aTitle , /* "" */
	char const * const aDefaultHexRGB , /* NULL or "#FF0000" */
	unsigned char aDefaultRGB[3] , /* { 0 , 255 , 255 } */
	unsigned char aoResultRGB[3] ) ; /* { 0 , 0 , 0 } */
/* returns the hexcolor as a string "#FF0000" */
/* aoResultRGB also contains the result */
/* aDefaultRGB is used only if aDefaultHexRGB is NULL */
/* aDefaultRGB and aoResultRGB can be the same array */

#ifdef	__cplusplus
}
#endif /* __cplusplus */

#endif /* TINYFILEDIALOGS_H */


/*
- On windows: link against Comdlg32.lib User32.lib and Shell32.lib
- On unix: it tries command line calls, so no such need.
- Use windows separator on windows and unix separator on unix.
- char const * fileFilters[3] = { "*.obj" , "*.stl" , "*.dxf" } ;
- String memory is preallocated statically for all the returned values.
- On unix you need zenity or kdialog or python2/tkinter or dialog installed.
  Don't worry, it's already included on most (if not all) desktops.
- If you pass only a path instead of path + filename,
  make sure it ends with a separator.
- tinyfd_forceConsole=1; forces all dialogs into console mode (unix only).
*/
