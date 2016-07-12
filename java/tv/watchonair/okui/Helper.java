package tv.watchonair.okui;

import android.app.Activity;
import android.app.AlertDialog;
import android.content.DialogInterface;
import android.os.Build;
import android.os.Handler;
import android.content.Context;
import android.provider.Settings.Secure;
import android.util.DisplayMetrics;
import android.net.ConnectivityManager;
import android.net.NetworkInfo;

import java.lang.Runnable;
import java.lang.String;

public class Helper {
    private Activity _activity;

    public Helper(Activity activity) {
        _activity = activity;
    }

    static public class OpenDialogCallback {
        public native void invoke(int button);
        private long _nativeHandle = 0;
    }

    public void openDialog(final String title, final String message, final String[] buttons, final OpenDialogCallback callback) {
        Handler mainHandler = new Handler(_activity.getMainLooper());
        Runnable runnable = new Runnable() {
            @Override
            public void run() {
                AlertDialog.Builder builder = new AlertDialog.Builder(_activity);
                builder.setCancelable(false);
                if (buttons.length > 2) {
                    builder.setTitle(message);
                    builder.setItems(buttons, new DialogInterface.OnClickListener() {
                        @Override
                        public void onClick(DialogInterface dialog, int which) {
                            callback.invoke(which);
                        }
                    });
                } else {
                    builder.setTitle(title);
                    builder.setMessage(message);
                    builder.setPositiveButton(buttons[0], new DialogInterface.OnClickListener() {
                        @Override
                        public void onClick(DialogInterface dialog, int which) {
                            callback.invoke(0);
                        }
                    });
                    if (buttons.length > 1) {
                        builder.setNegativeButton(buttons[1], new DialogInterface.OnClickListener() {
                            @Override
                            public void onClick(DialogInterface dialog, int which) {
                                callback.invoke(1);
                            }
                        });
                    }
                }
                builder.show();
            }
        };
        mainHandler.post(runnable);
    }

    public String distinctId() {
        String id = Secure.getString(_activity.getContentResolver(), Secure.ANDROID_ID);
        return id;
    }

    public String operatingSystem() {
        return "Android " + Build.VERSION.RELEASE;
    }

    public String deviceModel() {
        return Build.MODEL;
    }

    public float renderScale() {
        DisplayMetrics metrics = new DisplayMetrics();
        _activity.getWindowManager().getDefaultDisplay().getMetrics(metrics);
        return metrics.density;
    }

    public boolean hasNetworkConnection() {
         ConnectivityManager connectivityManager = (ConnectivityManager)_activity.getSystemService(Context.CONNECTIVITY_SERVICE);
         NetworkInfo networkInfo = connectivityManager.getActiveNetworkInfo();
         return networkInfo != null && networkInfo.isConnected();
    }

    public boolean isMobileConnection() {
         ConnectivityManager connectivityManager = (ConnectivityManager)_activity.getSystemService(Context.CONNECTIVITY_SERVICE);
         NetworkInfo networkInfo = connectivityManager.getActiveNetworkInfo();
         return networkInfo != null && networkInfo.isConnected() && networkInfo.getType() == ConnectivityManager.TYPE_MOBILE;
    }
}
