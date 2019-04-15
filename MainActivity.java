package com.example.mikolaj.sterowanierobotem;

import android.graphics.Color;
import android.graphics.Point;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.util.Log;
import android.view.View;
import android.widget.Button;

import java.io.IOException;

public class MainActivity extends AppCompatActivity implements Joystick.JoystickListener {

    private BluetoothMessages bluetoothMessages;
    private Joystick joystick;
    private Button button;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        joystick = this.findViewById(R.id.joystick);

        bluetoothMessages = new BluetoothMessages("98:D3:31:FC:AB:CD", joystick);

        bluetoothMessages.read();

        button = this.findViewById(R.id.SumoButton);
        button.setText("Tryb Sumo");
        button.setBackgroundColor(Color.BLUE);

        button.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                try {
                    bluetoothMessages.write("*");
                    button.setBackgroundColor(Color.GREEN);
                } catch (IOException e) {
                    Log.e("error", "Can't turn on sumo mode");
                }
            }
        });

    }

    @Override
    public void onJoyStickMoved(int x, int y) {

        try {
            bluetoothMessages.write(x + "|" + y + ";");
            System.out.println(x + "|" + y + ";");
            button.setBackgroundColor(Color.BLUE);
        } catch (IOException e) {
            Log.e("error", "Can't send data");
        }
    }
}
