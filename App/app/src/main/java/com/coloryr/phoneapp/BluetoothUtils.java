package com.coloryr.phoneapp;

import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.location.LocationManager;

public class BluetoothUtils {

    private BluetoothAdapter mBluetoothAdapter;
    private final static int REQUEST_ENABLE_BT = 1;
    private IRes res;

    public void init() {
        mBluetoothAdapter = BluetoothAdapter.getDefaultAdapter();
        if (mBluetoothAdapter == null) {
            MainActivity.show("不支持蓝牙");
            return;
        }
        if (!mBluetoothAdapter.isEnabled()) {
            Intent enableBtIntent = new Intent(BluetoothAdapter.ACTION_REQUEST_ENABLE);
            MainActivity.activity.startActivityForResult(enableBtIntent, REQUEST_ENABLE_BT);
        }
        LocationManager manager = (LocationManager) MainActivity.activity.getSystemService(Context.LOCATION_SERVICE);
        boolean gpsProviderOK = manager.isProviderEnabled(LocationManager.GPS_PROVIDER);
        boolean networkProviderOK = manager.isProviderEnabled(LocationManager.NETWORK_PROVIDER);
        if (!(gpsProviderOK || networkProviderOK)) {
            MainActivity.info("扫描错误", "需要打开定位才能扫描到蓝牙设备");
        }
        IntentFilter filter = new IntentFilter(BluetoothDevice.ACTION_FOUND);
        MainActivity.activity.registerReceiver(mReceiver, filter);
    }

    private final BroadcastReceiver mReceiver = new BroadcastReceiver() {
        public void onReceive(Context context, Intent intent) {
            String action = intent.getAction();
            if (BluetoothDevice.ACTION_FOUND.equals(action)) {
                BluetoothDevice device = intent.getParcelableExtra(BluetoothDevice.EXTRA_DEVICE);
                if (res != null)
                    res.find(device);
            }
        }
    };

    public void setReceiver(IRes res) {
        this.res = res;
    }

    public boolean scan() {
        boolean res = mBluetoothAdapter.startDiscovery();
        if (!res) {
            MainActivity.info("扫描错误", "需要开启定位才能扫描设备");
        }
        return res;
    }

    public void stop() {
        mBluetoothAdapter.cancelDiscovery();
    }
}
