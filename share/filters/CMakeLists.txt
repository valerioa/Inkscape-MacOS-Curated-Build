add_custom_command(
  OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/filters.svg.h
  COMMAND python ${CMAKE_CURRENT_SOURCE_DIR}/i18n.py ${CMAKE_CURRENT_SOURCE_DIR}/filters.svg > ${CMAKE_CURRENT_SOURCE_DIR}/filters.svg.h
  MAIN_DEPENDENCY ${CMAKE_CURRENT_SOURCE_DIR}/filters.svg
  DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/i18n.py
)
add_custom_target(filters_svg_h ALL DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/filters.svg.h)

install(FILES "filters.svg" "README" DESTINATION ${INKSCAPE_SHARE_INSTALL}/filters)
