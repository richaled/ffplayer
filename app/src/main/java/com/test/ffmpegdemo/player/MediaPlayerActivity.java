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


    private MediaPlayerView mediaPlayerView;

    @Override
    protected void onCreate(@Nullable Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        setContentView(R.layout.activity_media_player);

        findViewById(R.id.test_btn).setOnClickListener(this);
        mediaPlayerView = findViewById(R.id.media_player_view);

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
//                MediaPlayer2 mediaPlayer2 = new MediaPlayer2();
                MediaPlayer2 mediaPlayer = mediaPlayerView.getMediaPlayer();

                MediaClip[] mediaClips = new MediaClip[1];
                MediaClip mediaClip = new MediaClip();
                mediaClip.path = "/sdcard/DCIM/one_piece.mp4";
                mediaClip.timeRange = new MediaClip.TimeRange(0,40);
                mediaClip.type = 1;
                mediaClips[0] = mediaClip;
                mediaPlayer.setMediaClipSource(mediaClips);

//                Options options = new Options();
//                options.setBool(Options.OptionKey.kEnableHardware,false);
//                mediaPlayer.prepare(options);

                mediaPlayer.play();

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
