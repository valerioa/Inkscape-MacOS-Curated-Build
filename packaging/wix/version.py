#!/usr/bin/python

from __future__ import print_function

import os
import struct

from helpers import get_inkscape_dist_dir

version = ''
versionstr = ''
architecture = ''

def is64bitArchitecture(filename):
	''' test if a executable is of x64 format @see http://stackoverflow.com/questions/1001404/check-if-unmanaged-dll-is-32-bit-or-64-bit/1002672#1002672
	@see http://www.microsoft.com/whdc/system/platform/firmware/PECOFF.mspx
	  //offset to PE header is always at 0x3C
	  //PE header starts with "PE\0\0" =  0x50 0x45 0x00 0x00
	  //followed by 2-byte machine type field (see document above for enum)
	'''
	with open(filename, 'rb') as cofffile:
		cofffile.seek(0x3c)
		peOffset = struct.unpack('H', cofffile.read(2))[0]
		cofffile.seek(peOffset)
		peHead = struct.unpack('I', cofffile.read(4))[0]
		if peHead != 0x00004550:	# "PE\0\0", little-endian
			# throw new Exception("Can't find PE header")
			pass
		machineType = struct.unpack('H', cofffile.read(2))[0]
		if machineType in (0x8664, 0x200):
			return True
	return False

#get directory containing the inkscape distribution files
inkscape_dist_dir = get_inkscape_dist_dir()

if is64bitArchitecture(inkscape_dist_dir + '\\inkscape.exe'):
	architecture = '-x64'
else:
	architecture = '-x86'

# retrieve the version information from CMakeLists.txt
with open('..\..\CMakeLists.txt', 'r') as rc:
	isversioninfo = False

	for line in rc.readlines():
		if 'set(INKSCAPE_VERSION_MAJOR' in line:
			version_major = line.split()[-1][0:-1]
		if 'set(INKSCAPE_VERSION_MINOR' in line:
			version_minor = line.split()[-1][0:-1]
		if 'set(INKSCAPE_VERSION_PATCH' in line:
			version_patch = line.split()[-1][0:-1]
			if version_patch == '':
				version_patch = '0'
		if 'set(INKSCAPE_VERSION_SUFFIX' in line:
			version_suffix = line.split()[-1][0:-1]
			version_suffix = version_suffix.replace('"', '')

	version_list = [version_major, version_minor, version_patch, '0']
	version = '.'.join(version_list)

	versionstr = '.'.join(version_list[0:2])
	if version_patch != '0':
		versionstr += '.' + version_patch
	versionstr += version_suffix

	# required by install.bat
	print(versionstr + architecture)

with open('version.wxi', 'w') as wxi:
	wxi.write("<?xml version='1.0' encoding='utf-8'?>\n")
	wxi.write("<!-- do not edit, this file is created by version.py tool any changes will be lost -->\n")
	wxi.write("<Include>\n")
	wxi.write("<?define ProductVersion='" + version + "' ?>\n")
	wxi.write("<?define FullProductName='Inkscape " + versionstr + "' ?>\n")
	if 'x64' in architecture:
		wxi.write("<?define ProgramFilesFolder='ProgramFiles64Folder' ?>\n")
		wxi.write("<?define Win64='yes' ?>\n")
		wxi.write("<?define InstallerVersion='200' ?>\n")
		wxi.write("<?define Platform='x64' ?>\n")
	else:
		wxi.write("<?define ProgramFilesFolder='ProgramFilesFolder' ?>\n")
		wxi.write("<?define Win64='no' ?>\n")
		wxi.write("<?define InstallerVersion='100' ?>\n")
		wxi.write("<?define Platform='x86' ?>\n")
	wxi.write("</Include>\n")

