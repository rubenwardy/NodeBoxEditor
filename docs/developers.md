Introduction
============

This guide contains technical information on installing, building, and maintaining the Node Box Editor

Code Style
----------

The project tries to follow [Minetest's code style](http://dev.minetest.net/Code_style_guidelines)

GitHub things
-------------

Patches should be in their own branch when making a pull request.

Do not pull request from upstream to your repo using GitHub's pull request and merge feature.

Releasing a new version
-----------------------

* Bump settings in CMAKE.
* Update README.md
* Create windows build.
* Write changelog.
* Commit and push code to upstream.
* Post message on Minetest forums.
* Tag and release version on GitHub.


Building and Installation
=========================

Linux
-----

**Install Dependencies**

    # download source and go to the root folder
    $ sudo apt-get install build-essential libirrlicht-dev cmake libpng12-dev libbz2-dev libjpeg8-dev libgl1-mesa-dev

**Compiling**

    # Compile the editor
    $ cmake .
    $ make -j2

**Running**

    $ ./bin/nodeboxeditor
    # You could also double click the executable file in bin

**Installing**

    $ sudo make install
    $ nodeboxeditor
    # See make_install.md
	
Microsoft Visual Studio
-----------------------

Express edition makes no difference to the process.

**Prerequisites**

* Download Irrlicht. Some features in NBE require Irrlicht 1.8 or later in order to be enabled.
* Download the source code for NBE
* You will need to download Visual Studio, of course.

**Step One: Set up a project**

* Copy the conf_cmake.hpp.in to conf_cmake.hpp where you downloaded the source
* Open up conf_cmake.hpp, and edit the defines where there is @NBE_A_SETTING@
	* @NBE_DESCR_VERSION@:  "0.7 - Iron" for example
* Create a C++ project in Visual Studio
* Add the source code using right click > Add > Existing file. Make sure to include src/FileFormat and src/util as well.

**Step Two: Adding Irrlicht**

See [Irrlicht's tutorial](http://irrlicht.sourceforge.net/docu/example001.html) on setting up Visual Studio if you have problems.
* Right click on the name of the project on the Solution Explorer
* Click properties
* Go to the C++ Include Directories tab
* Add the irrlicht include folder to include directories
* Add the irrlicht lib/win32-visualstudio folder to lib directories
* Copy irrlicht/bin/win32-visualstudio/irrlicht.dll to project/debug/irrlicht.dll

The project should now build correctly. You will need to copy the media folder across to project/debug


	
Code
====

This is outdated info, needs updating.

* **common**.hpp - general compile settings, includes and defines. Used in every other file.
* **conf_cmake**.hpp.in - defines from cmake.
* **main**.cpp - contains the main() function, starts irrlicht and the editor.
* **Editor**.cpp/hpp - contains the Editor class which contains the update loop.
* **EditorState**.cpp/hpp - contains the EditorState class, which is used to share common variables. Also contains main class for mode FSM.
* **MenuState**.cpp/hpp - contains the MenuState class which handles the user interface such as the menu bar and mode icons.
* **NBEEditor**.cpp/hpp - contains the node box editor mode in the mode FSM.
* **NodeEditor**.cpp/hpp - contains the node editor mode in the mode FSM.
* **Project**.cpp/hpp - holds the project in a contained way.
* **Node**.cpp/hpp - hold the data and node boxes for a single node.
* **NodeBox**.cpp/hpp - a single node box.
* **Configuration**.cpp/hpp - the settings manager for the editor. Reads from / writes to editor.conf
* **GUIHelpers**.cpp/hpp - contains helpers such as creating x/y/z text boxes.
* FileFormat
	* **FileFormat**.cpp/hpp - factory and base class for file formats.
	* **Lua**.cpp/hpp - the Lua file parser.
	* **NBE**.cpp/hpp - the NBE file parser.
	* **MTC**.cpp/hpp - the Minetest Classic (.cpp) file parser.
* util
	* **string**.cpp/hpp - helper functions for strings

