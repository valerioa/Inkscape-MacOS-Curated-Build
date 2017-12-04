# Inkscape MacOS Curated Build

This is a curated build of Inkscape (currently 0.92.2) for MacOS.

Sometime around 2013, I produced an Inkscape build that was tightly
integrated with MacOS, and did not need X11/XQuartz:

1. DnD functionality
2. MacOS look and feel menubar
3. MacOS shortcut keys (Cmd+V, Cmd+C, et.)
4. Fully functional pasteboard integration

That build has been well received by users. However, contributing
patches to the Inkscape mainstream source repository is not the
easiest thing in the world. My 2013 patches never made it to the
Inkscape source repository.

I have been using that build (0.48.4) for all these years. It sill
works on High Sierra - you can find a dmg installer for that in the
Releases tab.

I wanted to try the newest version (0.92.2) and newest feature, so I
ported my old patches to the mainline Inkscape source.

The current release (0.92.2) is in the Releases tab

## The Build

MacOS build tools and compilers implicitly rpath dynamic libraries
into other libraries and executable. To build a self-contained
Inkscape app package for MacOS, in 2013, the accepted method to
build Inkscape for MacOS was to:

1. install Macports into a separate directory
1. Build Inkscape against the MacPorts directory
1. Rewrite all rpath'ed paths in the Inkscape libraries and executable

That might still the build process today. The revision od the rpathed
path was particularly painful.

To make my life a little easier, I am pioneering a new build process:

1. Install Macports into /Applications/Inkscape.app/Contents/Resources
2. Build Inkscape against the MacPorts directory

No revision of rpath'ed paths is necessary as all paths are already in
the proper place

This buils contains a python installation and all python packages
necessary to run all python extensions (including
Uniconvertor). Hopefully, I did not forget any.

### Gtk2 patching

The Gtk2/3 quartz engine is designed mostly around the GIMP build.
The gtk-clipboard to MacOS-pasteboard connectivty work only for
clipboard and pasteboard types that GIMP uses.

In order for the OS to be able to shuttle clipboard types that are
used by Inkscape (SVG, PDF and postscript, mostly) Gtk2 has to be
patched.

Without being linked to a patched Gtk2/3, Inscape is not able to use
the MacOS clipboard.

These buils and releases are linked with a patched Gtk2, thus they can
full use the MacOS clipboard

Patches are in the valerio-patches directory.




## Known Problems

The Inkscape UI is designed and built around the Windows/Linux UI paradigm, where
each window owns a menubar. On Mac, there is a menubar common to all
windows. For this build, I am just considering a common menubar for
all Inkscape windows. I know there are Inscape extensions that
dynamically modify the menubar. Personally, I've never used one of
those. However, if not having dynamic menubars probel to be a problem
to many, we can easily fix it - but it might render the UI slightly
more sluggish: The menubar has to be refreshed at each "verb" execution.

MenuItems that use checkmarks do not update - this could be easily
fixed too.

DnD on a window does not work yet.

## Bugs & Other Critters

Report to me bugs that pertain to gtk/quartz/MacOS integration of
these buils. Inkscape bugs need to be reported to the Inkscape
authors: https://inkscape.org/en/contribute/report-bugs/



