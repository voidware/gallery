/*
 * This file is part of Fluid.
 *
 * Copyright (C) 2018 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
 *
 * $BEGIN_LICENSE:MPL2$
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * $END_LICENSE$
 */

#ifndef CONTROLSUTILS_H
#define CONTROLSUTILS_H

#include <QObject>
#include <QUrl>

class ControlsUtils : public QObject
{
    Q_OBJECT
public:
    explicit ControlsUtils(const QUrl &baseUrl, QObject *parent = nullptr);

    Q_INVOKABLE QUrl iconUrl(const QString &name);

private:
    QUrl m_baseUrl;
};

#endif // CONTROLSUTILS_H
