package org.speedynote.app;

import android.app.Activity;
import android.content.Context;
import android.util.Log;
import android.view.MotionEvent;

import io.github.davidallison.android.sdk.penremote.ButtonEvent;
import io.github.davidallison.android.sdk.penremote.SPenRemote;
import io.github.davidallison.android.sdk.penremote.SPenUnitManager;
import io.github.davidallison.android.sdk.penremote.SPenUnitType;

/**
 * Connects the S Pen side button and forwards press/release to Qt as one key.
 * Devices that are not Samsung, or that have no S Pen, fail the connection
 * and are ignored.
 */
public final class SPenRemoteBridge {
    private static final String TAG = "SPenRemoteBridge";

    private static volatile boolean sButtonDown;

    private SPenRemoteBridge() {}

    /**
     * Side button from the EMR digitizer. Tab Active3 reports it as
     * BUTTON_STYLUS_PRIMARY on hover and pen events. The BLE remote service
     * refuses this device, so this is the path that actually fires.
     * Ignored while the remote service is connected, or that service and the
     * digitizer would each emit one press.
     */
    public static void onDigitizerButtons(int buttonState) {
        if (SPenRemote.INSTANCE.isConnected()) {
            return;
        }
        boolean down = (buttonState & MotionEvent.BUTTON_STYLUS_PRIMARY) != 0;
        if (down == sButtonDown) {
            return;
        }
        sButtonDown = down;
        Log.i(TAG, "digitizer button " + (down ? "down" : "up"));
        nativeOnSpenButton(down);
    }

    /**
     * Connect and listen for the side button. No-op when already connected.
     */
    public static void onResume(Activity activity) {
        if (SPenRemote.INSTANCE.isConnected()) {
            return;
        }
        SPenRemote.INSTANCE.connect(activity, new SPenRemote.ConnectionResultCallback() {
            @Override
            public void onSuccess(SPenUnitManager unitManager) {
                unitManager.registerSPenEventListener(
                        unitManager.getUnit(SPenUnitType.TYPE_BUTTON),
                        event -> {
                            boolean down = new ButtonEvent(event).getAction()
                                    == ButtonEvent.ButtonAction.ACTION_DOWN;
                            sButtonDown = down;
                            nativeOnSpenButton(down);
                        });
            }

            @Override
            public void onFailure(SPenRemote.ConnectionResultCallback.Error code) {
                Log.i(TAG, "S Pen remote unavailable: " + code);
            }
        });
    }

    /**
     * If the side button is still down, deliver a release, then disconnect.
     */
    public static void onPause(Context context) {
        if (sButtonDown) {
            sButtonDown = false;
            nativeOnSpenButton(false);
        }
        if (SPenRemote.INSTANCE.isConnected()) {
            SPenRemote.INSTANCE.disconnect(context);
        }
    }

    private static native void nativeOnSpenButton(boolean down);
}
