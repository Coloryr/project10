package com.coloryr.phoneapp;

import android.Manifest;
import android.annotation.SuppressLint;
import android.content.pm.PackageManager;
import android.os.Build;
import android.os.Bundle;
import android.os.Handler;
import android.os.Looper;
import android.widget.Toast;
import androidx.core.app.ActivityCompat;
import androidx.core.content.ContextCompat;
import androidx.navigation.fragment.NavHostFragment;
import com.google.android.material.snackbar.Snackbar;
import androidx.appcompat.app.AppCompatActivity;
import android.view.View;
import androidx.navigation.NavController;
import androidx.navigation.Navigation;
import androidx.navigation.ui.AppBarConfiguration;
import androidx.navigation.ui.NavigationUI;
import com.coloryr.phoneapp.databinding.ActivityMainBinding;

import android.view.Menu;
import android.view.MenuItem;

import java.util.ArrayList;
import java.util.List;

public class MainActivity extends AppCompatActivity {

    @SuppressLint("StaticFieldLeak")
    public static MainActivity activity;
    @SuppressLint("StaticFieldLeak")
    public static BluetoothUtils bluetooth;

    private AppBarConfiguration appBarConfiguration;
    private NavController nav;

    private final String[] permissions = new String[]{
            Manifest.permission.BLUETOOTH,
            Manifest.permission.BLUETOOTH_ADMIN
    };

    private void isPermission() {
        if (Build.VERSION.SDK_INT >= 26) {
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
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        activity = this;
        isPermission();
        ActivityMainBinding binding = ActivityMainBinding.inflate(getLayoutInflater());
        setContentView(binding.getRoot());

        setSupportActionBar(binding.toolbar);

        nav = Navigation.findNavController(this, R.id.nav_host_fragment_content_main);
        appBarConfiguration = new AppBarConfiguration.Builder(nav.getGraph()).build();
        NavigationUI.setupActionBarWithNavController(this, nav, appBarConfiguration);

        bluetooth = new BluetoothUtils();
        bluetooth.init();
    }

    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        getMenuInflater().inflate(R.menu.menu_main, menu);
        return true;
    }

    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
        int id = item.getItemId();

        if (id == R.id.action_settings) {
            nav.navigate(R.id.action_FirstFragment_to_SecondFragment);
        }

        return super.onOptionsItemSelected(item);
    }

    @Override
    public boolean onSupportNavigateUp() {
        NavController navController = Navigation.findNavController(this, R.id.nav_host_fragment_content_main);
        return NavigationUI.navigateUp(navController, appBarConfiguration)
                || super.onSupportNavigateUp();
    }

    public static void select(String item) {

    }

    public static void run(Runnable runnable) {
        Handler mainHandler = new Handler(Looper.getMainLooper());
        mainHandler.post(runnable);
    }

    public static void show(String data) {
        run(() -> Toast.makeText(activity, data, Toast.LENGTH_SHORT).show());
    }
}