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
- Download glm and copy the "glm" folder (which includes e.g. vec3.hpp) into host_AndroidStudio/app/src/main/cpp/glm/.

Note: glm is used as header-only library - no compilation is necessary.

- SDK manager, "SDK Platforms": Install required SDK (see build.gradle)
- SDK manager, "SDK Tools": Install required NDK (see build.gradle)
- Click on "run app" green arrow

### Linux build
to be updated (use CMakeLists.txt, replace glew32, glfw3, opengl32 library dependencies with glfw3 and GL). 

On Android, see https://github.com/android/ndk-samples/blob/main/gles3jni/app/src/main/cpp/CMakeLists.txt for possible improvements e.g. support lower versions (code is based on older gles2ini sample)

![architecture diagram](doc/architecture.png)

### Time stamps
A time stamp is delivered for selected events (Mouse / pointer / keyboard, screen redraw. NOT audio / midi callback functions. 
TBD: Provide host function to request accurate time (not "last event"-time) from any thread)

Time is reported as 64-bit value in units of nanoseconds after application start (the actual clock resolution may be worse, depending on the host / OS). The timer has sufficient bits so it will never overflow.

Time stamps are handled internally and not visible via evtGetHostToApp(). Instead, the last received timestamp (which will be the timestamp associated with an eligible event) can be read from uint64_t MNOGLA::lastTimestamp_nanosecs.

# Best practices
The project serves me to try out C++ features and develop my "handwriting". I arrived at the following insights:

## Hiding private content from API header files
A conventional class header file needs to include all protected/private members, which makes the header file less readable as API documentation.

One solution is pImpl but it seemed to clumsy and has a runtime penalty which makes it attractive for "hot" functions that the compiler would conventionally inline.

My solution: 

- For API class xyz, derive from private:class xyz_internal, which is #included by the API header. The content is not strictly hidden from the API user (the compiler still needs it for memory layout), but it needs to be explicitly opened. 
- A virtual function provided by the API class for overriding (e.g. a callback), which is called from the internal context, needs to be defined already in the internal class. The API file may redefine it for documentation purposes. Both require a dummy implementation to avoid linker error (gcc: "undefined reference to vtable").
- A method e.g. void m() is implented as void xyz::m() at API level for void xyz_internal::m() for an internal feature, which improves code readability.
- Downside: vscode Intellisense does not (yet?) hide "internal" context from autocompletion lists, if used where access is restricted.
- all internal code is organized into a subfolder (src for now, consider "internal" name?) so that the API user sees only relevant files at toplevel.