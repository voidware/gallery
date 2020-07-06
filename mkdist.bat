rem gets release files ready to build installer

rm -rf gallery
mkdir gallery
rem copy /Y app\release\*.dll gallery
copy /Y app\release\Gallery.exe gallery
cd gallery
windeployqt --release --no-translations --compiler-runtime --qmldir .. .
cd ..



