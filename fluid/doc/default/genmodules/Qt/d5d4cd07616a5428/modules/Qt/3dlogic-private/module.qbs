import '../QtModule.qbs' as QtModule

QtModule {
    qtModuleName: "3DLogic"
    Depends { name: "Qt"; submodules: ["core-private","gui-private","3dcore-private","3dlogic"]}

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
    cpp.includePaths: ["D:/Qt/Qt5.12.8/5.12.8/mingw73_64/include/Qt3DLogic/5.12.8","D:/Qt/Qt5.12.8/5.12.8/mingw73_64/include/Qt3DLogic/5.12.8/Qt3DLogic"]
    cpp.libraryPaths: []
    
}
