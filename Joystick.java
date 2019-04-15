package com.example.mikolaj.sterowanierobotem;

import android.content.Context;
import android.graphics.*;
import android.util.AttributeSet;
import android.view.*;


public class Joystick extends SurfaceView implements SurfaceHolder.Callback, View.OnTouchListener{

    private int center;
    private int bigRadius;
    private int smallRadius;
    private int x;
    private int y;
    private JoystickListener joystickCallback;
    private int color = Color.WHITE;


    public Joystick(Context context){
        super(context);
        this.getHolder().addCallback(this);
        setOnTouchListener(this);
        calculateSize();

        if(context instanceof JoystickListener){
            joystickCallback = (JoystickListener) context;
        }


    }

    public Joystick(Context context, AttributeSet a , int style){
        super(context, a, style);
        this.getHolder().addCallback(this);
        setOnTouchListener(this);
        calculateSize();

        if(context instanceof JoystickListener){
            joystickCallback = (JoystickListener) context;
        }

    }


    public Joystick(Context context, AttributeSet a){
        super(context, a);
        this.getHolder().addCallback(this);
        setOnTouchListener(this);
        calculateSize();

        if(context instanceof JoystickListener){
            joystickCallback = (JoystickListener) context;
        }

    }

    private void drawJoystick(){
        if(this.getHolder().getSurface().isValid())
        {
            Canvas canvas = this.getHolder().lockCanvas();

            canvas.drawColor(this.color);

            Paint color = new Paint();

            color.setARGB(100, 230, 230, 240);
            canvas.drawCircle(center, center, bigRadius, color);

            color.setARGB(150, 200, 200, 200);
            canvas.drawCircle(x, y, smallRadius, color);


            this.getHolder().unlockCanvasAndPost(canvas);

        }
    }

    @Override
    public void surfaceCreated(SurfaceHolder holder) {
        calculateSize();
        drawJoystick();
    }

    @Override
    public void surfaceChanged(SurfaceHolder holder, int format, int width, int height) {
        calculateSize();
        drawJoystick();
    }

    @Override
    public void surfaceDestroyed(SurfaceHolder holder) {

    }

    public void calculateSize() {
        this.center = Math.min(getWidth(), getHeight())/2;
        this.bigRadius = (int)(this.center/1.5);
        this.smallRadius = this.center/5;
        this.x = this.y = this.center;
    }


    public boolean onTouch(View v,MotionEvent e){
        if(v.equals(this)){
            if(e.getAction() != e.ACTION_UP){
                int tmpX = Math.abs((int)e.getX() - center);
                int tmpY = Math.abs((int)e.getY() - center);
                double d = Math.sqrt(tmpX*tmpX + tmpY*tmpY);
                if(d < this.bigRadius) {
                    this.x = (int) e.getX();
                    this.y = (int) e.getY();
                    drawJoystick();

                }
                else{
                    this.x = (int)(this.center + (e.getX() - center)*bigRadius/d);
                    this.y = (int)(this.center + (e.getY() - center)*bigRadius/d);
                    drawJoystick();

                }

            }
            else{
                this.x = this.y = this.center;
                drawJoystick();

            }
            joystickCallback.onJoyStickMoved(getXInRange(100), getYInRange(100));


        }
        return true;
    }

    public void setColor(int color) {
        this.color = color;
        drawJoystick();
    }

    public interface JoystickListener{
        void onJoyStickMoved(int x, int y);
    }

    public int getXInRange(int range){
        return (int)range*(x - center)/bigRadius;
    }
    public int getYInRange(int range){
        return (int)range*(y - center)/bigRadius;
    }
}
