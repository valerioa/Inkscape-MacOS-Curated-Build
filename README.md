# Important

Since the release of Inkscape 1.0, and following versions, this project has no reason to exist anymore. Inkscape 1.0 (Inkscape 1.1.1 at the time of this writing - https://inkscape.org/gallery/item/29282/Inkscape-1.1.1.dmg ) is fully integrated with the Mac OSX UI. The 1.x.x Inkscape has Mac-style menus and window decorations. Granted, it is not perfect: the clipboard integration still has some problems (November 2021). However, Inkscape 1.x.x takes advantage of all the bugfixes and improvement that have been developed. I advise all to install the most recent version of Inkscape 1.x.x.

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

I will publsih here the full Inkscape source that builds with
Macports, has full gtk2/quartz,DnD,shortcuts/pasteboard integration.

Build is configured with

<pre>
./autogen.sh
./configure \
--prefix=/Applications/Inkscape.app/Contents/Resources \
--without-gnome-vfs \
--enable-lcms \
--enable-poppler-cairo \
--disable-silent-rules \
--disable-strict-build \
--enable-carbon \
--disable-dbusapi
</pre>

This build dmg installer package is larger than the mainstream
one. Build contains python, all python packages, and tcl+tk (a
dependency of python-pil, which is a dependency of Uniconvertor).
The dmg file in the Releases tab should be about 1.5 times the
original Inkscape dmg.

### Build Details

1. Install MacPorts with
<pre>
./configure --prefix=/Applications/Inkscape.app/Contents/Resources/
</pre>

2. Install MacPorts packages
3. Build and install Gtk2
4. Build and install Inkscape
5. cd to the valerio-packaging directory
<pre>
bash gen_keepers ; say Done! # this takes a bit of time
mv /Applications/Inkscape.app /Applications/Inkscape_Backup.app
mkdir /Applications/Inkscape.app
rsync -Sav --files-from=./keepers /Applications/Inkscape_Backup.app/ /Applications/Inkscape.app/ ; say Done!

</pre>
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

Gtk2 is built (after patching) with:

<pre>
CUPS_CONFIG=/usr/bin/cups-config \
LDFLAGS="-framework Cocoa -framework Carbon" \
./configure \
--prefix=/Applications/Inkscape.app/Contents/Resources \
--enable-static \
--disable-glibtest \
--enable-introspection \
--with-gdktarget=quartz \
gio_can_sniff=yes
</pre>




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



