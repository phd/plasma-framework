function(install_package_structure name)
    kcoreaddons_add_plugin(${name} SOURCES ${name}_packagestructure.cpp INSTALL_NAMESPACE "kf6/packagestructure")
    target_link_libraries(${name} plasma_packagestructure_static)
endfunction()

add_library(plasma_packagestructure_static STATIC packages.cpp)
target_link_libraries(plasma_packagestructure_static KF6::Package KF6::Plasma KF6::I18n)
set_property(TARGET plasma_packagestructure_static PROPERTY POSITION_INDEPENDENT_CODE ON)

install_package_structure(plasma_generic)
install_package_structure(plasma_applet)
install_package_structure(plasma_theme)
install_package_structure(plasma_containmentactions)

