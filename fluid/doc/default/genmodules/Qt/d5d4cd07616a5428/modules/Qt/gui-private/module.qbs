import '../QtModule.qbs' as QtModule

QtModule {
    qtModuleName: "Gui"
    Depends { name: "Qt"; submodules: ["core-private","gui"]}

    architectures: ["x86_64"]
    targetPlatform: "windows"
    hasLibrary: false
    staticLibsDebug: []
    staticLibsRelease: []
    dynamicLibsDebug: []
    dynamicLibsRelease: []
    linkerFlagsDebug: []
    linkerFlagsRelease: []
    frameworksDebug: []
    frameworksRelease: []
    frameworkPathsDebug: []
    frameworkPathsRelease: []
    libNameForLinkerDebug: undefined
    libNameForLinkerRelease: undefined
    libFilePathDebug: undefined
    libFilePathRelease: undefined
    pluginTypes: []
    moduleConfig: []
    cpp.defines: []
    cpp.includePaths: ["D:/Qt/Qt5.12.8/5.12.8/mingw73_64/include/QtGui/5.12.8","D:/Qt/Qt5.12.8/5.12.8/mingw73_64/include/QtGui/5.12.8/QtGui"]
    cpp.libraryPaths: []
    
}
