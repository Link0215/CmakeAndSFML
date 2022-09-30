# all source files
set(src Sockets.cpp
            TCP.cpp
            UDP.cpp)

# define the sockets target
sfml_add_example(sockets
                               SOURCES ${SRC}
	              DEPENDS SFML::Network)