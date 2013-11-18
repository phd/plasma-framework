#!/bin/sh
#    ${KDE4_KIDLETIME_LIBRARY}
#    ${KIdleTime_LIBRARIES}
#     ${KCore_LIBRARIES}
#     ${Solid_LIBRARIES}
#     ${Plasma_LIBRARIES}
# ./geolocation/CMakeLists.txt:29:    ${KDE4_PLASMA_LIBS}
# ./geolocation/CMakeLists.txt:30:    ${KDE4_KDECORE_LIBS}
# ./geolocation/CMakeLists.txt:31:    ${KDE4_KIO_LIBS}
# ./geolocation/CMakeLists.txt:32:    ${KDE4_SOLID_LIBS})


for FS in `find $PWD -type f -name 'CMakeLists.txt'`; do
    perl -p -i -e 's/plasma_add_plugin\(([a-zA-Z_\-]+) /add_library(\1 MODULE /g' $FS
    perl -p -i -e 's/kde4_add_plugin\(([a-zA-Z_\-]+) /add_library(\1 MODULE /g' $FS
#     perl -p -i -e 's/kde4_add_ui_files/qt5_wrap_ui/g' $FS #TODO: need a ki18n_wrap_ui, otherwise changes in the cpp files are needed
    perl -p -i -e 's/qt4_wrap_ui/qt5_wrap_ui/g' $FS
    perl -p -i -e 's/kde4_add_kdeinit_executable/kf5_add_kdeinit_executable/g' $FS
    perl -p -i -e 's/kde4_add_library/add_library/g' $FS
    perl -p -i -e 's/kde4_add_executable/add_executable/g' $FS
    perl -p -i -e 's/kde4_create_handbook/kdoctools_create_handbook/g' $FS
    perl -p -i -e 's/qt4_add_resources/qt5_add_resources/g' $FS
    perl -p -i -e 's/qt4_add_dbus_interface/qt5_add_dbus_interface/g' $FS
    perl -p -i -e 's/kde4_install_auth_helper_files/kauth_install_helper_files/g' $FS
    perl -p -i -e 's/kde4_install_auth_actions/kauth_install_actions/g' $FS

    perl -p -i -e 's/KF5\:\:plasma/KF5\:\:Plasma/g' $FS
    perl -p -i -e 's/\$\{KDE4_KIDLETIME_LIBRARY\}/KF5::KIdleTime/g' $FS
    perl -p -i -e 's/\$\{KDE4_PLASMA_LIBS\}/KF5::Plasma/g' $FS
    perl -p -i -e 's/\$\{KDE4_KDECORE_LIBS\}//g' $FS
    perl -p -i -e 's/\$\{KDE4_SOLID_LIBS\}/KF5\:\:Solid/g' $FS
    perl -p -i -e 's/\$\{KDE4_KIO_LIBS\}/KF5\:\:KIOWidgets/g' $FS
    perl -p -i -e 's/\$\{KDE4_KNOTIFYCONFIG_LIBRARY\}/KF5\:\:KNotifyConfig/g' $FS
    perl -p -i -e 's/\$\{KDE4_KNEWSTUFF3_LIBRARY\}/KF5\:\:KNewStuff/g' $FS
    perl -p -i -e 's/\$\{KDE4_KNEWSTUFF3_LIBS\}/KF5\:\:KNewStuff/g' $FS
    perl -p -i -e 's/\$\{KDE4_KDESU_LIBS\}/KF5\:\:KDESu/g' $FS
    perl -p -i -e 's/\$\{KDE4_KTEXTEDITOR_LIBS\}/KF5\:\:KTextEditor/g' $FS
    perl -p -i -e 's/\$\{KDE4_KPTY_LIBS\}/KF5\:\:KPty/g' $FS
    perl -p -i -e 's/\$\{Solid_LIBRARIES\}/KF5\:\:Solid/g' $FS
    perl -p -i -e 's/\$\{KService_LIBRARIES\}/KF5\:\:KService/g' $FS
    perl -p -i -e 's/\$\{KCoreAddons_LIBRARIES\}/KF5\:\:KCoreAddons/g' $FS
    perl -p -i -e 's/\$\{KI18n_LIBRARIES\}/KF5\:\:KI18n/g' $FS
    perl -p -i -e 's/\$\{KArchive_LIBRARIES\}/KF5\:\:KArchive/g' $FS
    perl -p -i -e 's/\$\{Plasma_LIBRARIES\}/KF5\:\:Plasma/g' $FS
    perl -p -i -e 's/\$\{KNotifications_LIBRARIES\}/KF5\:\:KNotifications/g' $FS
    perl -p -i -e 's/\$\{KIconThemes_LIBRARIES\}/KF5\:\:KIconThemes/g' $FS
    perl -p -i -e 's/\$\{KWindowSystem_LIBRARIES\}/KF5\:\:KWindowSystem/g' $FS
    perl -p -i -e 's/\$\{KConfigWidgets_LIBRARIES\}/KF5\:\:KConfigWidgets/g' $FS
    perl -p -i -e 's/\$\{KCMUTILS_LIBS\}/KF5\:\:KCMUtils/g' $FS

    perl -p -i -e 's/\$\{QT_QTXML_LIBRARY\}/Qt5\:\:Xml/g' $FS
    perl -p -i -e 's/\$\{QT_QTGUI_LIBRARY\}/Qt5\:\:Gui/g' $FS
    perl -p -i -e 's/\$\{QT_QTNETWORK_LIBRARY\}/Qt5\:\:Network/g' $FS
    perl -p -i -e 's/\$\{QT_QTCORE_LIBRARY\}/Qt5\:\:Core/g' $FS
    perl -p -i -e 's/\$\{QT_QTDECLARATIVE_LIBRARY\}/Qt5\:\:Declarative/g' $FS
    perl -p -i -e 's/\$\{QT_QTTEST_LIBRARY\}/Qt5\:\:Test/g' $FS
    perl -p -i -e 's/\$\{QT_QTSVG_LIBRARY\}/Qt5\:\:Svg/g' $FS
    perl -p -i -e 's/\$\{QT_QTOPENGL_LIBRARY\}/Qt5\:\:OpenGL/g' $FS
    perl -p -i -e 's/\$\{QT_QTSQL_LIBRARY\}/Qt5\:\:Sql/g' $FS
    perl -p -i -e 's/\$\{QT_QTDBUS_LIBRARY\}/Qt5\:\:DBus/g' $FS
    perl -p -i -e 's/\$\{Qt5DBus_LIBRARIES\}/Qt5\:\:DBus/g' $FS
    perl -p -i -e 's/\$\{Qt5Quick_LIBRARIES\}/Qt5\:\:Quick/g' $FS
    perl -p -i -e 's/\$\{Qt5Qml_LIBRARIES\}/Qt5\:\:Qml/g' $FS
    perl -p -i -e 's/\$\{Qt5Widgets_LIBRARIES\}/Qt5\:\:Widgets/g' $FS
    perl -p -i -e 's/\$\{Qt5Script_LIBRARIES\}/Qt5\:\:Script/g' $FS
done

