package com.android.MNOGLAJNI;

import android.app.Activity;
import android.os.Bundle;

public class MNOGLAActivity extends Activity {

    MNOGLAView mView;
    @Override protected void onCreate(Bundle icicle) {
        super.onCreate(icicle);
        mView = new MNOGLAView(getApplication());
    	setContentView(mView);
    }

    @Override protected void onPause() {
        super.onPause();
        mView.onPause();
    }

    @Override protected void onResume() {
        super.onResume();
        mView.onResume();
    }
}
