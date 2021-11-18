package com.example.tgr;

import android.util.Log;

import java.io.DataOutputStream;
import java.io.IOException;
import java.net.ServerSocket;
import java.net.Socket;
import java.util.concurrent.LinkedBlockingQueue;

public class DataSender implements Runnable{
    public static final byte MOTOR_LEFT = 1;
    public static final byte MOTOR_RIGHT = 2;
    public static final byte LIGHT = 3;
    public static final byte DIRECTION_FORWARD = 1;
    public static final byte DIRECTION_BACKWARD = 2;
    public static final byte LIGHT_ON = 1;
    public static final byte LIGHT_OFF = 2;


    LinkedBlockingQueue<byte[]> queue;
    private final int PORT;

    Socket socket;
    private boolean isConnected;
    ServerSocket serverSocket;

    public DataSender(LinkedBlockingQueue<byte[]> queue, int PORT) {
        this.queue = queue;
        this.PORT = PORT;
        isConnected = false;
    }


    @Override
    public void run() {
        try {
            serverSocket = new ServerSocket(PORT);
            socket = serverSocket.accept();
            Log.d("JoyStickSender", "Connected to " + PORT);
            DataOutputStream dos = new DataOutputStream(socket.getOutputStream());
            isConnected = true;
            while(!socket.isClosed()){
                byte[] data = queue.take();
                dos.write(data);
            }
        } catch (IOException e) {
            e.printStackTrace();
            isConnected = false;
        }
        catch (Exception e) {
            e.printStackTrace();
        }
        finally {
            close();
        }
    }

    public boolean getIsConnected(){
        return isConnected;
    }

    public void close(){
        try {
            socket.close();
            serverSocket.close();
        } catch (IOException e) {
            e.printStackTrace();
        }
    }
}
