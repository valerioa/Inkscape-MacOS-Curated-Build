#!/bin/bash -e

if [ "$SNAP_ARCH" == "amd64" ]; then
  ARCH="x86_64-linux-gnu"
elif [ "$SNAP_ARCH" == "armhf" ]; then
  ARCH="arm-linux-gnueabihf"
else
  ARCH="$SNAP_ARCH-linux-gnu"
fi

export GNOME_VFS_MODULE_CONFIG_PATH="${SNAP}/etc/gnome-vfs-2.0/modules"
export GNOME_VFS_MODULE_PATH="${SNAP}/usr/lib/${ARCH}/gnome-vfs-2.0/modules"

export GTK_PATH="${SNAP}/usr/lib/${ARCH}/gtk-2.0/"

export INKSCAPE_PORTABLE_PROFILE_DIR="${SNAP_USER_DATA}"
export INKSCAPE_LOCALEDIR="${SNAP}/share/locale/"

if [ -d "/home/${USER}" ]; then
	export HOME="/home/${USER}"
fi

exec $@
