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

#pragma once

#define QSPACE  "com.voidware.myapp"

#define QREGISTER(_t)  \
   qmlRegisterType<_t>(QSPACE, 1, 0, #_t)

#define QREGISTER_TYPE(_t)  \
    qmlRegisterUncreatableType<_t>(QSPACE, 1, 0, #_t, "is managed")


#define QREGISTER_SINGLE(_t) \
   extern QObject* _t ## _provider(QQmlEngine*, QJSEngine*);  \
   qmlRegisterSingletonType<_t>(QSPACE, 1, 0, #_t, _t ## _provider)

#define QCHARS(_s) QString::fromUtf8(_s)
#define QSTR(_s) QString::fromUtf8((_s).c_str())
#define QSTRC(_s) QString::fromUtf8(_s)
#define STRQ(_s) (_s).toStdString()
#define STRQC(_s) ((const char*)((_s).toUtf8().constData()))

#define OWN(_v) \
   QQmlEngine::setObjectOwnership(_v, QQmlEngine::CppOwnership)
