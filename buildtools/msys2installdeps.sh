#!/usr/bin/env bash
# -------------------------------------------------------------------------------
# This script installs all dependencies required for building Inkscape with MSYS2
#   execute it once on an MSYS shell, i.e.
#    - use the "MSYS2 MSYS" shortcut in the start menu or
#    - run "msys2.exe" in MSYS2's installation folder
#
# MSYS2 and installed libraries can be updated later by executing
#   pacman -Syu --ignore=mingw-w64-*-imagemagick
# in an MSYS shell
# -------------------------------------------------------------------------------

# select if you want to build 32-bit (i686), 64-bit (x86_64), or both
case "$MSYSTEM" in
  MINGW32)
    ARCH=mingw-w64-i686
    ;;
  MINGW64)
    ARCH=mingw-w64-x86_64
    ;;
  *)
    ARCH={mingw-w64-i686,mingw-w64-x86_64}
    ;;
esac

# sync package databases
pacman -Sy

# install basic development system, compiler toolchain and build tools
eval pacman -S --needed --noconfirm \
git \
intltool \
base-devel \
$ARCH-toolchain \
$ARCH-cmake \
$ARCH-ninja

# install Inkscape dependecies (required)
eval pacman -S --needed --noconfirm \
$ARCH-gc \
$ARCH-gsl \
$ARCH-popt \
$ARCH-libxslt \
$ARCH-boost \
$ARCH-gtk2 \
$ARCH-gtkmm

# install Inkscape dependecies (optional)
eval pacman -S --needed --noconfirm \
$ARCH-poppler \
$ARCH-potrace \
$ARCH-libcdr \
$ARCH-libvisio \
$ARCH-libwpg \
$ARCH-aspell \
$ARCH-aspell-en \
$ARCH-gtkspell

# install ImageMagick (as Inkscape requires old version ImageMagick 6 we have to specify it explicitly)
# to prevent future updates:
#     add the line
#        "IgnorePkg = mingw-w64-*-imagemagick"
#     to
#        "C:\msys64\etc\pacman.conf"
#     or (always!) run pacman with the additional command line switch
#        --ignore=mingw-w64-*-imagemagick
for arch in $(eval echo $ARCH); do
  case ${arch} in
    mingw-w64-i686)
      pacman -U --needed --noconfirm https://downloads.sourceforge.net/project/msys2/REPOS/MINGW/i686/mingw-w64-i686-imagemagick-6.9.3.7-1-any.pkg.tar.xz
      ;;
    mingw-w64-x86_64)
      pacman -U --needed --noconfirm https://downloads.sourceforge.net/project/msys2/REPOS/MINGW/x86_64/mingw-w64-x86_64-imagemagick-6.9.3.7-1-any.pkg.tar.xz
      ;;
  esac
done


# install Python and modules used by Inkscape
eval pacman -S --needed --noconfirm \
$ARCH-python2 \
$ARCH-python2-pip \
$ARCH-python2-lxml \
$ARCH-python2-numpy \
$ARCH-python2-pillow
for arch in $(eval echo $ARCH); do
  case ${arch} in
    mingw-w64-i686)
      /mingw32/bin/pip install coverage pyserial scour
      ;;
    mingw-w64-x86_64)
      /mingw64/bin/pip install coverage pyserial scour
      ;;
  esac
done
