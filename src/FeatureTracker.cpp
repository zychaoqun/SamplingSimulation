#include "FeatureTracker.h"
#include "GL/gl.h"
#include "World.h"
#include <stdio.h>

//#define RAND(a,b)    a+((double)(b-a))*((double)(rand()%1000))/1000
//#define RAND(a,b)    (a)+((b)-(a))*((double)(rand()%1000))/1000
#define MIN_FEATURES    50
#define MAX(a,b)    (a>b)?a:b



FeatureTracker::FeatureTracker(TooN::Vector<3,double> startPos):Visualizer()
{
    srand(time(NULL));



    SetPose(startPos);

//    for(int i=0; i< 1000; i++)
//    {
//        Feature f;
//        f.pos[0] = RAND(-5,5);
//        f.pos[1] = RAND(-5,5);

//        f.pos[2] = 0.01 + World::Instance()->GetHeight(f.pos[0], f.pos[1]);

//        features.push_back(f);
//    }

//    vector<Entity*> ev;
//    for(int i=0;i<10; i++)
//    {
//        double ww = 5;
//        double wl = 5;
//        Vector<3> p = makeVector(pow(-1,i)*i,i,4);//*/makeVector(RAND(-ww/2,ww/2), RAND(-wl/2,wl/2), 4);
//        Entity * en = new Entity();
//        en->pos = p;
//        if(i==0)
//            en->start = true;
//        else
//            en->start = false;

//        ev.push_back(en);
//    }

//    Vector<3> p = makeVector(101,1,4);//*/makeVector(RAND(-ww/2,ww/2), RAND(-wl/2,wl/2), 4);
//    Entity * en1 = new Entity();
//    en1->pos = p;
//    en1->start = false;
//    ev.push_back(en1);

//    p = makeVector(100,1,4);//*/makeVector(RAND(-ww/2,ww/2), RAND(-wl/2,wl/2), 4);
//    Entity * en2 = new Entity();
//    en2->pos = p;
//    en2->start = false;
//    ev.push_back(en2);

   // shortestPath = tsp.GetShortestPath(ev);

}

void FeatureTracker::DrawFrustum(Vector<3, double> camp, double size)
{
    glColor3f(1,0,0);
    glPointSize(10);
    glBegin(GL_POINTS);
    glVertex3f(camp[0], camp[1], camp[2]);
    glEnd();

    double dxy = size * tan(FOV/2);

    glColor3f(0,0,0);
    glLineWidth(1);
    glBegin(GL_LINES);
    glVertex3f(camp[0], camp[1], camp[2]);
    glVertex3f(camp[0], camp[1], camp[2]-0.5);

    glVertex3f(camp[0], camp[1], camp[2]);
    glVertex3f(camp[0]+dxy, camp[1]+dxy, camp[2]-size);

    glVertex3f(camp[0], camp[1], camp[2]);
    glVertex3f(camp[0]+dxy, camp[1]-dxy, camp[2]-size);

    glVertex3f(camp[0], camp[1], camp[2]);
    glVertex3f(camp[0]-dxy, camp[1]+dxy, camp[2]-size);

    glVertex3f(camp[0], camp[1], camp[2]);
    glVertex3f(camp[0]-dxy, camp[1]-dxy, camp[2]-size);
    glEnd();


}

void FeatureTracker::glDraw()
{
    DrawFrustum(pose, pose[2]);

    //std::vector<Feature> fs = MatchedFeatures(pose);
    std::vector<Feature> fs = features;

    glColor3f(1,0,0);
    glPointSize(5);

    if(false)
    {
        for(int i=0; i<fs.size(); i++)
        {
            glPointSize(5*fs[i].size);
            glBegin(GL_POINTS);
            glVertex3f(fs[i].pos[0], fs[i].pos[1], fs[i].pos[2]+0.01);
            glEnd();
        }
    }
    // draw matched features
//    glColor3f(1,0,1);
//    for(int i=0; i<matchedFeatures.size(); i++)
//    {
//        glPointSize(10*matchedFeatures[i].size);
//        glBegin(GL_POINTS);
//        glVertex3f(matchedFeatures[i].pos[0], matchedFeatures[i].pos[1], matchedFeatures[i].pos[2]+0.01);
//        glEnd();
//    }


    // draw path
    for(int i=0; i<pathWPs.size(); i++)
    {
       // DrawFrustum(pathWPs[i], pathWPs[i][2]);
        glColor3f(1,1,1);
        glPointSize(10);
        glBegin(GL_POINTS);
        glVertex3f(pathWPs[i][0], pathWPs[i][1], pathWPs[i][2]);
        glEnd();


        if(i+1<pathWPs.size())
        {
            glColor3f(0,0,0);
            glLineWidth(3);
            glBegin(GL_LINES);
            glVertex3f(pathWPs[i][0],pathWPs[i][1],pathWPs[i][2]);
            glVertex3f(pathWPs[i+1][0],pathWPs[i+1][1],pathWPs[i+1][2]);
            glEnd();
        }
    }


    for(int i=0; i<shortestPath.size(); i++)
    {
       // DrawFrustum(pathWPs[i], pathWPs[i][2]);
        glColor3f(0,1,0);
        glPointSize(10);
        glBegin(GL_POINTS);
        glVertex3f(shortestPath[i]->pos[0], shortestPath[i]->pos[1], shortestPath[i]->pos[2]);
        glEnd();


        if(i+1<shortestPath.size())
        {
            glColor3f(0.1*((double)i),0,0);
            glLineWidth(3);
            glBegin(GL_LINES);
            glVertex3f(shortestPath[i]->pos[0],shortestPath[i]->pos[1],shortestPath[i]->pos[2]);
            glVertex3f(shortestPath[i+1]->pos[0],shortestPath[i+1]->pos[1],shortestPath[i+1]->pos[2]);
            glEnd();
        }
    }

    if(bDrawEntropy)
    {
        //Draw entropy field
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        for(int i=0; i< entropies.size(); i++)
        {
            glColor4f(0,0,1,entropies[i]/mesh.maxEntropy);
            Vector<3, double > p1 = makeVector(nextPath[i][0]-footprint_length/4, nextPath[i][1]+footprint_length/4, nextPath[i][2]);
            Vector<3, double > p2 = makeVector(nextPath[i][0]+footprint_length/4, nextPath[i][1]-footprint_length/4, nextPath[i][2]);

            glBegin(GL_POLYGON);
            glVertex3f(p1[0], p1[1], p1[2]);
            glVertex3f(p2[0], p1[1], p2[2]);
            glVertex3f(p2[0], p2[1], p2[2]);
            glVertex3f(p1[0], p2[1], p2[2]);
            glEnd();
        }
    }
}

void FeatureTracker::ClearHistory()
{
    bSensing = false;
    features.clear();
    matchedFeatures.clear();
    mesh.CleanMesh();
    nextPath.clear();
    entropies.clear();
}

void FeatureTracker::MoveSensor(TooN::Vector<3, double> dPos)
{
    SetPose(pose + dPos);
}

void FeatureTracker::MoveSensorTo(Vector<3,double> pos)
{
    SetPose(pos);
}

void FeatureTracker::SetPose(Vector<3, double> newpose)
{
    pose = newpose;

    if(bSensing)
    {
        vector<Feature> fs = TrackFeatures(pose);

        double rndMin = MIN_FEATURES * RAND(0.7,1.3);
        if(fs.size() < rndMin)
        {
            GenerateFeatures(rndMin - fs.size(), pose);
        }

        UpdateMatchedFeatures();
    }

}
double FeatureTracker::GetSamplingLevels(int &level)
{
    //return ((double)level) * 0.5;
    //double wl = World::Instance()->GetLength();
    if(level<1)
        level = 1;


    double ww = World::Instance()->GetWidth();

    double max_l = ww;//MAX(wl,ww);

    double dv=1;
    for(int i=0;i<level-1; i++)
    {
        dv*=2;
    }
    double result = (0.5*max_l/dv)*tan(FOV/2);
    if(result < 0.1/*World::Instance()->GetMaxHeight()*/)
    {
        if(level <=1)
        {
            printf("Too high world!!");
            return 0;
        }
        else
        {
            printf("Too no lower level!! At level: %d", level);
            return GetSamplingLevels(--level);
        }
    }
    else
        return result;
}

void FeatureTracker::GenerateFeatures(int size, Vector<3, double> viewpoint)
{

    for(int i=0; i< size; i++)
    {
        Feature f;
        f.view = viewpoint;

        double x1 = viewpoint[0]-viewpoint[2]*tan(FOV/2);
        double x2 = viewpoint[0]+viewpoint[2]*tan(FOV/2);

        double y1 = viewpoint[1]-viewpoint[2]*tan(FOV/2);
        double y2 = viewpoint[1]+viewpoint[2]*tan(FOV/2);


        f.pos[0] = RAND(x1, x2);
        f.pos[1] = RAND(y1, y2);

        f.pos[2] = World::Instance()->GetHeight(f.pos[0], f.pos[1])+RAND(0,0.1)*viewpoint[2];
        f.size = (viewpoint[2] - f.pos[2])*tan(FOV/2)*RAND(0.5, 0.8);

        //printf("%f %f %f %f\n", f.pos[0], f.pos[1], f.pos[2], f.size);

        mesh.AddVertex(f.pos);

        features.push_back(f);
    }
}

bool FeatureTracker::InsideFOV(Feature f, Vector<3, double> pos)
{
    return fabs(f.pos[0]-pos[0]) < fabs(pos[2]*tan(FOV/2)) && fabs(f.pos[1]-pos[1]) < fabs(pos[2]*tan(FOV/2));
}

void FeatureTracker::UpdateMatchedFeatures()
{
    matchedFeatures = TrackFeatures(pose);
}

std::vector<Feature> FeatureTracker::TrackFeatures(Vector<3, double> viewpoint)
{
    vector<Feature> result;

    for(int i=0; i< features.size(); i++)
    {
        if(InsideFOV(features[i], viewpoint))
        {
            double dist = sqrt((viewpoint-features[i].view)*(viewpoint-features[i].view));
            if(dist < features[i].size)
            {
                result.push_back(features[i]);
            }
        }
    }

    return result;
}
double FeatureTracker::GetMaxHeightInFootprint(double x, double y, double footprint_l)
{
    double mxheight=0;
    for(double i=x-footprint_l/2;i<x+footprint_l/2; i+=0.2)
        for(double j=y-footprint_l/2;j<y+footprint_l/2; j+=0.2)
        {
            double h = World::Instance()->GetHeight(i,j);
            mxheight = (mxheight < h)?h:mxheight;
        }
    return mxheight;
}

void FeatureTracker::ExecuteCoveragePlan(double w_w, double w_l, double flying_height, double step_l)
{
    bSensing = false;

    double footPrint_l = 2*flying_height*tan(FOV/2);
    footprint_length = footPrint_l;

    Vector<3, double> startPoint = makeVector(-w_w/2, -w_l/2, flying_height);
    SetPose(startPoint+ makeVector(footPrint_l/2,footPrint_l/2,0));

    pathWPs.clear();
    //printf("start point: %f %f %f", pose[0], pose[1], pose[2]);

    bool alternateFlag = false;
    for(int i=0; i< ceil(w_w/footPrint_l); i++)
    {
        double x = startPoint[0] + footPrint_l/2 + ((double)i)*footPrint_l;

        alternateFlag = !alternateFlag;

        for(int j=(alternateFlag)?0:(ceil(w_l/footPrint_l)-1); (alternateFlag)?(j<ceil(w_l/footPrint_l)):(j>=0);(alternateFlag)?(j++):(j--))
        {
            double y = startPoint[1] + footPrint_l/2 + ((double)j)*footPrint_l;

            double scanHeight = flying_height+GetMaxHeightInFootprint(x,y,footPrint_l);//World::Instance()->GetHeight(x,y);
            if(!pathWPs.empty())
            {
                Vector<3,double>  prevWP = (pathWPs.back());
                if(fabs(scanHeight-prevWP[2])<0.001)
                {
                    pathWPs.push_back(makeVector(x,y, scanHeight));
                }
                else if(prevWP[2] < scanHeight)
                {
                    pathWPs.push_back(makeVector(prevWP[0],prevWP[1], scanHeight));
                    pathWPs.push_back(makeVector(x,y, scanHeight));
                }
                else
                {
                    pathWPs.push_back(makeVector(x,y, prevWP[2]));
                    pathWPs.push_back(makeVector(x,y, scanHeight));
                }
            }
            else
            {
                pathWPs.push_back(makeVector(x,y, scanHeight));
            }

            nextPath.push_back(makeVector(x+footPrint_l/4,y+footPrint_l/4, flying_height));
            nextPath.push_back(makeVector(x+footPrint_l/4,y-footPrint_l/4, flying_height));
            nextPath.push_back(makeVector(x-footPrint_l/4,y+footPrint_l/4, flying_height));
            nextPath.push_back(makeVector(x-footPrint_l/4,y-footPrint_l/4, flying_height));
        }

    }

}

void FeatureTracker::GoToNextWP(double step_l)
{
    bSensing = true;
    for(int i=0; i+1 < pathWPs.size(); i++)
    {
        double wpd = sqrt((pathWPs[i+1]-pathWPs[i])*(pathWPs[i+1]-pathWPs[i]));
        for(int j =0; j< floor(wpd/step_l); j++)
        {
            //SetPose(/*pose + (step_l/wpd)*(pathWPs[i+1]-pathWPs[i])*/pathWPs[i]);
            MoveSensor((step_l/wpd)*(pathWPs[i+1]-pathWPs[i]));
        }

        MoveSensorTo(pathWPs[i+1]);

        if(pathWPs.size() == 2)
        {
            OnPlanExecuted();
            pathWPs.clear();
        }
        else
        {
            pathWPs.erase(pathWPs.begin());
        }

        break;
    }
}

void FeatureTracker::OnPlanExecuted()
{
    double footPrint_l = footprint_length/2;
    for(int i=0; i< nextPath.size(); i++)
    {
        Vector<2, double > tl = makeVector(nextPath[i][0]-footPrint_l/2, nextPath[i][1]+footPrint_l/2);
        Vector<2, double > rb = makeVector(nextPath[i][0]+footPrint_l/2, nextPath[i][1]-footPrint_l/2);
        entropies.push_back(mesh.GetEntropy(tl,rb));
    }
}
