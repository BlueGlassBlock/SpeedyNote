// Q_OS_ANDROID is a Qt macro and is unset until a Qt header is included.
// The NDK always defines __ANDROID__, so the JNI symbol is actually compiled.
#if defined(__ANDROID__)

#include <QApplication>
#include <QKeyEvent>
#include <QMetaObject>
#include <QWidget>
#include <jni.h>

// ============================================================================
// S Pen side button → one ordinary Qt key
// ============================================================================
// SPenRemoteBridge.java reports ACTION_DOWN / ACTION_UP on the binder thread.
// The key is delivered on the Qt thread so shortcut capture and the hold
// handler see the same event a keyboard would produce. The key is F13, which
// no default shortcut uses. Air motion is not forwarded.

namespace {

/**
 * @brief Post F13 to the focused widget, or the active window if nothing has focus.
 * @param pressed True for the side-button press, false for the release.
 */
void postSpenKey(bool pressed)
{
    QMetaObject::invokeMethod(qApp, [pressed]() {
        QWidget* target = QApplication::focusWidget();
        if (!target) {
            target = QApplication::activeWindow();
        }
        if (!target) {
            return;
        }
        QKeyEvent event(pressed ? QEvent::KeyPress : QEvent::KeyRelease,
                        Qt::Key_F13, Qt::NoModifier);
        QApplication::sendEvent(target, &event);
    }, Qt::QueuedConnection);
}

} // namespace

extern "C" JNIEXPORT void JNICALL
Java_org_speedynote_app_SPenRemoteBridge_nativeOnSpenButton(
        JNIEnv* /*env*/, jclass /*clazz*/, jboolean down)
{
    if (!qApp) {
        return;
    }
    postSpenKey(down == JNI_TRUE);
}

#endif // __ANDROID__
