MACRO(SUBDIRLIST result curdir)
  FILE(GLOB children ${curdir} ${curdir}/*)
  SET(dirlist "")
  FOREACH(child ${children})
    IF(IS_DIRECTORY ${child})
      LIST(APPEND dirlist ${child})
    ENDIF()
  ENDFOREACH()
  SET(${result} ${dirlist})
ENDMACRO()

SUBDIRLIST(SUBDIRS .)

FOREACH(subdir ${SUBDIRS})
    message(STATUS " scan subdir : ${subdir}")
    add_subdirectory(${subdir})
ENDFOREACH()
