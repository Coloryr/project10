package com.coloryr.phoneapp;

import android.annotation.SuppressLint;
import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.Paint;
import android.graphics.Path;
import android.opengl.GLSurfaceView;
import android.os.Bundle;
import android.view.*;
import android.widget.ListView;
import android.widget.TextView;
import androidx.annotation.NonNull;
import androidx.fragment.app.Fragment;

public class SecondFragment extends Fragment {

    private SurfaceView view;
    private SurfaceHolder mHolder;
    private Canvas mCanvas;
    private Paint mPaint;
    private Path mPath;

    private TextView thd;
    private TextView t1;
    private TextView t2;
    private TextView t3;
    private TextView t4;
    private TextView t5;

    private boolean init;
    private Thread thread = new Thread(this::run);
    private boolean isRun;

    public View onCreateView(@NonNull LayoutInflater inflater,
                             ViewGroup container, Bundle savedInstanceState) {
        View root = inflater.inflate(R.layout.fragment_second, container, false);
        view = root.findViewById(R.id.surfaceView);
        thd = root.findViewById(R.id.text_thd);
        t1 = root.findViewById(R.id.text_1);
        t2 = root.findViewById(R.id.text_2);
        t3 = root.findViewById(R.id.text_3);
        t4 = root.findViewById(R.id.text_4);
        t5 = root.findViewById(R.id.text_5);
        mHolder = view.getHolder();
        mHolder.addCallback(new SurfaceHolder.Callback() {
            @Override
            public void surfaceCreated(@NonNull SurfaceHolder holder) {
                clear();
                init = true;
            }

            @Override
            public void surfaceChanged(@NonNull SurfaceHolder holder, int format, int width, int height) {

            }

            @Override
            public void surfaceDestroyed(@NonNull SurfaceHolder holder) {
                init = false;
            }
        });
        isRun = true;
        thread.start();
        return root;
    }

    private void clear() {
        mCanvas = mHolder.lockCanvas();
        mCanvas.drawColor(Color.BLACK);
        int height = mCanvas.getHeight();
        int width = mCanvas.getWidth();
        mPath = new Path();
        mPath.moveTo(0, 0);
        mPaint = new Paint();
        mPaint.setColor(Color.parseColor("#8800FF00"));
        for (int a = 0; a < height; a += 50) {
            mCanvas.drawLine(0, a, width, a, mPaint);
        }
        for (int a = 0; a < width; a += 50) {
            mCanvas.drawLine(a, 0, a, height, mPaint);
        }
        mHolder.unlockCanvasAndPost(mCanvas);
    }

    @SuppressLint("SetTextI18n")
    private void run() {
        while (isRun) {
            try {
                Thread.sleep(500);
                if (!init)
                    continue;
                if (MainActivity.device != null) {
                    byte[] data = MainActivity.device.get();
                    if (data != null) {
                        start(data);
                        float[] temp = MainActivity.device.getData1();
                        MainActivity.run(() -> {
                            thd.setText(MainActivity.device.getData2() + "%");
                            t1.setText("" + temp[0]);
                            t2.setText("" + temp[1]);
                            t3.setText("" + temp[2]);
                            t4.setText("" + temp[3]);
                            t5.setText("" + temp[4]);
                        });
                    }
                }
            } catch (Exception e) {
                MainActivity.error(e);
            }
        }
    }

    private void start(byte[] data) {
        mCanvas = mHolder.lockCanvas();
        mCanvas.drawColor(Color.BLACK);
        int height = mCanvas.getHeight();
        int width = mCanvas.getWidth();
        float ax = (float) width / data.length;
        float nowx = 0f;
        mPath = new Path();
        mPaint = new Paint();
        mPaint.setColor(Color.parseColor("#8800FF00"));
        for (int a = 0; a < height; a += 50) {
            mCanvas.drawLine(0, a, width, a, mPaint);
        }
        for (int a = 0; a < width; a += 50) {
            mCanvas.drawLine(a, 0, a, height, mPaint);
        }
        mPaint.setColor(Color.parseColor("#00FFFF"));
        mPaint.setStyle(Paint.Style.STROKE);
        mPaint.setStrokeWidth(2);
        mPath.moveTo(nowx,data[0] * 3 + 400);
        for (byte datum : data) {
            mPath.lineTo(nowx, (datum * 3) + 400);
            mCanvas.drawPath(mPath, mPaint);
            nowx += ax;
        }
        mHolder.unlockCanvasAndPost(mCanvas);
    }
}