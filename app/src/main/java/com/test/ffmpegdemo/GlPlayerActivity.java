package com.test.ffmpegdemo;

import android.Manifest;
import android.os.Bundle;
import android.util.Log;
import android.view.View;

import androidx.annotation.Nullable;
import androidx.appcompat.app.AppCompatActivity;

import com.mylhyl.acp.Acp;
import com.mylhyl.acp.AcpListener;
import com.mylhyl.acp.AcpOptions;

import java.io.File;
import java.util.List;

public class GlPlayerActivity extends AppCompatActivity implements View.OnClickListener, AcpListener {

    private static final String TAG = GlPlayerActivity.class.getName();
    private String url = "/storage/emulated/0/byteflow/one_piece.mp4";
    private PlayerView mPlayerView;
    @Override
    protected void onCreate(@Nullable Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        setContentView(R.layout.activity_player);

        mPlayerView = findViewById(R.id.player_view);
        findViewById(R.id.play_btn).setOnClickListener(this);
        requestPremission();
    }

    private void requestPremission() {
        Acp.getInstance(this).request(
                new AcpOptions.Builder()
                        .setPermissions(
                                Manifest.permission.READ_EXTERNAL_STORAGE,
                                Manifest.permission.WRITE_EXTERNAL_STORAGE
                        ).build(), this
        );
    }

    @Override
    public void onClick(View v) {
        switch (v.getId()){
            case R.id.play_btn:{
                playVideo();
                break;
            }
        }
    }

    private void playVideo() {

        File file = new File(url);
        boolean exists = file.exists();
        Log.i(TAG,"exists : " + exists);

        int prepare = mPlayerView.prepare(url);
        if(prepare != 0){
            Log.i(TAG,"video player prepare fail :" + prepare);
            return;
        }
        Log.i(TAG,"prepare init success");
        mPlayerView.play();
    }

    @Override
    public void onGranted() {

    }

    @Override
    public void onDenied(List<String> permissions) {

    }
}
