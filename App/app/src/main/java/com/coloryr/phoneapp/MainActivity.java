package com.coloryr.phoneapp;

import android.Manifest;
import android.annotation.SuppressLint;
import android.app.ProgressDialog;
import android.bluetooth.BluetoothDevice;
import android.content.pm.PackageManager;
import android.os.Bundle;
import android.os.Handler;
import android.os.Looper;
import android.widget.Toast;
import androidx.appcompat.app.AlertDialog;
import androidx.appcompat.app.AppCompatActivity;
import androidx.core.app.ActivityCompat;
import androidx.core.content.ContextCompat;
import androidx.navigation.NavController;
import androidx.navigation.Navigation;
import androidx.navigation.ui.AppBarConfiguration;
import androidx.navigation.ui.NavigationUI;
import com.coloryr.phoneapp.databinding.ActivityMainBinding;

import java.util.ArrayList;
import java.util.List;

public class MainActivity extends AppCompatActivity {

    @SuppressLint("StaticFieldLeak")
    public static MainActivity activity;
    @SuppressLint("StaticFieldLeak")
    public static BluetoothUtils bluetooth;
    @SuppressLint("StaticFieldLeak")
    public static BluetoothThread device;

    private AppBarConfiguration appBarConfiguration;
    public NavController nav;

    private final String[] permissions = new String[]{
            Manifest.permission.BLUETOOTH,
            Manifest.permission.BLUETOOTH_ADMIN,
            Manifest.permission.ACCESS_FINE_LOCATION,
            Manifest.permission.ACCESS_COARSE_LOCATION
    };

    private void isPermission() {
        List<String> mPermissionList = new ArrayList<>();
        for (String item : permissions) {
            if (ContextCompat.checkSelfPermission(this, item) != PackageManager.PERMISSION_GRANTED) {
                mPermissionList.add(item);
            }
        }
        if (mPermissionList.size() > 0) {
            show("APP需要请求一些权限才能正常使用");
            ActivityCompat.requestPermissions(this, permissions, 100);
        }
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        activity = this;
        isPermission();

        bluetooth = new BluetoothUtils();
        bluetooth.init();

        Tasks.start();

        ActivityMainBinding binding = ActivityMainBinding.inflate(getLayoutInflater());
        setContentView(binding.getRoot());

        setSupportActionBar(binding.toolbar);

        nav = Navigation.findNavController(this, R.id.nav_host_fragment_content_main);
        appBarConfiguration = new AppBarConfiguration.Builder(nav.getGraph()).build();
        NavigationUI.setupActionBarWithNavController(this, nav, appBarConfiguration);

//        activity.nav.navigate(R.id.navigation_second);
    }

    @Override
    public boolean onSupportNavigateUp() {
        NavController navController = Navigation.findNavController(this, R.id.nav_host_fragment_content_main);
        return NavigationUI.navigateUp(navController, appBarConfiguration)
                || super.onSupportNavigateUp();
    }

    public static void disconnect() {
        show("设备断开");
        run(() -> {
            device.close();
            activity.nav.navigateUp();
        });
    }

    public static void error(Exception e) {
        e.printStackTrace();
        run(() -> new AlertDialog.Builder(activity)
                .setTitle("错误")
                .setMessage("程序运行发生错误\n" + e)
                .setPositiveButton("确定", null)
                .show());
    }

    @SuppressLint("ResourceType")
    public static void select(BluetoothDevice device) {
        android.app.AlertDialog alertDialog = ProgressDialog.show(MainActivity.activity, "", "正在连接");
        if (MainActivity.device != null)
            MainActivity.device.close();
        MainActivity.device = new BluetoothThread(device);
        Tasks.run(() -> {
            boolean res = MainActivity.device.init();
            MainActivity.run(() -> {
                alertDialog.dismiss();
                if (res)
                    activity.nav.navigate(R.id.navigation_second);
            });
        });
    }

    public static void run(Runnable runnable) {
        Handler mainHandler = new Handler(Looper.getMainLooper());
        mainHandler.post(runnable);
    }

    public static void show(String data) {
        run(() -> Toast.makeText(activity, data, Toast.LENGTH_SHORT).show());
    }

    public static void info(String title, String message) {
        run(() -> new AlertDialog.Builder(activity)
                .setTitle(title)
                .setMessage(message)
                .setPositiveButton("确定", null)
                .show());
    }
}