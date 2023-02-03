package com.android.MNOGLAJNI;

import android.app.Activity;
import android.content.Context;
import android.content.pm.PackageManager;
import android.media.midi.MidiDevice;
import android.media.midi.MidiDeviceInfo;
import android.media.midi.MidiManager;
import android.media.midi.MidiOutputPort;
import android.media.midi.MidiReceiver;
import android.os.Bundle;
import android.util.Log;

public class MNOGLAActivity extends Activity {

    private class myMidiReceiver extends MidiReceiver {
        public myMidiReceiver() {
        }

        /*
         * @see android.media.midi.MidiReceiver#onSend(byte[], int, int, long)
         */
        @Override
        public void onSend(byte[] data, int offset, int count, long timestamp) {
            if (count != 3)
                return;
            MNOGLALIB.midiCb(data[offset] & 0xFF, data[offset + 1] & 0xFF, data[offset + 2] & 0xFF); // prevent sign extension
//            String text = "MIDI:" + offset + " : ";
//            for (int i = 0; i < count; i++) {
//                text += String.format("0x%02X, ", data[offset + i]);
//            }
//            Log.i("MNOGL", text);
        }
    }


    MNOGLAView mView;

    @Override
    protected void onCreate(Bundle icicle) {
        super.onCreate(icicle);
        myMidiReceiver r = new myMidiReceiver();
        if (getPackageManager().hasSystemFeature(PackageManager.FEATURE_MIDI)) {
            MidiManager m = (MidiManager) getSystemService(Context.MIDI_SERVICE);
            MidiDeviceInfo[] devInfo = m.getDevices();
            for (MidiDeviceInfo i : devInfo) {
                if (i.getOutputPortCount() > 0) {

                    m.openDevice(i, new MidiManager.OnDeviceOpenedListener() {
                        @Override
                        public void onDeviceOpened(MidiDevice device) {
                            if (device == null) {
                                Log.e("MNOGL", "could not open device " + i);
                                return;
                            } else {
                                Log.i("MNOGL", "midi opened ");
                                MidiOutputPort p = device.openOutputPort(0);
                                if (p == null) {
                                    Log.e("MNOGL", "failed to open MIDI output port:" + i);
                                    return;
                                }
                                p.connect(r);
                                Log.i("MNOGL", "MIDI connected:" + i);
                                return;
                            }
                        }
                    }, null);
                } // if TYPE_OUTPUT
            } // for midi device info
        } // if MIDI system feature is available

        mView = new MNOGLAView(getApplication());
        setContentView(mView);
    }

    @Override
    protected void onPause() {
        super.onPause();
        mView.onPause();
    }

    @Override
    protected void onResume() {
        super.onResume();
        mView.onResume();
    }
}
