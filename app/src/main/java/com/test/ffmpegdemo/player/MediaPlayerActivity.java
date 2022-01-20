package com.test.ffmpegdemo.player;

import android.os.Bundle;
import android.view.View;

import androidx.annotation.Nullable;
import androidx.appcompat.app.AppCompatActivity;

import com.test.ffmpegdemo.R;

public class MediaPlayerActivity extends AppCompatActivity implements View.OnClickListener {

    @Override
    protected void onCreate(@Nullable Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        setContentView(R.layout.activity_media_player);

        findViewById(R.id.test_btn).setOnClickListener(this);
    }

    @Override
    public void onClick(View v) {
        switch (v.getId()){
            case R.id.test_btn:{
                MediaPlayer2 mediaPlayer2 = new MediaPlayer2();

                MediaClip[] mediaClips = new MediaClip[1];
                MediaClip mediaClip = new MediaClip();
                mediaClip.path = "xxxxx";
                mediaClip.timeRange = new MediaClip.TimeRange(0,40);
                mediaClip.type = 1;
                mediaClips[0] = mediaClip;
                mediaPlayer2.prepare(mediaClips);
                break;
            }
        }
    }
}
