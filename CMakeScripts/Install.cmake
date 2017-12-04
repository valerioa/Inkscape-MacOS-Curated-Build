if(UNIX)
    #The install directive for the binaries and libraries are found in src/CMakeList.txt
    install(FILES
      ${CMAKE_BINARY_DIR}/inkscape.desktop
      DESTINATION ${CMAKE_INSTALL_PREFIX}/${SHARE_INSTALL}/applications)
endif()

if(WIN32)
  install(FILES
    AUTHORS
    COPYING
    NEWS
    README
    TRANSLATORS
    DESTINATION .)

  install(FILES
    GPL2.txt
    GPL3.txt
    LGPL2.1.txt
    DESTINATION .)

  install(DIRECTORY doc
    DESTINATION .)
    
  # devlibs and mingw dlls

  # There are differences in the devlibs for 64-Bit and 32-Bit build environments.
  if(HAVE_MINGW64)
    install(FILES
      ${DEVLIBS_BIN}/bz2-1.dll
      ${DEVLIBS_BIN}/icudt56.dll
      ${DEVLIBS_BIN}/icuin56.dll
      ${DEVLIBS_BIN}/icuuc56.dll
      ${DEVLIBS_BIN}/libMagick++-6.Q16-6.dll
      ${DEVLIBS_BIN}/libMagickCore-6.Q16-2.dll
      ${DEVLIBS_BIN}/libMagickWand-6.Q16-2.dll
      ${DEVLIBS_BIN}/libaspell-15.dll
      ${DEVLIBS_BIN}/libatk-1.0-0.dll
      ${DEVLIBS_BIN}/libatkmm-1.6-1.dll
      ${DEVLIBS_BIN}/libcairo-2.dll
      ${DEVLIBS_BIN}/libcairomm-1.0-1.dll
      ${DEVLIBS_BIN}/libcdr-0.1.dll
      ${DEVLIBS_BIN}/libcurl-4.dll
      ${DEVLIBS_BIN}/libexif-12.dll
      ${DEVLIBS_BIN}/libexpat-1.dll
      ${DEVLIBS_BIN}/libexslt-0.dll
      ${DEVLIBS_BIN}/libffi-6.dll
      ${DEVLIBS_BIN}/libfontconfig-1.dll
      ${DEVLIBS_BIN}/libfreetype-6.dll
      ${DEVLIBS_BIN}/libgc-1.dll
      ${DEVLIBS_BIN}/libgdk-win32-2.0-0.dll
      ${DEVLIBS_BIN}/libgdk_pixbuf-2.0-0.dll
      ${DEVLIBS_BIN}/libgdkmm-2.4-1.dll
      ${DEVLIBS_BIN}/libgio-2.0-0.dll
      ${DEVLIBS_BIN}/libgiomm-2.4-1.dll
      ${DEVLIBS_BIN}/libglib-2.0-0.dll
      ${DEVLIBS_BIN}/libglibmm-2.4-1.dll
      ${DEVLIBS_BIN}/libgmodule-2.0-0.dll
      ${DEVLIBS_BIN}/libgobject-2.0-0.dll
      ${DEVLIBS_BIN}/libgsl-19.dll
      ${DEVLIBS_BIN}/libgslcblas-0.dll
      ${DEVLIBS_BIN}/libgthread-2.0-0.dll
      ${DEVLIBS_BIN}/libgtk-win32-2.0-0.dll
      ${DEVLIBS_BIN}/libgtkmm-2.4-1.dll
      ${DEVLIBS_BIN}/libharfbuzz-0.dll
      ${DEVLIBS_BIN}/libiconv-2.dll
      ${DEVLIBS_BIN}/libintl-8.dll
      ${DEVLIBS_BIN}/libjpeg-9.dll
      ${DEVLIBS_BIN}/liblcms2-2.dll
      ${DEVLIBS_BIN}/liblzma-5.dll
      ${DEVLIBS_BIN}/libpango-1.0-0.dll
      ${DEVLIBS_BIN}/libpangocairo-1.0-0.dll
      ${DEVLIBS_BIN}/libpangoft2-1.0-0.dll
      ${DEVLIBS_BIN}/libpangomm-1.4-1.dll
      ${DEVLIBS_BIN}/libpangowin32-1.0-0.dll
      ${DEVLIBS_BIN}/libpixman-1-0.dll
      ${DEVLIBS_BIN}/libpng16-16.dll
      ${DEVLIBS_BIN}/libpoppler-58.dll
      ${DEVLIBS_BIN}/libpoppler-glib-8.dll
      ${DEVLIBS_BIN}/libpopt-0.dll
      ${DEVLIBS_BIN}/libpotrace-0.dll
      ${DEVLIBS_BIN}/librevenge-0.0.dll
      ${DEVLIBS_BIN}/librevenge-stream-0.0.dll
      ${DEVLIBS_BIN}/libsigc-2.0-0.dll
      ${DEVLIBS_BIN}/libtiff-5.dll
      ${DEVLIBS_BIN}/libvisio-0.1.dll
      ${DEVLIBS_BIN}/libwpd-0.10.dll
      ${DEVLIBS_BIN}/libwpg-0.3.dll
      ${DEVLIBS_BIN}/libxml2-2.dll
      ${DEVLIBS_BIN}/libxslt-1.dll
      ${DEVLIBS_BIN}/zlib1.dll
      ${MINGW_BIN}/libstdc++-6.dll
      ${MINGW_BIN}/libwinpthread-1.dll
      ${MINGW_BIN}/libgcc_s_seh-1.dll
      ${MINGW_BIN}/libgomp-1.dll
      DESTINATION .)
  else()
    install(FILES
      ${DEVLIBS_BIN}/bzip2.dll
      ${DEVLIBS_BIN}/freetype6.dll
      ${DEVLIBS_BIN}/iconv.dll
      ${DEVLIBS_BIN}/icudata50.dll
      ${DEVLIBS_BIN}/icui18n50.dll
      ${DEVLIBS_BIN}/icuuc50.dll
      ${DEVLIBS_BIN}/intl.dll
      ${DEVLIBS_BIN}/libMagick++-3.dll
      ${DEVLIBS_BIN}/libMagickCore-3.dll
      ${DEVLIBS_BIN}/libMagickWand-3.dll
      ${DEVLIBS_BIN}/libatk-1.0-0.dll
      ${DEVLIBS_BIN}/libatkmm-1.6-1.dll
      ${DEVLIBS_BIN}/libcairo-2.dll
      ${DEVLIBS_BIN}/libcairomm-1.0-1.dll
      ${DEVLIBS_BIN}/libcdr-0.1.dll
      ${DEVLIBS_BIN}/libexif-12.dll
      ${DEVLIBS_BIN}/libexpat-1.dll
      ${DEVLIBS_BIN}/libexslt.dll
      ${DEVLIBS_BIN}/libfontconfig-1.dll
      ${DEVLIBS_BIN}/libgcc_s_sjlj-1.dll
      ${DEVLIBS_BIN}/libgdk-win32-2.0-0.dll
      ${DEVLIBS_BIN}/libgdk_pixbuf-2.0-0.dll
      ${DEVLIBS_BIN}/libgdkmm-2.4-1.dll
      ${DEVLIBS_BIN}/libgio-2.0-0.dll
      ${DEVLIBS_BIN}/libgiomm-2.4-1.dll
      ${DEVLIBS_BIN}/libglib-2.0-0.dll
      ${DEVLIBS_BIN}/libglibmm-2.4-1.dll
      ${DEVLIBS_BIN}/libgmodule-2.0-0.dll
      ${DEVLIBS_BIN}/libgobject-2.0-0.dll
      ${DEVLIBS_BIN}/libgthread-2.0-0.dll
      ${DEVLIBS_BIN}/libgtk-win32-2.0-0.dll
      ${DEVLIBS_BIN}/libgtkmm-2.4-1.dll
      ${DEVLIBS_BIN}/libintl-8.dll
      ${DEVLIBS_BIN}/libjpeg-7.dll
      ${DEVLIBS_BIN}/liblcms-1.dll
      ${DEVLIBS_BIN}/liblcms2-2.dll
      ${DEVLIBS_BIN}/libopenjpeg-2.dll
      ${DEVLIBS_BIN}/libpango-1.0-0.dll
      ${DEVLIBS_BIN}/libpangocairo-1.0-0.dll
      ${DEVLIBS_BIN}/libpangoft2-1.0-0.dll
      ${DEVLIBS_BIN}/libpangomm-1.4-1.dll
      ${DEVLIBS_BIN}/libpangowin32-1.0-0.dll
      ${DEVLIBS_BIN}/libpixman-1-0.dll
      ${DEVLIBS_BIN}/libpng12-0.dll
      ${DEVLIBS_BIN}/libpng14-14.dll
      ${DEVLIBS_BIN}/libpoppler-58.dll
      ${DEVLIBS_BIN}/libpoppler-glib-8.dll
      ${DEVLIBS_BIN}/libpopt-0.dll
      ${DEVLIBS_BIN}/librevenge-0.0.dll
      ${DEVLIBS_BIN}/librevenge-stream-0.0.dll
      ${DEVLIBS_BIN}/libsigc-2.0-0.dll
      ${DEVLIBS_BIN}/libtiff-3.dll
      ${DEVLIBS_BIN}/libvisio-0.1.dll
      ${DEVLIBS_BIN}/libwpd-0.9.dll
      ${DEVLIBS_BIN}/libwpd-stream-0.9.dll
      ${DEVLIBS_BIN}/libwpg-0.2.dll
      ${DEVLIBS_BIN}/libxml2.dll
      ${DEVLIBS_BIN}/libxslt.dll
      ${DEVLIBS_BIN}/msvcr70.dll
      ${DEVLIBS_BIN}/msvcr71.dll
      ${DEVLIBS_BIN}/pthreadGC2.dll
      ${DEVLIBS_BIN}/zlib1.dll
      ${MINGW_BIN}/mingwm10.dll
      ${MINGW_BIN}/libgomp-1.dll
      DESTINATION .)
  endif()

  # Generate a dummy file in hicolor/index.theme to avoid bug 1635207
  file(GENERATE OUTPUT share/icons/hicolor/index.theme
    CONTENT "[Icon Theme]\nName=hicolor\nDirectories=")

  install(DIRECTORY ${DEVLIBS_PATH}/share/themes
    DESTINATION share)

  install(DIRECTORY ${DEVLIBS_PATH}/share/locale
    DESTINATION share
    PATTERN "*gtk30.mo" EXCLUDE)

  install(DIRECTORY ${DEVLIBS_PATH}/share/poppler
    DESTINATION share)

  install(DIRECTORY ${DEVLIBS_PATH}/etc/fonts
    DESTINATION etc)

  install(DIRECTORY ${DEVLIBS_PATH}/etc/gtk-2.0
    DESTINATION etc)

  # GTK 2.0
  install(DIRECTORY ${DEVLIBS_LIB}/gtk-2.0
    DESTINATION lib
    FILES_MATCHING
    PATTERN "*.dll"
    PATTERN "*.cache")

  install(DIRECTORY ${DEVLIBS_LIB}/gdk-pixbuf-2.0
    DESTINATION lib
    FILES_MATCHING
    PATTERN "*.dll"
    PATTERN "*.cache")

  # Aspell dictionaries
  install(DIRECTORY ${DEVLIBS_LIB}/aspell-0.60
    DESTINATION lib)

  # Necessary to run extensions on windows if it is not in the path
  if (HAVE_MINGW64)
    install(FILES
      ${DEVLIBS_BIN}/gspawn-win64-helper.exe
      ${DEVLIBS_BIN}/gspawn-win64-helper-console.exe
      DESTINATION .)
  else()
    install(FILES
      ${DEVLIBS_BIN}/gspawn-win32-helper.exe
      ${DEVLIBS_BIN}/gspawn-win32-helper-console.exe
      DESTINATION .)
  endif()

  # Perl
  install(FILES
    ${DEVLIBS_PATH}/perl/bin/perl58.dll
    DESTINATION .)

  # Python
  install(DIRECTORY ${DEVLIBS_PATH}/python
    DESTINATION .
    PATTERN "python/include" EXCLUDE
    PATTERN "python/libs" EXCLUDE
    PATTERN "*.pyc" EXCLUDE)
endif()