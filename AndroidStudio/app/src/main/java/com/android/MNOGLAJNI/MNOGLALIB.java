package com.android.MNOGLAJNI;

public class MNOGLALIB {

     static {
         System.loadLibrary("MNOGLA_host");
     }

     public static native void init(int width, int height);
     public static native void render();
     public static native void evt2(int key, int v1, int v2);
     public static native void evt3(int key, int v1, int v2, int v3);
}
