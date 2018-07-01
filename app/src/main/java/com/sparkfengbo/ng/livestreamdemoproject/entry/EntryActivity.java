package com.sparkfengbo.ng.livestreamdemoproject.entry;

import com.sparkfengbo.ng.livestreamdemoproject.R;

import android.app.Activity;
import android.os.Bundle;
import android.support.v7.widget.LinearLayoutManager;
import android.support.v7.widget.RecyclerView;

public class EntryActivity extends Activity {

    private RecyclerView mRecycleListView;
    private EntryAdapter mEntryAdapter;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_entry);

        mEntryAdapter = new EntryAdapter();

        mRecycleListView = (RecyclerView) findViewById(R.id.recycle_view);
        mRecycleListView.setLayoutManager(new LinearLayoutManager(this));
        mRecycleListView.setAdapter(mEntryAdapter);


    }
}
