interface Node
{

    void display();
    Road get_road(int i);

    float getX();
    float getY();

    void addRoad(Road R);
}

//Mainly for debugging, a portal to hell, cars drive at full speed into this hole and disappear
class Hellhole implements Node
{
    float x;
    float y;

    //For debugging purposes, there is only one neighbout
    Road OneRoad;

    Hellhole(float _x, float _y)
    {
        x=_x;
        y=_y;
        OneRoad=null;
    }

    void addRoad(Road R)
    {
        OneRoad=R;
    }


    void display()
    {

        stroke(0);
        //A hole directly into hell
        fill(255,50,0);
        ellipse(x*zoom,y*zoom,40*zoom,40*zoom);
        noStroke();
        fill(255,150,50);
        ellipse(x*zoom,y*zoom,30*zoom,30*zoom);
    }

    Road get_road(int i)
    {
        return OneRoad;
    }

    float getX(){return x;}
    float getY(){return y;}

}
