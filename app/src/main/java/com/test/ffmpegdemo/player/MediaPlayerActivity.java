package com.test.ffmpegdemo.player;

import android.Manifest;
import android.os.Bundle;
import android.view.View;

import androidx.annotation.Nullable;
import androidx.appcompat.app.AppCompatActivity;

import com.mylhyl.acp.Acp;
import com.mylhyl.acp.AcpListener;
import com.mylhyl.acp.AcpOptions;
import com.test.ffmpegdemo.R;

import java.util.List;

public class MediaPlayerActivity extends AppCompatActivity implements View.OnClickListener, AcpListener {

    @Override
    protected void onCreate(@Nullable Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        setContentView(R.layout.activity_media_player);

        findViewById(R.id.test_btn).setOnClickListener(this);

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
            case R.id.test_btn:{
                MediaPlayer2 mediaPlayer2 = new MediaPlayer2();

                MediaClip[] mediaClips = new MediaClip[1];
                MediaClip mediaClip = new MediaClip();
                mediaClip.path = "/sdcard/DCIM/one_piece.mp4";
                mediaClip.timeRange = new MediaClip.TimeRange(0,40);
                mediaClip.type = 1;
                mediaClips[0] = mediaClip;
                mediaPlayer2.prepare(mediaClips);
                break;
            }
        }
    }

    @Override
    public void onGranted() {

    }

    @Override
    public void onDenied(List<String> permissions) {

    }
}
