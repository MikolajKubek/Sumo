package com.example.mikolaj.sterowanierobotem;

import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;
import android.bluetooth.BluetoothSocket;
import android.graphics.Color;
import android.util.Log;

import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.util.Set;

public class BluetoothMessages {


    private OutputStream outputStream;
    private InputStream inStream;
    private Joystick joystick;

    public BluetoothMessages(String address, Joystick joystick){

        this.joystick = joystick;

        BluetoothAdapter bluetoothAdapter = BluetoothAdapter.getDefaultAdapter();
        if(bluetoothAdapter != null){
            if(bluetoothAdapter.isEnabled()){
                Set<BluetoothDevice> bondedDevices = bluetoothAdapter.getBondedDevices();

                if(bondedDevices.size() > 0){
                    BluetoothDevice myDevice = null;
                    for(BluetoothDevice device : bondedDevices) {
                        if (device.getAddress().contains(address)) {
                            myDevice = device;
                            System.out.println(device.getName());
                        }
                    }

                    if(myDevice != null) {
                        try {
                            BluetoothSocket socket = myDevice.createRfcommSocketToServiceRecord(myDevice.getUuids()[0].getUuid());
                            socket.connect();
                            outputStream = socket.getOutputStream();
                            inStream = socket.getInputStream();
                        }
                        catch (IOException e){Log.e("error", "IOException creating socket");}
                    }
                }
            }

        }
        else{
            Log.e("error", "No bluetooth adapter");
        }
    }


    public void write(String s) throws IOException {
        if(outputStream != null) {
            byte[] bytes = s.getBytes();
            for(byte b : bytes){
                outputStream.write(b);
            }
        }
    }

    public void read() {
        if(inStream != null) {
            new Thread() {
                @Override
                public synchronized void run() {
                    boolean interrupted = false;
                    while(!interrupted) {
                        try {
                            int tmp = inStream.read();
                            if(tmp == '!'){
                                joystick.setColor(Color.RED);
                            }
                            else {
                                joystick.setColor(Color.WHITE);
                            }
                        } catch (IOException e) {
                            interrupted = true;
                            System.out.println(interrupted);
                        }
                    }
                }
            }.start();
        }
    }

}
