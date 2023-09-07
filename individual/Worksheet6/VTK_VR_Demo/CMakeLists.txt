#
# Basic VTK Cylinder Example with VR output
# Taken from: https://kitware.github.io/vtk-examples/site/Cxx/GeometricObjects/CylinderExample/
#

cmake_minimum_required( VERSION 3.12 FATAL_ERROR )

project( vtkexample_vr1 )

find_package( VTK COMPONENTS 
  CommonColor
  CommonCore
  FiltersSources
  InteractionStyle
  RenderingContextOpenGL2
  RenderingCore
  RenderingFreeType
  RenderingGL2PSOpenGL2
  RenderingOpenGL2
  RenderingOpenVR               # This is new
)

if( NOT VTK_FOUND )
    message( FATAL_ERROR "CylinderExample: Unable to find the VTK build folder." )
endif()

# Prevent a "command line is too long" failure in Windows.
set( CMAKE_NINJA_FORCE_RESPONSE_FILE "ON" CACHE BOOL "Force Ninja to use response files." )

# Define executable
add_executable( vtkexample_vr1 MACOSX_BUNDLE vtkvr.cpp )

# Link to VTK libraries
target_link_libraries( vtkexample_vr1 PRIVATE ${VTK_LIBRARIES} )

# vtk_module_autoinit is needed
vtk_module_autoinit( TARGETS vtkexample_vr1 MODULES ${VTK_LIBRARIES} )

# Copy across Open VR config files
# These exist in VTK bin dir but for some reason VR classes expect them to be in local dir
add_custom_target(  VRBindings )
add_custom_command( TARGET VRBindings PRE_BUILD
        COMMAND ${CMAKE_COMMAND} -E
        copy_directory ${CMAKE_SOURCE_DIR}/vrbindings ${CMAKE_BINARY_DIR}/ )
