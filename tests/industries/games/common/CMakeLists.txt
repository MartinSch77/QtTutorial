qttutorial_add_test(test_table_message
    SOURCES test_table_message.cpp
    LIB games_common_lib
    QT_LIBS Qt6::Core
)

qttutorial_add_test(test_lan_beacon
    SOURCES test_lan_beacon.cpp
    LIB games_common_lib
    QT_LIBS Qt6::Core
)

qttutorial_add_test(test_table_server_client
    SOURCES test_table_server_client.cpp
    LIB games_common_lib
    QT_LIBS Qt6::Core Qt6::Network
)

qttutorial_add_test(test_lan_advertiser_browser
    SOURCES test_lan_advertiser_browser.cpp
    LIB games_common_lib
    QT_LIBS Qt6::Core Qt6::Network
)
