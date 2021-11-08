package com.coloryr.phoneapp;

import android.bluetooth.BluetoothDevice;
import android.os.Bundle;
import android.view.*;
import android.view.animation.Animation;
import android.view.animation.LinearInterpolator;
import android.view.animation.RotateAnimation;
import android.widget.ImageView;
import android.widget.ListView;
import androidx.fragment.app.Fragment;

import java.util.ArrayList;

public class FirstFragment extends Fragment {

    private ListView list;
    private CheckListAdapter adapter;

    private ImageView load;
    private RotateAnimation rotate;
    private boolean isRun;

    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container, Bundle savedInstanceState) {
        View root = inflater.inflate(R.layout.fragment_first, container, false);
        list = root.findViewById(R.id.list);
        load = root.findViewById(R.id.list_load);

        adapter = new CheckListAdapter(root.getContext(),
                R.layout.list_item, new ArrayList<>());
        list.setAdapter(adapter);

        setHasOptionsMenu(true);

        start();
        getList();
        return root;
    }

    private void start() {
        rotate = new RotateAnimation(0f, 360f, Animation.RELATIVE_TO_SELF, 0.5f, Animation.RELATIVE_TO_SELF, 0.5f);
        LinearInterpolator lin = new LinearInterpolator();
        rotate.setInterpolator(lin);
        rotate.setDuration(1500);//设置动画持续时间
        rotate.setRepeatCount(-1);//设置重复次数
        rotate.setFillAfter(true);//动画执行完后是否停留在执行完的状态
        rotate.setStartOffset(10);//执行前的等待时间
        load.startAnimation(rotate);
        load.setVisibility(View.VISIBLE);
    }

    private void stop() {
        rotate.cancel();
        load.clearAnimation();
        load.setVisibility(View.GONE);
    }

    private void add(BluetoothDevice device) {
        adapter.add(device);
    }

    private void getList() {
        if (isRun)
            return;
        isRun = true;
        start();
        MainActivity.bluetooth.setReceiver(this::add);
        adapter.clear();
        if (!MainActivity.bluetooth.scan()) {
            stop();
            isRun = false;
            return;
        }
        Tasks.run(() -> {
            try {
                Thread.sleep(12000);
            } catch (InterruptedException e) {
                e.printStackTrace();
            }
            MainActivity.bluetooth.stop();
            MainActivity.run(() -> {
                stop();
                isRun = false;
            });
        });
    }

    @Override
    public void onCreateOptionsMenu(Menu menu, MenuInflater inflater) {
        menu.add(1, 1, 1, "刷新");
        super.onCreateOptionsMenu(menu, inflater);
    }

    //当OptionsMenu被选中的时候处理具体的响应事件
    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
        switch (item.getItemId()) {
            case 1:
                getList();
                break;
        }
        return super.onOptionsItemSelected(item);
    }
}