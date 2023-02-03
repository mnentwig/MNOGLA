package com.android.MNOGLAJNI;

import android.content.Context;
import android.opengl.GLSurfaceView;
import android.view.MotionEvent;

import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.opengles.GL10;

public class MNOGLAView extends GLSurfaceView {
    public MNOGLAView(Context context) {
        super(context);
        setEGLConfigChooser(8, 8, 8, 0, 16, 0);
        setEGLContextClientVersion(3);
        setRenderer(new Renderer());
    }

    private static class Renderer implements GLSurfaceView.Renderer {
        public void onDrawFrame(GL10 gl) {
            MNOGLALIB.render();
        }

        public void onSurfaceChanged(GL10 gl, int width, int height) {
            MNOGLALIB.init(width, height);
        }

        public void onSurfaceCreated(GL10 gl, EGLConfig config) {
            // Do nothing.
        }
    }

    @Override public boolean onTouchEvent(MotionEvent ev) {
        int key;
        switch(ev.getAction()){
            case MotionEvent.ACTION_DOWN:
                key = 200;
                break;
            case MotionEvent.ACTION_UP:
                key = 201;
                break;
            case MotionEvent.ACTION_MOVE:
                key = 202;
                break;
            default:
                return true;
        }

        final int historySize = ev.getHistorySize();
        final int pointerCount = ev.getPointerCount();
        for (int h = 0; h < historySize; h++)
            for (int p = 0; p < pointerCount; p++)
                MNOGLALIB.evt3(key, ev.getPointerId(p), (int)ev.getHistoricalX(p, h), (int)ev.getHistoricalY(p, h));

        for (int p = 0; p < pointerCount; p++)
            MNOGLALIB.evt3(key, ev.getPointerId(p), (int)ev.getX(p), (int)ev.getY(p));
        return true;
    }
}
