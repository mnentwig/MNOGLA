package com.android.MNOGLAJNI;

import android.content.Context;
import android.opengl.GLSurfaceView;
import android.util.Log;
import android.view.MotionEvent;

import java.util.HashMap;
import java.util.Map;

import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.opengles.GL10;

public class MNOGLAView extends GLSurfaceView {
    public MNOGLAView(Context context) {
        super(context);
        setEGLConfigChooser(8, 8, 8, 0, 16, 0);
        setEGLContextClientVersion(3);
        setRenderer(new Renderer());
    }

    protected Renderer r = null;

    public void onPause() {
        Log.i("MNOGLA", "onPause");
    }

    public void onResume() {
        Log.i("MNOGLA", "onResume");
    }

    private static class Renderer implements GLSurfaceView.Renderer {
        protected boolean isPaused = false;

        public void onDrawFrame(GL10 gl) {
//            Log.i("MNOGLA", "onDrawFrame");
            if (!isPaused)
                MNOGLALIB.render();
        }

        public void onSurfaceChanged(GL10 gl, int width, int height) {
            Log.i("MNOGLA", "onSurfaceChanged" + width + " " + height);
            MNOGLALIB.evt2(/*key: WINSIZE*/1000, width, height);
        }

        public void onSurfaceCreated(GL10 gl, EGLConfig config) {
            Log.i("MNOGLA", "onSurfaceCreated");
            MNOGLALIB.initGlContext();
        }
    }

    private class touchXy {
        public touchXy(float xf, float yf) {
            x = (int) xf;
            y = (int) yf;
        }

        public int x;
        public int y;
    }

    static Map<Integer, touchXy> touchStateByPtrId = new HashMap<Integer, touchXy>();

    @Override
    public boolean onTouchEvent(MotionEvent ev) {
        final int action = ev.getActionMasked();
        switch (action) {
            case MotionEvent.ACTION_DOWN:
            case MotionEvent.ACTION_POINTER_DOWN: {
                //Log.i("MNOGLA java", "ACTION_(POINTER_)DOWN");
                int ptrIx;
                if (action == MotionEvent.ACTION_DOWN)
                    ptrIx = 0;
                else
                    ptrIx = ev.getActionIndex(); // documented for ACTION_POINTER_(UP/DOWN) only
                int ptrId = ev.getPointerId(ptrIx);
                touchXy xy = new touchXy(ev.getX(ptrIx), ev.getY(ptrIx));
                touchStateByPtrId.put(ptrId, xy);
                MNOGLALIB.evt3(200, ptrId, xy.x, xy.y);
                return true;
            }
            case MotionEvent.ACTION_POINTER_UP:
            case MotionEvent.ACTION_UP: {
                //Log.i("MNOGLA java", "ACTION_(POINTER_)UP");
                int ptrIx;
                if (action == MotionEvent.ACTION_UP)
                    ptrIx = 0;
                else
                    ptrIx = ev.getActionIndex(); // documented for ACTION_POINTER_(UP/DOWN) only

                int ptrId = ev.getPointerId(ptrIx);
                if (action == MotionEvent.ACTION_UP)
                    touchStateByPtrId.clear(); // force re-sync of our state against OS
                else
                    touchStateByPtrId.remove(ptrId);

                MNOGLALIB.evt2(201, ptrId, touchStateByPtrId.size());
                return true;
            }
            case MotionEvent.ACTION_MOVE:
                for (int ptrIx = 0; ptrIx < ev.getPointerCount(); ++ptrIx) {
                    int pointerId = ev.getPointerId(ptrIx);
                    touchXy xy = new touchXy(ev.getX(ptrIx), ev.getY(ptrIx));
                    touchXy xyPrev = touchStateByPtrId.get(pointerId);
                    if (xyPrev == null
                            || (xyPrev.x != xy.x)
                            || (xyPrev.y != xy.y)) {
                        touchStateByPtrId.put(pointerId, xy);
                        MNOGLALIB.evt3(/*key: MOVE*/202, pointerId, xy.x, xy.y);
                    }
                }
                return true;
            default:
                return true;
        } // switch action
    }
}
