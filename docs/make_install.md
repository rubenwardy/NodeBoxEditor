Make Install
============

This document will outline the process of installing the editor onto your system,
and will also give important information in order to avoid common slip ups.

This is only for Linux operating systems.

Installing
----------

After you compile the editor (see readme, or developers.md), it is as simple as this:

    $ sudo make install

If you receive any errors, please report them.

Using Installed Builds
----------------------

To run, use <code>$ nodeboxeditor</code>

* Project, imports and exports will be saved to and read from <code>/home/</code>.
  You can change this with the save_directory setting in editor.conf
* Configuration will be read from <code>/home/.config/nodeboxeditor.conf</code> first,
  and then </code>/usr/local/share/nodeboxeditor/editor.conf</code> if that fails.
* Resources are saved in <code>/usr/local/share/nodeboxeditor</code>
* The executable is in <code>/usr/local/bin</code>
* Textures will be imported and exported to <code>/home/</code>.

