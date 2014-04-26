Node Box Editor
===============

Use this editor to make nodeboxes for nodes in Minetest.

Version: 0.6.2

License
-------

Created by rubenwardy.

GPL 3.0 or later.

https://www.gnu.org/licenses/gpl-3.0.txt

Credits:

* Kaeza: small linux based fixes, and the start of the CMAKELISTS.txt (now modified)
* Traxie21: small changed textures for node.
* jmf: Fixed small compile bugs.
* ShadowNinja: Unlimited node boxes, small fixes.

Building
--------

    # download source and go to the root folder
    $ sudo apt-get install build-essential libirrlicht-dev cmake libpng12-dev libbz2-dev libjpeg8-dev libgl1-mesa-dev
    $ cmake .
    # replace 2 with the number of cores you have + 1
    $ make -j2

Using the editor
================

What actions you can take are limited by which mode / tool you are in.

Editor
------

* Use WASD to move the perspective view.
* Use the options under view to pick which view port to show.
* Use the mode icon on the top left of the screen to change modes / tools.
    * Click the icon to pop out a list of modes / tools
    * Click an icon to select the corresponding mode.

Node Box Tool
-------------

In this mode / tool, you can edit the contents of a node.
Icon: A blue cube with a chunk taken out of it.

* Click + on the sidebar to add a node box.
* Click an item in the listbox to select a node box.
* Click - on the sidebar to remove the selected node box.
* Size the node box using the handles in the orthographic viewports.
* Scale the node box by pressing control, and using the handles in the orthographic viewports.
* Position the node box by pressing shift, and using the handles in the orthographic viewports.
* Enter properties for a nodebox in the text boxes.
    * Click update to apply your changes.
    * Click revert to discard your changes, and get the current properties.

Node Tool
---------

In this mode / tool, you can create multiple nodes and manage them.
Icon: A yellow cube.

* Click + on the sidebar to add a node.
* Click an item in the listbox to select a node.
* Click - on the sidebar to remove the selected node.
