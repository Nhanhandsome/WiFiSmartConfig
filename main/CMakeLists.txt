# See the build system documentation in IDF programming guide
# for more information about component CMakeLists.txt files.

file(GLOB board_source "board/*.cpp")
file(GLOB components_source  "components/*.cpp")
file(GLOB services_source "service/*.cpp")	
file(GLOB app_source "app/*.cpp")	


idf_component_register(
    SRCS main.c    
    	 main.cpp
    	 ${board_source}
    	 ${components_source}
    	 ${services_source}
    	 ${app_source}     # list the source files of this component
    INCLUDE_DIRS        "app"
    					"board"
    					"component"
    					"service"
    					# optional, add here public include directories
    PRIV_INCLUDE_DIRS   # optional, add here private include directories
    REQUIRES            # optional, list the public requirements (component names)
    PRIV_REQUIRES       # optional, list the private requirements
)
