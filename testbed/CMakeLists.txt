project (testbed)

find_package(VTK REQUIRED)

include(${VTK_USE_FILE})

file( 
GLOB SOURCE_FILES
	*.cpp
	*.h
)

link_directories(
	${BOOST_LIB_DIR}
	)

add_executable(${PROJECT_NAME} MACOSX_BUNDLE ${SOURCE_FILES})
 
if(VTK_LIBRARIES)
  target_link_libraries(${PROJECT_NAME} 
  ${VTK_LIBRARIES}
  Newbase
  )
else()
  target_link_libraries(${PROJECT_NAME} 
  vtkHybrid
  vtkWidgets 
  Newbase
  )
endif()

target_include_directories(${PROJECT_NAME}
	PUBLIC ${Newbase_INCLUDE_DIRS}
	)

