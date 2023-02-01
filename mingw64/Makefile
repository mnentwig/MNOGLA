CFLAGS 	:=	-Wall -fmax-errors=1 -pedantic -Wextra -Wno-unused -O -DMNOGLA_WINDOWS
all: main.exe
setupMsys:
# largely for documentation purposes as it would need at least "make"
	pacman -S --needed base-devel mingw-w64-x86_64-toolchain
	pacman -S mingw-w64-x86_64-glm
	pacman -S mingw-w64-x86_64-glfw
	pacman -S mingw-w64-x86_64-glew
	pacman -S mingw-w64-x86_64-angleproject

MNOGLA.o: ../MNOGLA.cpp ../MNOGLA.h
	g++ -c ${CFLAGS} ../MNOGLA.cpp

userApp.o: ../userApp/userApp.cpp ../MNOGLA.h
	g++ -c ${CFLAGS} ../userApp/userApp.cpp

main.exe: userApp.o MNOGLA.o ../MNOGLA.h
	g++ -omain.exe ${CFLAGS} main.cpp userApp.o MNOGLA.o -lglew32 -lglfw3 -lopengl32 
main: userApp.o MNOGLA.o ../MNOGLA.h
	g++ -omain ${CFLAGS} main.cpp userApp.o MNOGLA.o -lGL -lglfw3
clean:
	rm -Rf main.exe userApp.o MNOGLA.o

.PHONY: setupMsys clean main.exe 
