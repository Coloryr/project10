package com.coloryr.phoneapp;

import android.os.Bundle;
import android.view.LayoutInflater;
import android.view.Menu;
import android.view.View;
import android.view.ViewGroup;

import android.widget.ListView;
import androidx.annotation.NonNull;
import androidx.appcompat.widget.PopupMenu;
import androidx.fragment.app.Fragment;
import androidx.navigation.fragment.NavHostFragment;
import com.coloryr.phoneapp.databinding.FragmentSecondBinding;

import java.util.ArrayList;

public class SecondFragment extends Fragment {

    private ListView list;
    private CheckListAdapter adapter;

    public View onCreateView(@NonNull LayoutInflater inflater,
                             ViewGroup container, Bundle savedInstanceState) {
        View root = inflater.inflate(R.layout.fragment_second, container, false);
        list = root.findViewById(R.id.list);
        adapter  = new CheckListAdapter(root.getContext(),
                R.layout.list_item, new ArrayList<>());
        list.setAdapter(adapter);
        getList();
        return root;
    }

    private void getList(){

    }
}