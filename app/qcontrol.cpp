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

#include <qqmlengine.h>
#include <QScreen>
#include <assert.h>
#include "opt.h"
#include "email.h"
#include "qcontrol.h"

QControl* QControl::_theControl;

QObject *QControl_provider(QQmlEngine *engine, QJSEngine *scriptEngine)
{
    Q_UNUSED(engine)
    Q_UNUSED(scriptEngine)
    return QControl::theControl();
}

void QControl::handleOptions(int argc, char** argv)
{
    for (int i = 1; i < argc; ++i)
    {
        if (argv[i][0] == '-')
        {
            char* arg;
            if ((arg = Opt::isOptArg(argv + i, "-d")) != 0)
                setLogLevel(atoi(arg));
            else if ((arg = Opt::isOptArg(argv + i, "-dir")) != 0)
            {
                _startDirectory = arg;
            }
            else if ((arg = Opt::isOptArg(argv + i, "-w")) != 0)
            {
                int w = atoi(arg);
                if (w > 0 && w < 10000) _uiOptionWidth = atoi(arg);
            }
            else if ((arg = Opt::isOptArg(argv + i, "-h")) != 0)
                _uiOptionHeight = atoi(arg);
            else
            {
                LOG("unknown option: ", argv[i]);
            }
        }
    }
}

void QControl::startUp(QGuiApplication* app)
{
    _app = app;

    // do init here
}

