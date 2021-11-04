package com.coloryr.phoneapp;

import android.bluetooth.BluetoothAdapter;

public class BluetoothUtils {
    private BluetoothAdapter mBluetoothAdapter;

    public void init() {
        mBluetoothAdapter = BluetoothAdapter.getDefaultAdapter();
        if (mBluetoothAdapter == null) {
            MainActivity.show("不支持蓝牙");
        }
    }
}
