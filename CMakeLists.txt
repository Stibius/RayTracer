cmake_minimum_required (VERSION 3.8)

# solution
project(RayTracer)
# project
set(APP_NAME "RayTracer")

set( CMAKE_AUTORCC ON )
set( CMAKE_AUTOUIC ON )
set( CMAKE_AUTOMOC ON )

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt5 QUIET COMPONENTS Widgets Concurrent) 
find_package(ASSIMP REQUIRED QUIET)    

find_path(ASSIMP_INCLUDE_DIR assimp/config.h HINTS ${ASSIMP_DIR}/../../../include)
find_library(ASSIMP_LIBRARY ${ASSIMP_LIBRARIES} HINTS ${ASSIMP_DIR}/../../../lib) 

set(GUI_FILES
   src/MainWindow.ui
   src/SettingsWindow.ui
)

set(RESOURCE_FILES
   src/resources.qrc 
)

set(HEADER_FILES
   src/Application.h
   src/AssimpModelLoader.h
   src/Camera.h   
   src/Color.h 
   src/ComponentShape.h
   src/CompositeShape.h
   src/ConcurrencyHandler.h
   src/Constants.h 
   src/EntityDescriptionInterface.h
   src/Image.h 
   src/Intersection.h
   src/KeyEventHandler.h
   src/LeafShape.h
   src/Light.h 
   src/MainWindow.h 
   src/Material.h 
   src/Matrix.h
   src/Mesh.h 
   src/Model.h 
   src/MouseEventHandler.h
   src/NamedEntity.h
   src/OrthoCamera.h   
   src/PerspectiveCamera.h  
   src/Plane.h
   src/Quad.h 
   src/Quadric.h
   src/Quaternion.h 
   src/Ray.h 
   src/RayTracer.h 
   src/Scene.h 
   src/SettingsIO.h
   src/SettingsWindow.h 
   src/Triangle.h 
   src/Utility.h
   src/Vector.h  
)

set(SOURCE_FILES
   src/Application.cpp
   src/AssimpModelLoader.cpp
   src/Camera.cpp  
   src/Color.cpp
   src/ComponentShape.cpp
   src/CompositeShape.cpp
   src/ConcurrencyHandler.cpp
   src/Image.cpp 
   src/Intersection.cpp
   src/KeyEventHandler.cpp
   src/LeafShape.cpp 
   src/Light.cpp 
   src/main.cpp
   src/MainWindow.cpp 
   src/Material.cpp 
   src/Matrix.cpp
   src/Mesh.cpp 
   src/Model.cpp 
   src/MouseEventHandler.cpp
   src/NamedEntity.cpp
   src/OrthoCamera.cpp  
   src/PerspectiveCamera.cpp  
   src/Plane.cpp
   src/Quad.cpp 
   src/Quadric.cpp
   src/Quaternion.cpp 
   src/Ray.cpp 
   src/RayTracer.cpp 
   src/Scene.cpp 
   src/SettingsIO.cpp
   src/SettingsWindow.cpp 
   src/Triangle.cpp 
   src/Utility.cpp
   src/Vector.cpp  
)

# Define a grouping for source files in IDE project generation
source_group("GUI Files" FILES ${GUI_FILES})

# Define a grouping for source files in IDE project generation
source_group("Resource Files" FILES ${RESOURCE_FILES})

# Define a grouping for source files in IDE project generation
source_group("Source Files" FILES ${SOURCE_FILES})

# Define a grouping for source files in IDE project generation
source_group("Header Files" FILES ${HEADER_FILES})

add_executable(${APP_NAME} ${SOURCE_FILES} ${HEADER_FILES} ${GUI_FILES} ${RESOURCE_FILES})

target_include_directories(${APP_NAME} PUBLIC ${ASSIMP_INCLUDE_DIR}
                                       )                                     

target_link_libraries(${APP_NAME} Qt5::Widgets 
                                  Qt5::Concurrent
                                  ${ASSIMP_LIBRARY}
                                  )
                     