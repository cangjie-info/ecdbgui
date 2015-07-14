HEADERS += \
    bounding_box.h \
    box_list.h \
    cangjie_img.h \
    config_handler.h \
    db_handler.h \
    edit_transcription_dialog.h \
    flow_layout.h \
    graph_transcription.h \
    graph_viewer.h \
    inscription_imgs.h \
    inscription_transcription.h \
    inscription_widget.h \
    surface_imgs.h \
    surface_transcription.h \
    surface_window.h \
    transcription_pane.h \
    image_pane.h \
    navigation_widget.h

SOURCES += \
    bounding_box.cpp \
    box_list.cpp \
    cangjie_img.cpp \
    config_handler.cpp \
    db_handler.cpp \
    edit_transcription_dialog.cpp \
    flow_layout.cpp \
    graph_transcription.cpp \
    graph_viewer.cpp \
    inscription_imgs.cpp \
    inscription_transcription.cpp \
    inscription_widget.cpp \
    main.cpp \
    surface_imgs.cpp \
    surface_transcription.cpp \
    surface_window.cpp \
    transcription_pane.cpp \
    image_pane.cpp \
    navigation_widget.cpp


QT += sql
QT += widgets

RESOURCES += \
    resources.qrc

OTHER_FILES += \
    no_image.png
