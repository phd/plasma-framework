add_executable(dpitest
    main.cpp
    dpitest.cpp
)

target_link_libraries(dpitest KF6::Plasma KF6::I18n Qt6::Gui)

