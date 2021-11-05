package com.coloryr.phoneapp;

import android.bluetooth.*;
import android.util.Log;

import java.lang.reflect.Array;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.List;
import java.util.UUID;

public class BluetoothThread {
    private static final UUID SERVICE_UUID = UUID.fromString("6E400001-B5A3-F393-E0A9-E50E24DCCA9E");
    private static final UUID RX_UUID = UUID.fromString("6E400002-B5A3-F393-E0A9-E50E24DCCA9E");
    private static final UUID TX_UUID = UUID.fromString("6E400003-B5A3-F393-E0A9-E50E24DCCA9E");

    private final BluetoothDevice mmDevice;
    private BluetoothGatt mGatt;
    private BluetoothGattService service;
    private BluetoothGattCharacteristic rx;
    private BluetoothGattCharacteristic tx;

    private boolean isOk;
    private MyLock lock = new MyLock();

    private boolean haveData;
    private int size;
    private final List<byte[]> tempdata = new ArrayList<>();

    public byte[] data;
    public float[] data1 = new float[5];
    public float data2;

    private final BluetoothGattCallback mCallBack = new BluetoothGattCallback() {

        @Override
        public void onConnectionStateChange(BluetoothGatt gatt, int status,
                                            int newState) {
            if (status == BluetoothGatt.GATT_SUCCESS) {
                if (newState == BluetoothProfile.STATE_CONNECTED) {
                    gatt.discoverServices();
                } else {
                    isOk = false;
                }
            } else {
                MainActivity.disconnect();
            }
        }

        @Override
        public void onServicesDiscovered(final BluetoothGatt gatt, int status) {
            if (status == BluetoothGatt.GATT_SUCCESS) {
                service = gatt.getService(SERVICE_UUID);
                if (service == null) {
                    MainActivity.disconnect();
                    MainActivity.info("连接失败", "不匹配的蓝牙设备");
                } else {
                    rx = service.getCharacteristic(RX_UUID);
                    tx = service.getCharacteristic(TX_UUID);
                    if (rx == null || tx == null) {
                        MainActivity.disconnect();
                        MainActivity.info("连接失败", "不匹配的蓝牙设备");
                    }
                    boolean isEnableNotification = gatt.setCharacteristicNotification(tx, true);
                    if (isEnableNotification) {
                        List<BluetoothGattDescriptor> descriptorList = tx.getDescriptors();
                        if (descriptorList != null && descriptorList.size() > 0) {
                            for (BluetoothGattDescriptor descriptor : descriptorList) {
                                descriptor.setValue(BluetoothGattDescriptor.ENABLE_NOTIFICATION_VALUE);
                                gatt.writeDescriptor(descriptor);
                            }
                        }
                        isOk = true;
                    } else {
                        MainActivity.disconnect();
                        MainActivity.info("连接失败", "不匹配的蓝牙设备");
                    }
                }
            } else {
                MainActivity.disconnect();
                MainActivity.info("连接失败", "不匹配的蓝牙设备");
            }
            lock.unlock();
        }

        @Override
        public void onCharacteristicChanged(BluetoothGatt gatt,
                                            BluetoothGattCharacteristic characteristic) {
            read(characteristic.getValue());
        }
    };

    public BluetoothThread(BluetoothDevice device) {
        mmDevice = device;
    }

    public boolean init() {
        isOk = false;
        mGatt = mmDevice.connectGatt(MainActivity.activity, false, mCallBack);
        lock.lock();
        return isOk;
    }

    public void send(String data) {
        if (isOk) {
            rx.setWriteType(BluetoothGattCharacteristic.WRITE_TYPE_DEFAULT);
            rx.setValue(data);
            mGatt.writeCharacteristic(rx);
        }
    }

    private boolean nowRead;

    private void read(byte[] data) {
        if (nowRead) {
            if (size > 0) {
                tempdata.add(data);
                size -= data.length;
            }
            if (size == 0) {
                int all = 0;
                for (byte[] item : tempdata) {
                    all += item.length;
                }
                this.data = new byte[all];
                all = 0;
                for (byte[] item : tempdata) {
                    System.arraycopy(item, 0, this.data, all, item.length);
                    all += item.length;
                }
                tempdata.clear();
                nowRead = false;
                haveData = true;
            }
            return;
        }
        String temp = new String(data);
        if (temp.startsWith("points:")) {
            temp = temp.substring(7);
            size = Integer.parseInt(temp);
            Log.i("Test", "points:" + size);
            nowRead = true;
        } else if (temp.startsWith("range")) {
            temp = temp.substring(5);
            int temp2 = Integer.parseInt(temp.substring(0, 1));
            temp = temp.substring(2);
            this.data1[temp2] = Float.parseFloat(temp);
        } else if (temp.startsWith("thd:")) {
            temp = temp.substring(4);
            this.data2 = Float.parseFloat(temp);
        }
    }

    public void close() {
        if (mGatt != null)
            mGatt.close();
    }

    public float[] getData1() {
        return data1;
    }

    public float getData2() {
        return data2;
    }

    public byte[] get() {
        if (haveData) {
            haveData = false;
            return data;
        } else
            return null;
    }
}