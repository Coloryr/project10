package com.coloryr.phoneapp;

import android.annotation.SuppressLint;
import android.bluetooth.BluetoothDevice;
import android.content.Context;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.ArrayAdapter;
import android.widget.TextView;
import androidx.annotation.NonNull;
import androidx.annotation.Nullable;

import java.util.List;

public class CheckListAdapter extends ArrayAdapter<BluetoothDevice> {

    public static class ViewHolder {
        public TextView name;
    }

    private final int resourceId;
    private final List<BluetoothDevice> objs;

    @Override
    public void add(BluetoothDevice object) {
        for (BluetoothDevice item : objs) {
            if (item.equals(object))
                return;
        }
        super.add(object);
    }

    public CheckListAdapter(Context context, int headImage, List<BluetoothDevice> obj) {
        super(context, headImage, obj);
        objs = obj;
        resourceId = headImage;
    }

    @SuppressLint("ViewHolder")
    @NonNull
    @Override
    public View getView(final int position, @Nullable View convertView, @NonNull ViewGroup parent) {
        BluetoothDevice item = getItem(position);
        View view;
        ViewHolder viewHolder;
        if (convertView == null) {

            view = LayoutInflater.from(getContext()).inflate(resourceId, parent, false);

            viewHolder = new ViewHolder();
            viewHolder.name = view.findViewById(R.id.item_name);

            view.setTag(viewHolder);
            view.setOnClickListener(v -> MainActivity.select(item));
        } else {
            view = convertView;
            viewHolder = (ViewHolder) view.getTag();
        }

        viewHolder.name.setText(item.getName());
        return view;
    }
}
