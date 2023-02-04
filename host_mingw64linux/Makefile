CFLAGS 	:=	-Wall -fmax-errors=1 -pedantic -Wextra -Wno-unused -O -DMNOGLA_WINDOWS
all: host.exe
setupMsys:
# largely for documentation purposes as it would need at least "make"
	pacman -S --needed base-devel mingw-w64-x86_64-toolchain
	pacman -S mingw-w64-x86_64-glm
	pacman -S mingw-w64-x86_64-glfw
	pacman -S mingw-w64-x86_64-glew

MNOGLA.o: ../core/MNOGLA.cpp ../MNOGLA.h
	g++ -c ${CFLAGS} ../core/MNOGLA.cpp

MNOGLA_util.o: ../core/MNOGLA_util.h ../core/MNOGLA_util.cpp ../MNOGLA.h
	g++ -c ${CFLAGS} ../core/MNOGLA_util.cpp

MNOGLA_core.a: MNOGLA.o MNOGLA_util.o
	ar rcs $@ $^

userApp.o: ../userApp/userApp.cpp ../MNOGLA.h ../core/MNOGLA_util.h
	g++ -c ${CFLAGS} ../userApp/userApp.cpp

userApp.a: userApp.o 
	ar rcs $@ $^

host.exe: host.cpp MNOGLA_core.a userApp.a
	g++ -o host.exe ${CFLAGS} host.cpp userApp.a MNOGLA_core.a -lglew32 -lglfw3 -lopengl32 

main: userApp.o MNOGLA.o ../MNOGLA.h
	g++ -omain ${CFLAGS} main.cpp userApp.o MNOGLA.o -lGL -lglfw3
clean:
	rm -Rf main.exe userApp.o MNOGLA.o

.PHONY: setupMsys clean main.exe 
