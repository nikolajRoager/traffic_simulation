/*Processing is not very strict about good OOP principles, prefering simple to write code over strict adherence to principles.
I do have access to ALL OF JAVA behind the scenes, but I will generally stick with Processings "bad" design, the goal right here is just to get something on the screen fast, everything serious happens in C++
*/




float cam_x=0;//Center in meters
float cam_y=0;

int zoom_lvl=0;
float zoom=1;

//Main lists, we can use to easilly identify the nodes
ArrayList<Node> Nodes;
ArrayList<Road> Roads;



//List of animated vehicles, i.e. the thing we are here to show
ArrayList<Vehicle> Vehicles;

//Previous mouse X and Y
int pmouseX,pmouseY;

JSONObject json;

//Time (and all units) are measured in SI units, i.e. seconds
float current_time=0;
float time_max=0;
float time_min=0;

boolean p_play_press=false;//Was the play button already held down
boolean play=false;//Are we currently playing

long p_millis;//Previous milli second

void setup()
{
    p_millis=0;
    size(1400,800,P2D);
    //Load city from city file, it is going to be run from the folder where they live
    json = loadJSONObject("city.json");


    Nodes = new ArrayList<Node>();
    Roads = new ArrayList<Road>();
    Vehicles= new ArrayList<Vehicle>();

    JSONArray Node_list = json.getJSONArray("nodes");

    for (int i = 0; i < Node_list.size(); ++i)
    {
        //We will have to decipher what type we are talking about here, so we can create the right thing
        JSONObject O = Node_list.getJSONObject(i);
        String type = O.getString("type");
        JSONArray pos = O.getJSONArray("pos");
        if (type.equals("Hellhole"))
            Nodes.add(new Hellhole(pos.getFloat(0),pos.getFloat(1)));
        else
            throw new IllegalArgumentException("Illegal node type "+type);
    }

    JSONArray Road_list = json.getJSONArray("auto_roads");

    for (int i = 0; i < Road_list.size(); ++i)
    {
        Roads.add(new Road(Road_list.getJSONObject(i),Nodes));
    }

    json = loadJSONObject("keyframes.json");

    JSONArray Vehicle_list = json.getJSONArray("vehicles");

    for (int i = 0; i < Vehicle_list.size(); ++i)
    {
        Vehicles.add(new Vehicle(Vehicle_list.getJSONObject(i),Roads));



    time_max=max(time_max,Vehicles.get(i).end_time());
    time_min= i==0 ? Vehicles.get(i).start_time(): min(time_min,Vehicles.get(i).start_time());
    current_time=0;
    }


    pmouseX=mouseX;
    pmouseY=mouseY;
}

void draw()
{

    long this_millis=millis();
    //Print city network:
    background(50);

    pushMatrix();
    translate(width/2,height/2);
    translate(-cam_x,-cam_y);

    for (Road R : Roads)
        R.display();

    for (Node N : Nodes)
        N.display();


    for (Vehicle V : Vehicles)
        V.display(current_time);


    if (mousePressed)
    {

        //If clicking the time-bar at the buttom
        if (mouseY>39*height/40)
        {
            current_time=(time_max-time_min)*(float(mouseX)/width)+time_min;
            play=false;//Stop playing when moving manually
        }
        else
        {
            cam_x+=pmouseX-mouseX;
            cam_y+=pmouseY-mouseY;
        }
    }

    popMatrix();

    //HUD
    //Mark center of screen
    line(0,height/2,width,height/2);
    line(width/2,0,width/2,height);

    //Play bar
    stroke(0);
    fill(0);
    rect(width*(current_time-time_min)/(time_max-time_min),39*height/40,width,height);
    fill(255,0,0);
    rect(0,39*height/40,width*(current_time-time_min)/(time_max-time_min),height);

    ellipse(width*(current_time-time_min)/(time_max-time_min),79*height/80,height/40,height/40);

    if (keyPressed && key==' ')
    {
        if (!p_play_press)
            play=!play;

        //If we are already at the end, start over
        if (play && current_time>=time_max-1e-8)
            current_time=time_min;

        p_play_press=true;
    }
    else
        p_play_press=false;


    //Move ahead for next frame
    if (play)
    {

        current_time += (this_millis-p_millis)*0.001;
        if (current_time >=time_max)
        {
            current_time =time_max;
            play=false;
        }
    }
    pmouseX=mouseX;
    pmouseY=mouseY;

    p_millis = this_millis;
}


void mouseWheel(MouseEvent event)
{


    zoom_lvl = constrain(zoom_lvl-event.getCount(),-10,10);
    float new_zoom = pow(1.2,zoom_lvl);

    //We want the center of the screen, to remain at the center of the screen
    cam_x=cam_x*(new_zoom/zoom);
    cam_y=cam_y*(new_zoom/zoom);


    zoom=pow(1.2,zoom_lvl);
}

