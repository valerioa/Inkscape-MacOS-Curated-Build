# use this to update the translated language strings in the *_languageNames.nsh files
# translated language names are extracted from the .po files in the /po directory

from __future__ import print_function
from __future__ import unicode_literals  # make all literals unicode strings by default (even in Python 2)

import os
import re
import sys
from io import open  # needed for support of encoding parameter in Python 2

from _polib import pofile


# get the list of available Inkscape UI translations (by parsing inkscape-preferences.cpp)
sys.path.append('../../wix/')
from helpers import get_inkscape_locales_and_names
inkscape_locales = get_inkscape_locales_and_names()

# get the list of translations offered in the installer (by parsing _language_lists.nsh)
re_installer_locales = re.compile(r'!macro INKSCAPE_TRANSLATIONS _MACRONAME\n(.+?)!macroend', re.DOTALL)
re_installer_locale = re.compile(r'[ \t]+!insertmacro \${_MACRONAME}[ \t]+([\w]+)[ \t]+([\w@]+)\n')
with open('_language_lists.nsh', 'r') as f:
    installer_locales = re.search(re_installer_locales, f.read())
if installer_locales:
    installer_locales = re.finditer(re_installer_locale, installer_locales.group(1))
    if installer_locales:
        installer_locales = {
          installer_locale.group(2): installer_locale.group(1) for installer_locale in installer_locales
        }
if not installer_locales:
    print("Could not get the list of Inkscape translations from _language_lists.nsh")
    sys.exit(1)
installer_locales['en'] = 'English'


# check if the lists are in sync
for installer_locale in installer_locales:
    if installer_locale not in inkscape_locales:
        print(
            "Error: Locale '" + installer_locale +
            "' is present in '_language_lists.nsh' but is not a selectable Inkscape translation!"
        )
        sys.exit(1)
for inkscape_locale in inkscape_locales:
    if inkscape_locale not in installer_locales:
        print(
            "Warning: Locale '" + inkscape_locale +
            "' is a selectable Inkscape translation but not present in '_language_lists.nsh'\n" +
            "         You should consider adding it."
        )


# extract translation names from .po files for locales offered in the installer
for locale in sorted(installer_locales):
    if os.path.isfile(installer_locales[locale] + '.nsh'):
        print("Getting translation names for " + inkscape_locales[locale])
        filename = installer_locales[locale] + '_languageNames.nsh'
        with open(filename, 'w', encoding='utf-8', newline='\n') as f:
            f.write(
                "### " + inkscape_locales[locale] + " translations for language names\n" +
                "# This file has been automatically created by '" + __file__ + "', do not edit it manually!\n"
                "# Strings that should be translated are in '" + installer_locales[locale] + ".nsh'\n")
            if locale is 'en':
                for locale in sorted(installer_locales):
                    locale_name = inkscape_locales[locale]
                    f.write('${LangFileString} lng_' + locale + ' ')
                    f.write('"' + locale_name + '"\n')
            else:
                po_filename = '../../../po/' + locale + '.po'
                po = pofile(po_filename)
                for locale in sorted(installer_locales):
                    po_entry = po.find(inkscape_locales[locale])
                    if po_entry.translated():
                        locale_name = po_entry.msgstr
                    else:
                        locale_name = inkscape_locales[locale]
                    f.write('${LangFileString} lng_' + locale + ' ')
                    f.write('"' + locale_name + '"\n')
