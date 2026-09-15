
if (NOT EXISTS "C:/Users/chanw/OneDrive/Desktop/과제/2학년/컴퓨터 그래픽스/Project2/vcpkg_installed/x64-windows/vcpkg/blds/glfw3/x64-windows-dbg/install_manifest.txt")
    message(FATAL_ERROR "Cannot find install manifest: \"C:/Users/chanw/OneDrive/Desktop/과제/2학년/컴퓨터 그래픽스/Project2/vcpkg_installed/x64-windows/vcpkg/blds/glfw3/x64-windows-dbg/install_manifest.txt\"")
endif()

file(READ "C:/Users/chanw/OneDrive/Desktop/과제/2학년/컴퓨터 그래픽스/Project2/vcpkg_installed/x64-windows/vcpkg/blds/glfw3/x64-windows-dbg/install_manifest.txt" files)
string(REGEX REPLACE "\n" ";" files "${files}")

foreach (file ${files})
  message(STATUS "Uninstalling \"$ENV{DESTDIR}${file}\"")
  if (EXISTS "$ENV{DESTDIR}${file}")
    exec_program("C:/Users/chanw/AppData/Local/vcpkg/downloads/tools/cmake-4.3.2-windows/cmake-4.3.2-windows-x86_64/bin/cmake.exe" ARGS "-E remove \"$ENV{DESTDIR}${file}\""
                 OUTPUT_VARIABLE rm_out
                 RETURN_VALUE rm_retval)
    if (NOT "${rm_retval}" STREQUAL 0)
      MESSAGE(FATAL_ERROR "Problem when removing \"$ENV{DESTDIR}${file}\"")
    endif()
  elseif (IS_SYMLINK "$ENV{DESTDIR}${file}")
    EXEC_PROGRAM("C:/Users/chanw/AppData/Local/vcpkg/downloads/tools/cmake-4.3.2-windows/cmake-4.3.2-windows-x86_64/bin/cmake.exe" ARGS "-E remove \"$ENV{DESTDIR}${file}\""
                 OUTPUT_VARIABLE rm_out
                 RETURN_VALUE rm_retval)
    if (NOT "${rm_retval}" STREQUAL 0)
      message(FATAL_ERROR "Problem when removing symlink \"$ENV{DESTDIR}${file}\"")
    endif()
  else()
    message(STATUS "File \"$ENV{DESTDIR}${file}\" does not exist.")
  endif()
endforeach()

