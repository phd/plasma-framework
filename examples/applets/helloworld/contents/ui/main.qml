// -*- coding: iso-8859-1 -*-
/*
    SPDX-FileCopyrightText: 2012 Marco Martin <mart@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

import QtQuick 2.0
import org.kde.plasma.plasmoid 2.0
import QtQuick.Layouts 1.0
import org.kde.plasma.components 3.0 as PC3

Item {
    Layout.minimumWidth: 200
    Layout.minimumHeight: 300

    PC3.Label {
        text: i18n("Hello world")
    }
}
