Node Box Editor
===============

[![Build Status](https://travis-ci.org/rubenwardy/NodeBoxEditor.svg?branch=master)](https://travis-ci.org/rubenwardy/NodeBoxEditor)

Use this editor to make nodeboxes for nodes in Minetest.

Version: 0.9.0

To do list and bug tracker is here: https://github.com/rubenwardy/NodeBoxEditor/issues?state=open

License
-------

Created by rubenwardy.

GPL 3.0 or later.

https://www.gnu.org/licenses/gpl-3.0.txt

Contributors:

* Kaeza: small Linux based fixes, and the start of the CMakeLists.txt (now modified)
* Traxie21: small changed textures for node.
* jmf: Fixed small compile bug fixes.
* ShadowNinja: Unlimited node boxes, code style, refactoring and small fixes.
* Krock/SmallJoker: Fixed warnings, fixed conf_cmake.hpp and Windows builds.
* asl97: Fixed a memory leak.

If I forgot anyone, please let me know.

Building
--------

For notes on compiling the editor, see docs/developers.md

Using the editor
================

What actions you can take are limited by which mode / tool you are in.

Editor
------

* Use WASD to move the perspective view.
* Use the options under view to pick which viewport to show.
* Use the mode icon on the top left of the screen to change modes / tools.
    * Click the icon to pop out a list of modes / tools.
    * Click an icon to select the corresponding mode.
* Press N to enter the node tool.
* Press B to enter the node box tool.
* Press T to enter the texture tool.

Menu
----

* File
    * Open Project - discard the current project and open a new one.
    * Save Project - save the current project.
    * Export - export the project to Lua, or other formats supported.
    * Exit - exit the editor.
* Editor
    * Limiting - toggle whether node boxes can go outside of node boundaries.
    * Snapping - toggle whether node boxes will snap to a 16 pixel grid.
* View - use these options to change the viewport (tiled, or make a viewport fullscreen).
* Project - options unique to an tool.
* Help
    * About - see program's version, and credits.

Node Box Tool
-------------

In this mode / tool, you can edit the contents of a node.
Icon: A blue cube with a chunk taken out of it.

* Click + on the sidebar to add a node box. (or press insert)
* Click an item in the listbox to select a node box. (or press up/down)
* Click - on the sidebar to remove the selected node box. (or press delete)
* Size the node box using the handles in the orthographic viewports.
* Scale the node box by pressing control, and using the handles in the orthographic viewports.
* Position the node box by pressing shift, and using the handles in the orthographic viewports.
* Enter properties for a node box in the text boxes on the side bar.
    * Click update to apply your changes.
    * Click revert to discard your changes, and get the current properties.

Node Tool
---------

In this mode / tool, you can create multiple nodes and manage them.
Icon: A yellow cube.

* Click + on the sidebar to add a node. (or press insert)
* Click an item in the listbox to select a node. (or press up/down)
* Click - on the sidebar to remove the selected node. (or press delete)

Texture Tool
------------

In this mode / tool, you can edit the textures of a node.
Icon: Red, green and blue cube.

* Click a face in the side bar to open up the texture dialog.
* Click import to import an texture.
	* Type the location relative to the NBE root if a portable build, or relative to the home directory if installed.
* Click export to export the currently selected texture to the nbe root folder if portable, or the home directory if installed.
* Click an texture in the list box to select it.
* Click apply to apply the selected texture to the side of the node.

Saving and Exporting
--------------------

The editor can currently save and export to several formats.

* Node Box Editor file (nbe) - The file unique to this editor. General save / open format.
* Lua file (lua) - Exports code which could be installed as a mod. Use when you want to run in Minetest.
* Minetest Classic (cpp) - Exports code to be used in Minetest Classic. Use when you want to run in Minetest Classic.
