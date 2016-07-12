package tv.watchonair.okui.tests;

import org.libsdl.app.SDLActivity;

public class MainActivity extends SDLActivity {
    private native void nativeInit();

    protected String[] getLibraries() {
        return new String[] {
            "okui-tests-android"
        };
    }

    @Override
    public void loadLibraries() {
        super.loadLibraries();
        nativeInit();
    }
}
