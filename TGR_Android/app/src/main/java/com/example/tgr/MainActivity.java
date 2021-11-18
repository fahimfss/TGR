package com.example.tgr;

import androidx.appcompat.app.AppCompatActivity;

import android.annotation.SuppressLint;
import android.os.Bundle;
import android.view.MotionEvent;
import android.view.View;
import android.view.ViewTreeObserver;
import android.widget.Button;
import android.widget.ImageView;
import android.widget.RelativeLayout;


import java.util.concurrent.LinkedBlockingQueue;

import io.github.controlwear.virtual.joystick.android.JoystickView;

public class MainActivity extends AppCompatActivity {
    public static final int PORT_CAM = 9001;
    public static final int PORT_DATA = 8999;

    LinkedBlockingQueue<byte[]> queue;

    DataSender dataSender;
    Thread dataThread, camThread;

    ImageView camViewer;

    ImageReceiver imageReceiver;

    @SuppressLint("ClickableViewAccessibility")
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        queue = new LinkedBlockingQueue<>();

        JoystickView joystickLeft = findViewById(R.id.joystickView1);
        joystickLeft.getViewTreeObserver().addOnGlobalLayoutListener(new ViewTreeObserver.OnGlobalLayoutListener() {
            @Override
            public void onGlobalLayout() {
                joystickLeft.getViewTreeObserver().removeOnGlobalLayoutListener(this);

                RelativeLayout.LayoutParams param = new RelativeLayout.LayoutParams(
                        RelativeLayout.LayoutParams.MATCH_PARENT,
                        RelativeLayout.LayoutParams.MATCH_PARENT
                );
                double space = joystickLeft.getHeight() * 0.25;
                int left = -(int)space;

                param.setMargins(left, 0, 0, 0);
//                Log.d("joystickView1", left + "");
                joystickLeft.setLayoutParams(param);
            }
        });

        joystickLeft.setOnMoveListener(new JoystickView.OnMoveListener() {
            @Override
            public void onMove(int angle, int strength) {
                if(dataSender.getIsConnected()){
                    byte direction = DataSender.DIRECTION_FORWARD;
                    if(angle > 100) direction = DataSender.DIRECTION_BACKWARD;
                    byte[] data = {DataSender.MOTOR_LEFT, direction, (byte) strength};
                    try {
                        queue.put(data);
                    } catch (InterruptedException e) {
                        e.printStackTrace();
                    }
                }
            }
        }, 25);

        JoystickView joystickRight = findViewById(R.id.joystickView2);

        joystickRight.getViewTreeObserver().addOnGlobalLayoutListener(new ViewTreeObserver.OnGlobalLayoutListener() {
            @Override
            public void onGlobalLayout() {
                joystickRight.getViewTreeObserver().removeOnGlobalLayoutListener(this);

                RelativeLayout.LayoutParams param = new RelativeLayout.LayoutParams(
                        RelativeLayout.LayoutParams.MATCH_PARENT,
                        RelativeLayout.LayoutParams.MATCH_PARENT
                );
                param.addRule(RelativeLayout.ALIGN_PARENT_END);
                double space = joystickRight.getHeight() * 0.25;
                int right = -(int)space;

                param.setMargins(0, 0, right, 0);
//                Log.d("joystickView2", right + "");
                joystickRight.setLayoutParams(param);
            }
        });

        joystickRight.setOnMoveListener(new JoystickView.OnMoveListener() {
            @Override
            public void onMove(int angle, int strength) {
                if(dataSender.getIsConnected()){
                    byte direction = DataSender.DIRECTION_FORWARD;
                    if(angle > 100) direction = DataSender.DIRECTION_BACKWARD;
                    byte[] data = {DataSender.MOTOR_RIGHT, direction, (byte) strength};
                    try {
                        queue.put(data);
                    } catch (InterruptedException e) {
                        e.printStackTrace();
                    }
                }
            }
        }, 25);

        Button lightButton = findViewById(R.id.lightButton);
        lightButton.setOnClickListener(new View.OnClickListener() {
            int state = DataSender.LIGHT_OFF;
            @Override
            public void onClick(View view) {
                if(state == DataSender.LIGHT_OFF){
                    state = DataSender.LIGHT_ON;
                    lightButton.setText("LIGHTS ON");
                    lightButton.setBackgroundColor(lightButton.getContext().getResources().getColor(R.color.transparent_green));
                    byte[] data = {DataSender.LIGHT, DataSender.LIGHT_ON, 0};
                    try {
                        queue.put(data);
                    } catch (InterruptedException e) {
                        e.printStackTrace();
                    }
                }
                else{
                    state = DataSender.LIGHT_OFF;
                    lightButton.setText("LIGHTS OFF");
                    lightButton.setBackgroundColor(lightButton.getContext().getResources().getColor(R.color.transparent_red));
                    byte[] data = {DataSender.LIGHT, DataSender.LIGHT_OFF, 0};
                    try {
                        queue.put(data);
                    } catch (InterruptedException e) {
                        e.printStackTrace();
                    }
                }
            }
        });

        camViewer = findViewById(R.id.camViewer);
    }

    @Override
    protected void onPause() {
        super.onPause();
        dataSender.close();
        dataThread.interrupt();

        imageReceiver.close();
        camThread.interrupt();
    }

    @Override
    protected void onResume() {
        super.onResume();
        dataSender = new DataSender(queue, PORT_DATA);
        dataThread = new Thread(dataSender);

        imageReceiver = new ImageReceiver(camViewer, PORT_CAM);
        camThread = new Thread(imageReceiver);

        camThread.start();
        dataThread.start();

    }
}