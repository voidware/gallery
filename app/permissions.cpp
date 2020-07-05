/**
 *
 *    _    __        _      __                           
 *   | |  / /____   (_)____/ /_      __ ____ _ _____ ___ 
 *   | | / // __ \ / // __  /| | /| / // __ `// ___// _ \
 *   | |/ // /_/ // // /_/ / | |/ |/ // /_/ // /   /  __/
 *   |___/ \____//_/ \__,_/  |__/|__/ \__,_//_/    \___/ 
 *                                                       
 * Copyright (c) 2020 Voidware Ltd.
 *
 * This program is free software: you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License (LGPL) as published
 * by the Free Software Foundation, either version 3 of the License, or (at
 * your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License
 * for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 * 
 * contact@voidware.com
 */

// The following resources where most helpful in implementing this permission request code insert
// https://doc.qt.io/qt-5/qtandroid.html
// http://falsinsoft.blogspot.com.by/2015/03/qt-android-interface-c-and-java-through.html
// http://www.kdab.com/qt-android-episode-5/
// http://www.kdab.com/qt-android-episode-6/
// https://www.kdab.com/qt-android-episode-7/
// http://stackoverflow.com/questions/35027043/implementing-android-6-0-permissions-in-unity3d
// http://stackoverflow.com/questions/32347532/android-m-permissions-confused-on-the-usage-of-shouldshowrequestpermissionrati
// https://developer.android.com/training/permissions/requesting.html
// https://inthecheesefactory.com/blog/things-you-need-to-know-about-android-m-permission-developer-edition/en
// https://gist.github.com/patrickhammond/47cff299bc34e976e18b
// https://github.com/codepath/android_guides/wiki/Understanding-App-Permissions
// http://doc.qt.io/qt-5/qandroidjniobject.html



// Object to request permissions from the Android system
#include "permissions.h"
//#include <QMessageBox>
//#include <QApplication>

#define P_READ "android.permission.READ_EXTERNAL_STORAGE"

// Method to request permissions
bool Permissions::requestExternalStoragePermission()
{
    _readPermissionResult = true;
    
#if defined(Q_OS_ANDROID)
    QtAndroid::PermissionResult request = QtAndroid::checkPermission(P_READ);

    if (request == QtAndroid::PermissionResult::Denied)
    {
        _readPermissionResult = false;
        
        QtAndroid::requestPermissionsSync(QStringList() <<  P_READ);
        request = QtAndroid::checkPermission(P_READ);

        if (request == QtAndroid::PermissionResult::Denied)
        {
            if (QtAndroid::shouldShowRequestPermissionRationale(P_READ))
            {
#if 0
                ShowPermissionRationale = QAndroidJniObject("org/bytran/bytran/ShowPermissionRationale",
                                                            "(Landroid/app/Activity;)V",
                                                            QtAndroid::androidActivity().object<jobject>()
                                                            );

                // Checking for errors in the JNI
                QAndroidJniEnvironment env;
                if (env->ExceptionCheck()) {
                    // Handle exception here.
                    env->ExceptionClear();
                }
#endif
            }
        }
        else
        { 
            _readPermissionResult = true;
        }
    }
#endif

    return _readPermissionResult;
}

