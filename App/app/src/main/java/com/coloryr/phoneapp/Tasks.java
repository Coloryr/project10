package com.coloryr.phoneapp;

import java.util.Queue;
import java.util.concurrent.ConcurrentLinkedQueue;

public class Tasks {
    private static Thread[] threads;
    private static boolean isRun;
    private static final Queue<Runnable> tasks = new ConcurrentLinkedQueue<>();

    public static void start() {
        threads = new Thread[2];
        isRun = true;
        for (int a = 0; a < threads.length; a++) {
            Thread thread = new Thread(Tasks::run, "逻辑线程");
            thread.start();
            threads[a] = thread;
        }
    }

    private static void run() {
        while (isRun) {
            try {
                Runnable runnable = tasks.poll();
                if (runnable != null) {
                    runnable.run();
                }
                Thread.sleep(100);
            } catch (Exception e) {
                MainActivity.error(e);
            }
        }
    }

    public static void stop() {
        isRun = false;
    }

    public static void run(Runnable runnable) {
        tasks.add(runnable);
    }
}
