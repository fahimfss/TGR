package com.example.tgr;

import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.os.Handler;
import android.os.Looper;
import android.widget.ImageView;

import java.io.DataInputStream;
import java.io.IOException;
import java.net.ServerSocket;
import java.net.Socket;

public class ImageReceiver implements Runnable{
    private ServerSocket serverSocket;
    private Socket socket;
    private final ImageView camViewer;
    private final int port;


    public ImageReceiver(ImageView camViewer, int port){
        this.camViewer = camViewer;
        this.port = port;
    }

    @Override
    public void run() {
        try {
            serverSocket = new ServerSocket(port);
            socket = serverSocket.accept();

            DataInputStream dis = new DataInputStream(socket.getInputStream());

            while(!socket.isClosed()){
                byte[] lenBytes = new byte[4];
                dis.readFully(lenBytes);
                int lenInt = java.nio.ByteBuffer.wrap(lenBytes).getInt();
                byte[] imgByte = new byte[lenInt];
                dis.readFully(imgByte);
                Bitmap bmp = BitmapFactory.decodeByteArray(imgByte, 0, imgByte.length);

                new Handler(Looper.getMainLooper()).post(new Runnable() {
                    @Override
                    public void run() {
                        camViewer.setImageBitmap(bmp);
                    }
                });
            }
        }
        catch (IOException e){
            e.printStackTrace();
        }
        finally {
            close();
        }
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
