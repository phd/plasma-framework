add_library(plasmaextracomponentsplugin SHARED)

target_sources(plasmaextracomponentsplugin PRIVATE
    plasmaextracomponentsplugin.cpp
    enums.cpp
    qmenu.cpp
    qmenuitem.cpp
)

target_link_libraries(plasmaextracomponentsplugin
        Qt6::Quick
        Qt6::Qml
        Qt6::Widgets
        KF6::ConfigWidgets
        KF6::Plasma)


install(TARGETS plasmaextracomponentsplugin DESTINATION ${KDE_INSTALL_QMLDIR}/org/kde/plasma/extras)

install(DIRECTORY qml/ DESTINATION ${KDE_INSTALL_QMLDIR}/org/kde/plasma/extras)

ecm_generate_qmltypes(org.kde.plasma.extras 2.0 DESTINATION ${KDE_INSTALL_QMLDIR}/org/kde/plasma/extras)


# The platform specific stuff, overwrites a copy of the desktop one
# it does install some files on top of the old ones, has to be done
# file by file since if some component from the generic set is more
# recent than the specific ones, it wouldn't be overwritten

#install(TARGETS plasmaextracomponentsplugin DESTINATION ${KDE_INSTALL_PLUGINDIR}/platformimports/touch/org/kde/plasma/extras)
#install(FILES qml/qmldir DESTINATION ${KDE_INSTALL_PLUGINDIR}/platformimports/touch/org/kde/plasma/extras)

# install(FILES qml/App.qml DESTINATION ${KDE_INSTALL_PLUGINDIR}/platformimports/touch/org/kde/plasma/extras)
# install(FILES qml/Heading.qml DESTINATION ${KDE_INSTALL_PLUGINDIR}/platformimports/touch/org/kde/plasma/extras)
# install(FILES qml/Title.qml DESTINATION ${KDE_INSTALL_PLUGINDIR}/platformimports/touch/org/kde/plasma/extras)
# install(DIRECTORY qml/animations/ DESTINATION ${KDE_INSTALL_PLUGINDIR}/platformimports/touch/org/kde/plasma/extras)

#install(DIRECTORY qml/animations/ DESTINATION ${KDE_INSTALL_QMLDIR}/org/kde/plasma/extras)

#install platform overrides -- enable as we use this feature for plasmaextras
#install(DIRECTORY platformcomponents/touch/ DESTINATION ${KDE_INSTALL_PLUGINDIR}/platformimports/touch/org/kde/plasma/extras)
