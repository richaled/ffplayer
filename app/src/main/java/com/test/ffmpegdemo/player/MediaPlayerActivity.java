package com.test.ffmpegdemo.player;

import android.Manifest;
import android.os.Bundle;
import android.util.Log;
import android.view.View;
import android.widget.SeekBar;

import androidx.annotation.Nullable;
import androidx.appcompat.app.AppCompatActivity;

import com.mylhyl.acp.Acp;
import com.mylhyl.acp.AcpListener;
import com.mylhyl.acp.AcpOptions;
import com.test.ffmpegdemo.R;

import java.util.List;

public class MediaPlayerActivity extends AppCompatActivity implements View.OnClickListener, AcpListener, SeekBar.OnSeekBarChangeListener {

    private static final String TAG = MediaPlayerActivity.class.getName();
    private MediaPlayerView mediaPlayerView;
    private SeekBar seekBar;

    @Override
    protected void onCreate(@Nullable Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        setContentView(R.layout.activity_media_player);

        findViewById(R.id.test_btn).setOnClickListener(this);
        mediaPlayerView = findViewById(R.id.media_player_view);
        seekBar = findViewById(R.id.seek_bar);
        seekBar.setOnSeekBarChangeListener(this);

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
                MediaPlayer2 mediaPlayer = mediaPlayerView.getMediaPlayer();

                MediaClip[] mediaClips = new MediaClip[1];
                MediaClip mediaClip = new MediaClip();
                mediaClip.path = "/sdcard/DCIM/one_piece.mp4";
                mediaClip.timeRange = new MediaClip.TimeRange(0,60000);
                mediaClip.type = 1;
                mediaClips[0] = mediaClip;
                mediaPlayer.setMediaClipSource(mediaClips);

//                Options options = new Options();
//                options.setBool(Options.OptionKey.kEnableHardware,false);
//                mediaPlayer.prepare(options);

//                mediaPlayer.play();

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

    @Override
    public void onProgressChanged(SeekBar seekBar, int progress, boolean fromUser) {
        if(fromUser){
            MediaPlayer2 mediaPlayer = mediaPlayerView.getMediaPlayer();
            if(mediaPlayer.isPlaying()){
                mediaPlayerView.pauseView();
                mediaPlayer.pause();
            }
            long durationMs = mediaPlayer.getDurationMs();
            long seekTime = (long) (1.0f * progress / 100 * durationMs + 0);/*clip startTime*/
            Log.i(TAG,"durationMs : " + durationMs + ",seekTime : " + seekTime);
            mediaPlayer.seek(seekTime);
        }
    }

    @Override
    public void onStartTrackingTouch(SeekBar seekBar) {

    }

    @Override
    public void onStopTrackingTouch(SeekBar seekBar) {

    }
}
