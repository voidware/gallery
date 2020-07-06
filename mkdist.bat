rem gets release files ready to build installer

rm -rf dist
mkdir dist
copy /Y app\release\*.dll dist
copy /Y app\release\Gallery.exe dist
cd dist
windeployqt --release --no-translations --compiler-runtime --qmldir .. .
cd ..



