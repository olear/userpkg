QT                                  += core gui network
TARGET                               = UserPKG
TEMPLATE                             = app
VERSION                              = 20140525
SOURCES                             += src/main.cpp src/mainwindow.cpp
HEADERS                             += src/mainwindow.h
FORMS                               += src/mainwindow.ui
RESOURCES                           += res/files.qrc

DESTDIR = build
OBJECTS_DIR                          = $${DESTDIR}/.obj
MOC_DIR                              = $${DESTDIR}/.moc
RCC_DIR                              = $${DESTDIR}/.qrc
UI_DIR                               = $${DESTDIR}/.ui

isEmpty(PREFIX) {
    PREFIX                           = /usr/local
}

isEmpty(LIBDIR) {
        LIBDIR                          = $${PREFIX}/lib$${LIBSUFFIX}
}

isEmpty(INCLUDEDIR) {
    INCLUDEDIR                   = $${PREFIX}/include
}

isEmpty(DOCDIR) {
    DOCDIR                           = $$PREFIX/share/doc
}

target.path                          = $${PREFIX}/bin
target_docs.path                     = $$DOCDIR/$${TARGET}-$${VERSION}
target_docs.files                   = doc/COPYING doc/README

target_desktop.path			= $${PREFIX}/share/applications
target_desktop.files			= res/$${TARGET}.desktop

target_icon_32.path			= $${PREFIX}/share/icons/hicolor/32x32/apps
target_icon_48.path			= $${PREFIX}/share/icons/hicolor/48x48/apps
target_icon_svg.path			= $${PREFIX}/share/icons/hicolor/scalable/apps

target_icon_32.files			= res/hicolor/32x32/$${TARGET}.png
target_icon_48.files			= res/hicolor/48x48/$${TARGET}.png
target_icon_svg.files			= res/hicolor/scalable/$${TARGET}.svg

INSTALLS                            += target target_docs target_desktop target_icon_32 target_icon_48 target_icon_svg
QMAKE_CLEAN                         += -r $${DESTDIR} Makefile

INCLUDEPATH                         += "$${INCLUDEDIR}"
LIBS                                += -L"$${PREFIX}/lib$${LIBSUFFIX}"

INCLUDEPATH+="$${_PRO_FILE_PWD_}/DracoPKG/build/include"
LIBS+=-L"$${_PRO_FILE_PWD_}/DracoPKG/build/lib"

CONFIG(libpkgsrc) {
LIBS                                += "-lPkgSrc"
DEFINES+=LIBPKGSRC
}

!CONFIG(libpkgsrc) {
LIBS                                += "-lDracoPKG"
DEFINES+=LIBDRACOPKG
}


DEFINES += APPV=\"\\\"$${VERSION}\\\"\"

message("PREFIX: $$PREFIX")
message("LIBDIR: $$LIBDIR")
message("LIBSUFFIX: $${LIBSUFFIX}")
message("INCLUDEDIR: $${INCLUDEDIR}")
message("DOCDIR: $${DOCDIR}")
