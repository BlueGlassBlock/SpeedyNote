#pragma once

// Defaults and key matching for tool-shortcut tap/hold. No shortcuts.json I/O.

#include "ShortcutManager.h"
#include <QDebug>
#include <Qt>

namespace ShortcutManagerTests {

/**
 * @brief Check default activations and single-key shortcut matching.
 * @return True if every check passes.
 */
inline bool testToolActivationDefaults()
{
    qDebug() << "=== Test: tool shortcut activation defaults ===";
    if (!ShortcutManager::supportsActivation(QStringLiteral("tool.pan")))
        return false;
    if (!ShortcutManager::supportsActivation(QStringLiteral("tool.eraser")))
        return false;
    if (ShortcutManager::supportsActivation(QStringLiteral("edit.undo")))
        return false;
    if (ShortcutManager::defaultActivationForAction(QStringLiteral("tool.pan"))
        != ShortcutManager::Activation::Hold)
        return false;
    if (ShortcutManager::defaultActivationForAction(QStringLiteral("tool.eraser"))
        != ShortcutManager::Activation::Trigger)
        return false;

    if (!ShortcutManager::shortcutMatchesKey(QStringLiteral("E"), Qt::Key_E, Qt::NoModifier))
        return false;
    if (!ShortcutManager::shortcutMatchesKey(QStringLiteral("Ctrl+E"), Qt::Key_E, Qt::ControlModifier))
        return false;
    if (ShortcutManager::shortcutMatchesKey(QStringLiteral("Ctrl+E"), Qt::Key_E, Qt::NoModifier))
        return false;
    if (ShortcutManager::shortcutMatchesKey(QStringLiteral("H"), Qt::Key_E, Qt::NoModifier))
        return false;
    if (ShortcutManager::shortcutMatchesKey(QString(), Qt::Key_E, Qt::NoModifier))
        return false;

    qDebug() << "=== tool shortcut activation defaults: PASS ===";
    return true;
}

/**
 * @brief Run all shortcut-manager tests.
 * @return True if all tests pass.
 */
inline bool runAllTests()
{
    return testToolActivationDefaults();
}

} // namespace ShortcutManagerTests
