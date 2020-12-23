rem gets release files ready to build installer

rm -rf gallery
mkdir gallery
copy /Y app\release\Gallery.exe gallery
cd gallery
windeployqt --no-translations --compiler-runtime --no-webkit2 --no-virtualkeyboard --qmldir .. .
xcopy /E /I ..\fluid\qml\Fluid Fluid
cd ..



