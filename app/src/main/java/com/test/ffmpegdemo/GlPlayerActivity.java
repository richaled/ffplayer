package com.test.ffmpegdemo;

import android.Manifest;
import android.os.Bundle;
import android.util.Log;
import android.view.View;
import android.widget.Button;
import android.widget.SeekBar;
import android.widget.TextView;

import androidx.annotation.Nullable;
import androidx.appcompat.app.AppCompatActivity;

import com.mylhyl.acp.Acp;
import com.mylhyl.acp.AcpListener;
import com.mylhyl.acp.AcpOptions;

import java.io.File;
import java.text.SimpleDateFormat;
import java.util.List;

public class GlPlayerActivity extends AppCompatActivity implements View.OnClickListener, AcpListener, SeekBar.OnSeekBarChangeListener, VideoPlayer.OnPlayProgressCallback, VideoPlayer.OnPlayerStateCallback {

    private static final String TAG = GlPlayerActivity.class.getName();
    private String url = "/sdcard/DCIM/one_piece.mp4";
    private PlayerView mPlayerView;
    private boolean isPlaying = false;
    private Button mPlayBtn;
    private SeekBar mSeekBar;
    private TextView mProgressText;
    @Override
    protected void onCreate(@Nullable Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        setContentView(R.layout.activity_player);

        mPlayerView = findViewById(R.id.player_view);
        mSeekBar = findViewById(R.id.seek_bar);
        mPlayBtn = findViewById(R.id.play_btn);
        mProgressText = findViewById(R.id.play_progress);
        mPlayBtn.setOnClickListener(this);
        mSeekBar.setOnSeekBarChangeListener(this);
        mPlayerView.setPlayProgressCallback(this);
        mPlayerView.setPlayStateCallback(this);
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

        if(isPlaying){
            mPlayerView.pause();
            mPlayBtn.setText("播放");
        }else {
            if (!mPlayerView.isPrepare()){
                int prepare = mPlayerView.prepare(url);
                if(prepare != 0){
                    Log.i(TAG,"video player prepare fail :" + prepare);
                    return;
                }
                Log.i(TAG,"prepare init success");
            }

            mPlayerView.play();
            mPlayBtn.setText("暂停");
        }
        isPlaying = !isPlaying;
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
            if(mPlayerView.isPrepare()){
                float duration = mPlayerView.getDuration();
                float v = 1.0f * progress / 100 * duration;
                mPlayerView.seek(v);
            }
        }
    }

    @Override
    public void onStartTrackingTouch(SeekBar seekBar) {

    }

    @Override
    public void onStopTrackingTouch(SeekBar seekBar) {

    }

    @Override
    public void onPlayProgress(float progress, float total) {
        Log.i(TAG,"play progress : " + progress + ",total : " + total);

        runOnUiThread(() -> {
            mSeekBar.setProgress((int) (progress/ total * 100));
            String time = getTime(progress) + "/" + getTime(total);
            mProgressText.setText(time);
        });
    }

    @Override
    public void onPlayState(int state) {
        Log.i(TAG,"play state : " + state);
        switch (state){
            case VideoPlayer.PlayState.READY:{
                break;
            }
            case VideoPlayer.PlayState.DONE:{
                runOnUiThread(()->{
                    mSeekBar.setProgress(0);
                    mProgressText.setText("00:00/00:00");
                });
                break;
            }

        }
    }


    public static String getTime(float millSeconds) {
        SimpleDateFormat format = new SimpleDateFormat("mm:ss");
        return format.format(millSeconds);
//        int seconds = (int) (millSeconds / 1000);
//        int h = seconds / 3600;
//        int m = (seconds % 3600) / 60;
//        int s = (seconds % 3600) % 60;
//        if(h > 0){
//            return h + "." + m + ":" + s;
//        }
//        return m + ":" + s;
    }
}
