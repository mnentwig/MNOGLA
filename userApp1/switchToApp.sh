echo "installing build files for this application."
echo "next:"
echo "	cd host_xyz (repeat for each platform xyz)"
echo "	cmake -S . -B build"
echo "	cmake --build build"

cp -f buildFiles/android_CMakeLists.txt ../host_AndroidStudio/app/src/main/cpp/CMakeLists.txt
cp -f buildFiles/mingw64linux_CMakeLists.txt ../host_mingw64linux/CMakeLists.txt
cp -f buildFiles/build.gradle ../host_AndroidStudio/app/
