//Any motor Vehicle, this is not a simulation, so it does not care if it is a car or, large truck or bus.


color carColors[] = {color(255,100,100),color(100,255,100),color(100,100,255),color(255)};



class Vehicle
{
    float length;//length, in meters

    ArrayList<keyframe> Keys;

    //For display only
    int type;//Processing  does not support enums, 0: car
    color myColor;

    //We are storing key-frames of these cars, with their position, velocity and acceleration constant, we will have to interpolate between these key-frames
    class keyframe
    {
        float time;//The only required element, when is this keyframe
        Road myRoad;//Where am I, and where am I going
        int lane;//Lane counting from 0 = outer lane)
        boolean goingForward;//Traveling towards the myRoad.second

        float pos;//Position from start of road
        float speed;
        float acceleration;


        keyframe ( JSONObject O, ArrayList<Road> Roads, keyframe prev)
        {

            time = O.getFloat("time");
            //If this is the first time this happens, we HAVE to load everything (or default to 0)

            if (prev==null)
            {
                pos         =O.getFloat("pos",0);
                speed       =O.getFloat("speed",0);
                acceleration=O.getFloat("acc",0);

                myRoad=Roads.get(O.getInt("road",0));

                lane=O.getInt("lane",0);

                goingForward=O.getBoolean("direction",true);

            }
            else
            {

                if (prev.time>=time)
                {
                    throw new IllegalArgumentException("Keyframe at time "+Float.toString(prev.time)+" followed by one at "+Float.toString(time));
                }

                float deltat= time-prev.time;

                pos         =O.getFloat("pos",prev.pos+prev.speed*deltat+0.5*deltat*deltat*prev.acceleration);
                speed       =O.getFloat("speed",prev.speed+prev.acceleration*deltat);
                acceleration=O.getFloat("acc",prev.acceleration);

                int R_id = O.getInt("road",-1);

                if (R_id == -1)
                    myRoad=prev.myRoad;
                else
                    myRoad=Roads.get(R_id);
                lane=O.getInt("lane",prev.lane);
                goingForward=O.getBoolean("direction",true);
            }
        }

    }

    Vehicle(JSONObject O, ArrayList<Road> Roads)
    {
        length=O.getFloat("length",3.0);


        String type_str= O.getString("type","car");
        if (type_str.equals("car"))
        {
            myColor=carColors[int(random(carColors.length))];
        }


        Keys=new ArrayList<keyframe>();
        JSONArray Key_list= O.getJSONArray("keyframes");

        for (int i = 0; i < Key_list.size(); ++i)
            Keys.add(new keyframe(Key_list.getJSONObject(i),Roads, i==0 ? null : Keys.get(i-1)));


    }

    void display(float current_time)
    {
        if (Keys.size()>=2)
        {
            if (current_time>=start_time() && current_time<end_time())
            {
                //Get the key frame we are betwixt, we will only get here if there are two
                keyframe This=null;
                keyframe Next=null;

                for (int i = 0; i+1 < Keys.size(); ++i)
                {
                    if (current_time>=Keys.get(i).time)
                    {
                        This = Keys.get(i);
                        Next = Keys.get(i+1);
                    }
                }

                if (This.myRoad == Next.myRoad)//Not at an intersection
                {
                    float deltaT = current_time-This.time;

                    This.myRoad.applyMatrix(This.lane,This.goingForward,This.pos+This.speed*deltaT+0.5*This.acceleration*deltaT*deltaT);
                    fill(myColor);
                    stroke(0);
                    //The pos is at the BACK of the car
                    rect(This.goingForward?0 :-length*zoom,-zoom,length*zoom,2*zoom);
                    popMatrix();
                }

            }
            //CORRECTION, cars despawn after last keyframe
 /*           if (current_time>=end_time())
            {


                keyframe This=Keys.get(Keys.size()-1);

                This.myRoad.applyMatrix(This.lane,This.goingForward,This.pos);
                fill(myColor);
                stroke(0);
                rect(0,-zoom,length*zoom,2*zoom);
                popMatrix();

            }
            */
        }
    }


    float start_time()
    {
        if (Keys.size()>0)
        {
            return Keys.get(0).time;
        }
        else
            return 0;
    }

    float end_time()
    {
        if (Keys.size()>0)
        {
            return Keys.get( Keys.size()-1).time;
        }
        else
            return 0;

    }

}
