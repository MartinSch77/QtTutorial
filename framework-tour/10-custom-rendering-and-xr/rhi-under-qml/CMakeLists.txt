find_package(Qt6 COMPONENTS Quick Qml Gui GuiPrivate ShaderTools QUIET)

if(NOT TARGET Qt6::Quick OR NOT TARGET Qt6::Qml OR NOT TARGET Qt6::GuiPrivate OR NOT TARGET Qt6::ShaderTools)
    message(STATUS "Qt6::Quick/Qt6::Qml/Qt6::GuiPrivate/Qt6::ShaderTools not found - skipping rhi-under-qml")
    return()
endif()

# Qt6::GuiPrivate exists whenever Qt Gui's private/development headers are
# installed (e.g. via the qt6-base-private-dev apt package), but the header
# *layout* underneath it changed between Qt releases: the locally installed
# Qt 6.4.2 only ships the older QtGui/private/qrhi_p.h include path, while
# the <rhi/qrhi.h> path this module's C++ code is written against (matching
# Qt's own current "Scene Graph - RHI Under QML" example) requires the
# restructured RHI private headers from a newer Qt. Probe for the header
# directly instead of guessing from the Qt version alone.
include(CheckIncludeFileCXX)
set(CMAKE_REQUIRED_LIBRARIES Qt6::GuiPrivate)
check_include_file_cxx("rhi/qrhi.h" QTTUTORIAL_HAVE_RHI_QRHI_H)
unset(CMAKE_REQUIRED_LIBRARIES)

if(NOT QTTUTORIAL_HAVE_RHI_QRHI_H)
    message(STATUS "<rhi/qrhi.h> not available via Qt6::GuiPrivate on this Qt install - skipping rhi-under-qml")
    return()
endif()

# QQuickWindow::rhi()/swapChain() (used by this module, matching Qt's own
# "Scene Graph - RHI Under QML" example) are a newer public QQuickWindow
# convenience API, not present on this repository's Qt 6.4.2 baseline
# (which only exposes the underlying resources indirectly, via
# QSGRendererInterface::getResource()). Probe for them directly rather than
# assuming they exist just because <rhi/qrhi.h> does.
include(CheckCXXSourceCompiles)
set(CMAKE_REQUIRED_LIBRARIES Qt6::Quick Qt6::GuiPrivate)
check_cxx_source_compiles("
    #include <QQuickWindow>
    #include <rhi/qrhi.h>
    void qttutorial_probe(QQuickWindow* window) {
        QRhi* rhi = window->rhi();
        QRhiSwapChain* swapChain = window->swapChain();
        (void)rhi;
        (void)swapChain;
    }
    int main() { return 0; }
" QTTUTORIAL_HAVE_QQUICKWINDOW_RHI_ACCESSORS)
unset(CMAKE_REQUIRED_LIBRARIES)

if(NOT QTTUTORIAL_HAVE_QQUICKWINDOW_RHI_ACCESSORS)
    message(STATUS "QQuickWindow::rhi()/swapChain() not available on this Qt install - skipping rhi-under-qml")
    return()
endif()

qttutorial_add_app(rhi_under_qml
    SOURCES
        src/main.cpp
        src/RhiUnderlayItem.h
        src/RhiUnderlayItem.cpp
        src/RhiUnderlayRenderer.h
        src/RhiUnderlayRenderer.cpp
    QT_LIBS Qt6::Quick Qt6::Qml Qt6::GuiPrivate
    QML_MODULE_URI QtTutorial.CustomRenderingXr.RhiUnderQml
    QML_SOURCES qml/Main.qml
)
# Unlike most modules in this repo, this app has no separate static library
# target to inherit a PUBLIC "src" include directory from (see the header
# comment above about this module having little pure logic to split out) --
# without this, qt_add_qml_module's generated qmltyperegistrations.cpp,
# which #includes "RhiUnderlayItem.h" via an angle-bracket
# __has_include(<RhiUnderlayItem.h>) check, silently fails to find it and
# skips the type registration instead of erroring, producing confusing
# "qttutorial was not declared" errors further down in that generated file.
target_include_directories(rhi_under_qml PRIVATE src)

qt_add_shaders(rhi_under_qml "rhi_under_qml_shaders"
    PREFIX
        /shaders
    FILES
        shaders/underlay.vert
        shaders/underlay.frag
)
