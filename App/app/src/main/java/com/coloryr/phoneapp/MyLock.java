package com.coloryr.phoneapp;

public class MyLock {
    private boolean lock;

    public void lock() {
        lock = true;
        while (lock) {
            try {
                Thread.sleep(1);
            } catch (Exception e) {

            }
        }
    }

    public void unlock() {
        lock = false;
    }
}
