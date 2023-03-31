package com.android.MNOGLAJNI;

import android.content.res.AssetManager;

public class MNOGLALIB {

     static {
         System.loadLibrary("MNOGLA_host");
     }

     public static native void init();
     public static native void initGlContext();
     public static native void render();
     public static native void evt2(int key, int v1, int v2);
     public static native void evt3(int key, int v1, int v2, int v3);

     public static native void midiCb(int v1, int v2, int v3);
     public static native void setAssetMgr(AssetManager mgr);
}
