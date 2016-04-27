/******************************************************************************
*   Copyright 2016 David Rosca <nowrep@gmail.com>                             *
*                                                                             *
*   This library is free software; you can redistribute it and/or             *
*   modify it under the terms of the GNU Library General Public               *
*   License as published by the Free Software Foundation; either              *
*   version 2 of the License, or (at your option) any later version.          *
*                                                                             *
*   This library is distributed in the hope that it will be useful,           *
*   but WITHOUT ANY WARRANTY; without even the implied warranty of            *
*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU          *
*   Library General Public License for more details.                          *
*                                                                             *
*   You should have received a copy of the GNU Library General Public License *
*   along with this library; see the file COPYING.LIB.  If not, write to      *
*   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,      *
*   Boston, MA 02110-1301, USA.                                               *
*******************************************************************************/

#include "iconitemtest.h"

#include <QIcon>
#include <QQmlEngine>
#include <QQmlContext>
#include <QQmlComponent>
#include <QQuickItemGrabResult>
#include <QtTest/QSignalSpy>

#include <KIconLoader>
#include <KIconEngine>
#include <KIconTheme>

#include "plasma/theme.h"
#include "plasma/svg.h"

static bool imageIsEmpty(const QImage &img)
{
    for (int i = 0; i < img.width(); ++i) {
        for (int j = 0; j < img.height(); ++j) {
            if (img.pixel(i, j) != 0) {
                return false;
            }
        }
    }
    return true;
}

void IconItemTest::initTestCase()
{
    // make our theme in search path
    qputenv("XDG_DATA_DIRS",
            qgetenv("XDG_DATA_DIRS") + ":" + QFINDTESTDATA("data").toLocal8Bit());

    // set default icon theme to test-theme
    QStandardPaths::setTestModeEnabled(true);

    QString configPath = QStandardPaths::writableLocation(QStandardPaths::GenericConfigLocation);

    if(!QDir(configPath).mkpath(QStringLiteral("."))) {
        qFatal("Failed to create test configuration directory.");
    }

    QFile::remove(configPath);

    QIcon::setThemeSearchPaths({QFINDTESTDATA("data/icons")});
    QIcon::setThemeName("test-theme");

    KConfigGroup cg(KSharedConfig::openConfig(), "Icons");
    cg.writeEntry("Theme", "test-theme");
    KConfigGroup plasmaConfig(KSharedConfig::openConfig("plasmarc"), "Theme");
    plasmaConfig.writeEntry("name", "default");

    cg.sync(); // sync explicitly so it will be correctly parsed
    KSharedConfig::openConfig()->reparseConfiguration();
    KIconTheme::reconfigure();
    KIconLoader::global()->reconfigure(QString());

    m_view = new QQuickView();
    m_view->setSource(QUrl::fromLocalFile(QFINDTESTDATA("data/view.qml")));
    m_view->show();
    QTest::qWaitForWindowExposed(m_view);

    if (!m_view->rootObject() || !m_view->rootObject()->grabToImage()) {
        QSKIP("Cannot grab item to image.");
    }
}

void IconItemTest::cleanupTestCase()
{
    delete m_view;
}

void IconItemTest::cleanup()
{
    qDeleteAll(m_view->rootObject()->childItems());
}

QQuickItem *IconItemTest::createIconItem()
{
    QByteArray iconQml =
        "import QtQuick 2.0;"
        "import org.kde.plasma.core 2.0 as PlasmaCore;"
        "PlasmaCore.IconItem {"
        "    id: root;"
        "}";

    QQmlComponent component(m_view->engine());

    QSignalSpy spy(&component, SIGNAL(statusChanged(QQmlComponent::Status)));
    component.setData(iconQml, QUrl("test://iconTest"));
    if (component.status() != QQmlComponent::Ready) {
        spy.wait();
    }

    QQuickItem *item = qobject_cast<QQuickItem*>(component.create(m_view->engine()->rootContext()));
    Q_ASSERT(item && qstrcmp(item->metaObject()->className(), "IconItem") == 0);
    item->setParentItem(m_view->rootObject());
    return item;
}

QImage IconItemTest::grabImage(QQuickItem *item)
{
    QSharedPointer<QQuickItemGrabResult> grab = item->grabToImage();
    QSignalSpy spy(grab.data(), SIGNAL(ready()));
    spy.wait();
    return grab->image();
}

Plasma::Svg *IconItemTest::findPlasmaSvg(QQuickItem *item)
{
    return item->findChild<Plasma::Svg *>();
}

void IconItemTest::changeTheme(Plasma::Theme *theme, const QString &themeName)
{
    if (theme->themeName() != themeName) {
        QSignalSpy spy(theme, SIGNAL(themeChanged()));
        theme->setThemeName(themeName);
        spy.wait();
    }
}

// ------ Tests

void IconItemTest::loadPixmap()
{
    QScopedPointer<QQuickItem> item(createIconItem());
    QPixmap sourcePixmap(QFINDTESTDATA("data/test_image.png"));

    item->setProperty("source", sourcePixmap);
    QVERIFY(item->property("valid").toBool());

    QImage capture = grabImage(item.data());
    QCOMPARE(capture, sourcePixmap.toImage().convertToFormat(QImage::Format_ARGB32_Premultiplied));
    QCOMPARE(sourcePixmap, item->property("source").value<QPixmap>());
}

//tests setting icon from a QImage
void IconItemTest::loadImage()
{
    QScopedPointer<QQuickItem> item(createIconItem());
    QImage sourceImage(QFINDTESTDATA("data/test_image.png"));

    item->setProperty("source", sourceImage);
    QVERIFY(item->property("valid").toBool());

    QImage capture = grabImage(item.data());
    QCOMPARE(capture, sourceImage.convertToFormat(QImage::Format_ARGB32_Premultiplied));
    QCOMPARE(sourceImage, item->property("source").value<QImage>());
}

void IconItemTest::invalidIcon()
{
    QString name("tst-plasma-framework-invalid-icon-name");
    KIconLoader iconLoader("tst_plasma-framework");
    if (iconLoader.hasIcon(name)) {
        QSKIP("Current icon theme has 'tst-plasma-framework-invalid-icon-name' icon.");
    }

    QQuickItem *item = createIconItem();
    item->setProperty("source", name);
    QVERIFY(!item->property("valid").toBool());
    QVERIFY(imageIsEmpty(grabImage(item)));
}

void IconItemTest::usesPlasmaTheme()
{
    Plasma::Theme theme;
    if (!theme.themeName().startsWith("default")) {
        QSKIP("Current Plasma theme is not Breeze.");
    }

    // usesPlasmaTheme = true (default)
    QQuickItem *item1 = createIconItem();
    item1->setProperty("source", "konversation");
    QVERIFY(item1->property("valid").toBool());
    QCOMPARE(QStringLiteral("konversation"), item1->property("source").toString());

    Plasma::Svg svg;
    svg.setContainsMultipleImages(true);
    svg.setImagePath("icons/konversation");

    QImage img1 = grabImage(item1);
    QImage img2 = svg.image(QSize(item1->width(), item1->height()), "konversation");
    QVERIFY(!imageIsEmpty(img1));
    QVERIFY(!imageIsEmpty(img2));
    QCOMPARE(img1, img2);

    // usesPlasmaTheme = false
    QQuickItem *item2 = createIconItem();
    item2->setProperty("usesPlasmaTheme", false);
    item2->setProperty("source", "konversation");

    img1 = grabImage(item2);
    // This depends on konversation icon being different in Plasma Breeze theme
    // and our test icon theme
    QVERIFY(img1 != img2);
}

void IconItemTest::animation()
{
    // animated = true (default)
    QQuickItem *item1 = createIconItem();
    item1->setProperty("source", "user-away");
    // first icon is not animated
    QImage userAwayImg = grabImage(item1);

    item1->setProperty("source", "user-busy");
    grabImage(item1);
    item1->setProperty("source", "user-away");
    // animation from user-busy -> user-away
    QVERIFY(userAwayImg != grabImage(item1));

    // animated = false
    QQuickItem *item2 = createIconItem();
    item2->setProperty("animated", false);
    item2->setProperty("source", "user-busy");
    QImage userBusyImg = grabImage(item2);

    item2->setProperty("source", "user-away");
    QCOMPARE(userAwayImg, grabImage(item2));

    item2->setProperty("source", "user-busy");
    QCOMPARE(userBusyImg, grabImage(item2));
}

void IconItemTest::animationAfterHide()
{
    QQuickItem *item1 = createIconItem();
    QQuickItem *item2 = createIconItem();
    item1->setProperty("source", "user-away");
    item2->setProperty("source", "user-busy");
    // first icon is not animated
    QImage userAwayImg = grabImage(item1);
    QImage userBusyImg = grabImage(item2);

    item1->setProperty("source", "user-busy");
    grabImage(item1);
    item1->setProperty("visible", "false");
    item1->setProperty("visible", "true");
    item1->setProperty("source", "user-away");
    // icon was hidden, no animation
    QCOMPARE(userAwayImg, grabImage(item1));

    item1->setProperty("source", "user-busy");
    QVERIFY(userBusyImg != grabImage(item1));
}

void IconItemTest::bug_359388()
{
    QString name("bug359388");
    KIconLoader iconLoader("tst_plasma-framework");
    QIcon customThemeIcon(new KIconEngine(name, &iconLoader));
    if (iconLoader.hasIcon(name)) {
        QSKIP("Current icon theme has 'bug359388' icon.");
    }

    iconLoader.addAppDir("tst_plasma-framework", QFINDTESTDATA("data/bug359388"));

    QQuickItem *item1 = createIconItem();
    item1->setProperty("source", customThemeIcon);
    QVERIFY(item1->property("valid").toBool());
    QCOMPARE(customThemeIcon, item1->property("source").value<QIcon>());

    QQuickItem *item2 = createIconItem();
    item2->setProperty("source", QIcon(QFINDTESTDATA("data/bug359388/hicolor/22x22/apps/" + name + ".svg")));
    QVERIFY(item2->property("valid").toBool());

    QCOMPARE(grabImage(item1), grabImage(item2));
}

void IconItemTest::loadSvg()
{
    QString name("tst-plasma-framework-test-icon");

    QQuickItem *item = createIconItem();
    item->setProperty("animated", false);
    item->setSize(QSize(22, 22));
    item->setProperty("source", name);
    QVERIFY(item->property("valid").toBool());

    Plasma::Svg *svg;
    svg = findPlasmaSvg(item);
    Q_ASSERT(svg);
    QCOMPARE(svg->imagePath(), QFINDTESTDATA("data/icons/test-theme/apps/22/" + name + ".svg"));

    // we only have 32x32 and 22x22 version in the theme, thus 32x32 is a better match.
    item->setSize(QSize(64, 64));
    // just to update the icon
    grabImage(item);
    svg = findPlasmaSvg(item);
    Q_ASSERT(svg);
    QCOMPARE(svg->imagePath(), QFINDTESTDATA("data/icons/test-theme/apps/32/" + name + ".svg"));
}

void IconItemTest::themeChange()
{
    // Icon from Plasma theme
    QQuickItem *item1 = createIconItem();
    item1->setProperty("animated", false);
    item1->setProperty("source", "zoom-fit-height");
    Plasma::Svg *svg1 = item1->findChild<Plasma::Svg*>();
    changeTheme(svg1->theme(), "breeze-light");
    QImage img1 = grabImage(item1);
    changeTheme(svg1->theme(), "breeze-dark");
    QImage img2 = grabImage(item1);
    QVERIFY(img1 != img2);

    // Icon from icon theme
    QQuickItem *item2 = createIconItem();
    item2->setProperty("animated", false);
    item2->setProperty("width", 22);
    item2->setProperty("height", 22);
    item2->setProperty("source", "tst-plasma-framework-test-icon");
    Plasma::Svg *svg2 = item2->findChild<Plasma::Svg*>();
    changeTheme(svg2->theme(), "breeze-light");
    img1 = grabImage(item2);
    changeTheme(svg2->theme(), "breeze-dark");
    img2 = grabImage(item2);
    QVERIFY(img1 != img2);
}

void IconItemTest::qiconFromTheme()
{
    // Icon from Plasma theme
    QQuickItem *item1 = createIconItem();
    QIcon icon1 = QIcon::fromTheme("konversation");
    item1->setProperty("source", icon1);
    QVERIFY(item1->findChild<Plasma::Svg*>());
    QVERIFY(!imageIsEmpty(grabImage(item1)));
    QCOMPARE(icon1, item1->property("source").value<QIcon>());

    // Icon from icon theme
    QQuickItem *item2 = createIconItem();
    QIcon icon2 = QIcon::fromTheme("tst-plasma-framework-test-icon");
    item2->setProperty("source", icon2);
    QVERIFY(item2->findChild<Plasma::Svg*>());
    QVERIFY(!imageIsEmpty(grabImage(item2)));
    QCOMPARE(icon2, item2->property("source").value<QIcon>());
}

void IconItemTest::changeColorGroup()
{
    // Icon from Plasma theme
    QQuickItem *item = createIconItem();
    item->setProperty("animated", false);
    item->setProperty("source", "zoom-fit-height");
    Plasma::Svg *svg = item->findChild<Plasma::Svg*>();
    // not using "breeze" theme as that one follows system color scheme
    // and that one might not have a complementary group or a broken one
    changeTheme(svg->theme(), "breeze-light");
    QSignalSpy spy(svg, SIGNAL(repaintNeeded()));
    QVERIFY(spy.isValid());
    QImage img1 = grabImage(item);
    item->setProperty("colorGroup", Plasma::Theme::ComplementaryColorGroup);
    QTRY_VERIFY(spy.count() == 1);
    QImage img2 = grabImage(item);
    QVERIFY(img1 != img2);
}

void IconItemTest::animatingActiveChange()
{
    QQuickItem *item1 = createIconItem();
    item1->setProperty("animated", false);
    item1->setProperty("source", "tst-plasma-framework-test-icon");
    QImage img1 = grabImage(item1);

    QQuickItem *item2 = createIconItem();
    item2->setProperty("animated", false);
    item2->setProperty("active", true);
    item2->setProperty("source", "tst-plasma-framework-test-icon");
    QImage img2 = grabImage(item2);
    QVERIFY(img1 != img2);

    item1->setProperty("active", true);
    img1 = grabImage(item1);
    QVERIFY(img1 != img2); // animation is running
}

void IconItemTest::animatingEnabledChange()
{
    QQuickItem *item1 = createIconItem();
    item1->setProperty("animated", false);
    item1->setProperty("source", "tst-plasma-framework-test-icon");
    QImage img1 = grabImage(item1);

    QQuickItem *item2 = createIconItem();
    item2->setProperty("animated", false);
    item2->setProperty("enabled", false);
    item2->setProperty("source", "tst-plasma-framework-test-icon");
    QImage img2 = grabImage(item2);
    QVERIFY(img1 != img2);

    item1->setProperty("enabled", false);
    img1 = grabImage(item1);
    QVERIFY(img1 != img2); // animation is running
}

void IconItemTest::windowChanged()
{
    QQuickItem *item = createIconItem();
    item->setProperty("animated", false);
    item->setProperty("source", "tst-plasma-framework-test-icon");
    QImage img = grabImage(item);

    QQuickView newView;
    newView.setSource(QUrl::fromLocalFile(QFINDTESTDATA("data/view.qml")));
    newView.show();
    QTest::qWaitForWindowExposed(&newView);

    item->setProperty("visible", false);
    item->setParentItem(newView.rootObject());
    item->setProperty("visible", true);
    QCOMPARE(grabImage(item), img);
}

QTEST_MAIN(IconItemTest)

