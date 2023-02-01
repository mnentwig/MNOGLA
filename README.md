# MNOGLA
MN's OS-independent Open-GL application host

A smallest-common-denominator framework for an OS-independent openGL app. Runs on
- Windows via mingw, using GLEW. Run 'make main.exe'
- Linux (tested: Raspberry Pi). Run 'make main'
- Android. Use Android Studio (tested: 2022.1.1)

Folder structure:
- toplevel: MNOGLA.h defines interface
- mingw: Windows-/linux host
- Android: Android host

"Host" meaning OS-dependent code to convert e.g. mouse events to messages.

On Android, see https://github.com/android/ndk-samples/blob/main/gles3jni/app/src/main/cpp/CMakeLists.txt for possible improvements e.g. support lower versions (code is based on older gles2ini sample)
