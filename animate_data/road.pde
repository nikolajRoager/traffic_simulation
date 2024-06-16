color RoadType_colors[] =
{
    color(80,50,50),//Byvej
    color(80,80,80),//Landevej
    color(30,30,30),//Motortrafikvej
    color(30,30,30)//Motorvej
};

float lane_width = 3;//Global variable, graphical width of road in meters

class Road
{


    //Start and end are interchangable
    Node start;
    Node end;

    int type;//Processing does not support enums

    boolean one_way;
    boolean no_overtake;//No overtaking on oposing lanes
    int lanes;

    //For graphics, the rotation of the road
    float theta;
    float dist;

    Road (JSONObject O,ArrayList<Node> Nodes)
    {


        String type_str= O.getString("type","landevej");
        int firstI = O.getInt("first");
        int secondI = O.getInt("second");
        lanes  = O.getInt("lanes",1);
        one_way= O.getBoolean("oneWay",false);
        if (type_str.equals("Byvej"))
            type=0;
        else if (type_str.equals("Landevej"))
            type=1;
        else if (type_str.equals("Motortrafikvej"))
            type=2;
        else if (type_str.equals("Motorvej"))
            type=3;
        else
            throw new IllegalArgumentException("Illegal road type "+type_str);

        start  =Nodes.get(firstI);
        end    =Nodes.get(secondI);

        start.addRoad(this);
        end.addRoad(this);

        dist = dist(start.getX(),start.getY(),end.getX(),end.getY());
        float cosTheta=(end.getX()-start.getX()) /dist;
        float sinTheta=(end.getY()-start.getY()) /dist;

        theta=atan2(sinTheta,cosTheta);

        no_overtake=O.getBoolean("noOvertake",false);
    }


    //Apply all the matrix rotation required to put a car at the start or end of this road, rotated correctly
    void applyMatrix(float lane/*Supports lane changes*/,boolean goForth, float pos)
    {
        pushMatrix();
        translate(start.getX()*zoom,start.getY()*zoom);
        rotate(theta);

        if (goForth)
            translate(pos*zoom,((0.5-lane)*lane_width+lanes*(one_way? lane_width*0.5 : lane_width))*zoom);
        else
            translate((dist-pos)*zoom,-((0.5-lane)*lane_width+lanes*(one_way? lane_width*0.5 : lane_width))*zoom);
    }

    void display()
    {
        stroke(100);

        fill(RoadType_colors[type]);



        pushMatrix();
        translate(start.getX()*zoom,start.getY()*zoom);
        rotate(theta);

        //Draw a road rectangle
        rect(0,-lanes*(one_way? lane_width*0.5 : lane_width)*zoom,dist*zoom,lanes*(one_way? lane_width : lane_width*2)*zoom);

        stroke(255);
        //Draw seperating lines betwixt lanes
        for (int i = 1; i<lanes*(one_way ? 1 : 2) ; ++i)
        {
            if (i==lanes)
            {

                stroke(255);
                line(0,(i*lane_width-lanes*(one_way? lane_width*0.5 : lane_width))*zoom,dist*zoom,(i*lane_width-lanes*(one_way? lane_width*0.5 : lane_width))*zoom);
            }
            else
            {
                stroke(150);

                for (float d = 0 ; d<dist; d+=10)
                {
                    line(d*zoom,(i*lane_width-lanes*(one_way? lane_width*0.5 : lane_width))*zoom,(d+lane_width)*zoom,(i*lane_width-lanes*(one_way? lane_width*0.5 : lane_width))*zoom);
                }
            }

            //line(0,(i*5)*zoom,dist*100*zoom,(i*5)*zoom);
        }
        popMatrix();



    }
}
