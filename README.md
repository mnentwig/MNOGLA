# MNOGLA
MN's OS-independent Open-GL application host

A smallest-common-denominator framework for an OS-independent openGL app. Runs on
- Windows via mingw, using GLEW.
- Linux (tested: Raspberry Pi).
- Android. Use Android Studio (tested: 2022.1.1)

### Windows (minGW) build
```pacman -S --needed base-devel mingw-w64-x86_64-toolchain
pacman -S mingw-w64-x86_64-glm
pacman -S mingw-w64-x86_64-glfw
pacman -S mingw-w64-x86_64-glew
cd host_mingw64linux
cmake -S . -B build
cmake --build build
```

### Android Studio build
SDK manager, "SDK Platforms": Install required SDK (see build.gradle)
SDK manager, "SDK Tools": Install required NDK (see build.gradle)
Click on "run app" green arrow

### Linux build
to be updated (use CMakeLists.txt, replace glew32, glfw3, opengl32 library dependencies with glfw3 and GL). 

On Android, see https://github.com/android/ndk-samples/blob/main/gles3jni/app/src/main/cpp/CMakeLists.txt for possible improvements e.g. support lower versions (code is based on older gles2ini sample)

![architecture diagram](doc/architecture.png)

### Time stamps
A time stamp is delivered for selected events (Mouse / pointer / keyboard, screen redraw. NOT audio / midi callback functions. 
TBD: Provide host function to request time from any thread)

Time is reported as 64-bit value in units of nanoseconds after application start (the actual clock resolution may be worse, depending on the host / OS). The timer has sufficient bits so it will never overflow.

Time stamps are handled internally and not visible via evtGetHostToApp(). Instead, the last received timestamp (which will be the timestamp associated with an eligible event) can be read from uint64_t MNOGLA::lastTimestamp_nanosecs.