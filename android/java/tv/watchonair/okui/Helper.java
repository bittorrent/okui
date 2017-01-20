/**
* Copyright 2017 BitTorrent Inc.
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
*    http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*/
package tv.watchonair.okui;

import android.app.Activity;
import android.app.AlertDialog;
import android.app.UiModeManager;
import android.content.Context;
import android.content.DialogInterface;
import android.content.Intent;
import android.content.res.Configuration;
import android.os.Build;
import android.os.Handler;
import android.provider.Settings.Secure;
import android.util.DisplayMetrics;
import android.net.ConnectivityManager;
import android.net.NetworkInfo;
import android.net.Uri;

import java.lang.Runnable;
import java.lang.String;

public class Helper {
    private Activity _activity;

    public Helper(Activity activity) {
        _activity = activity;
    }

    public boolean openURL(final String url) {
        Intent browserIntent = new Intent(Intent.ACTION_VIEW, Uri.parse(url));
        _activity.startActivity(browserIntent);
        return true;
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

    public boolean isTenFootUI() {
        UiModeManager uiModeManager = (UiModeManager) _activity.getSystemService(Activity.UI_MODE_SERVICE);
        return uiModeManager.getCurrentModeType() == Configuration.UI_MODE_TYPE_TELEVISION;
    }
}
