/* \author Aaron Brown */
// Create simple 3d highway enviroment using PCL
// for exploring self-driving car sensors

#include "sensors/lidar.h"
#include "render/render.h"
#include "processPointClouds.h"
// using templates for processPointClouds so also include .cpp to help linker
#include "processPointClouds.cpp"

std::vector<Car> initHighway(bool renderScene, pcl::visualization::PCLVisualizer::Ptr& viewer)
{

    Car egoCar( Vect3(0,0,0), Vect3(4,2,2), Color(0,1,0), "egoCar");
    Car car1( Vect3(15,0,0), Vect3(4,2,2), Color(0,0,1), "car1");
    Car car2( Vect3(8,-4,0), Vect3(4,2,2), Color(0,0,1), "car2");	
    Car car3( Vect3(-12,4,0), Vect3(4,2,2), Color(0,0,1), "car3");
  
    std::vector<Car> cars;
    cars.push_back(egoCar);
    cars.push_back(car1);
    cars.push_back(car2);
    cars.push_back(car3);

    if(renderScene)
    {
        renderHighway(viewer);
        egoCar.render(viewer);
        car1.render(viewer);
        car2.render(viewer);
        car3.render(viewer);
    }

    return cars;
}


void simpleHighway(pcl::visualization::PCLVisualizer::Ptr& viewer)
{
    // ----------------------------------------------------
    // -----Open 3D viewer and display simple highway -----
    // ----------------------------------------------------
    
    // RENDER OPTIONS
    // So here if we turn this to false then it will take away the cars on the scene and only show our car
    bool renderScene = false;
    std::vector<Car> cars = initHighway(renderScene, viewer);
    
    // TODO:: Create lidar sensor 
    Lidar* lidar = new Lidar(cars,0); 
    bool render_obst =false;
    bool render_plane =false;
    bool render_clusters =true;
    bool render_box =true;
    pcl::PointCloud<pcl::PointXYZ>::Ptr inputCloud= lidar->scan();
    // renderRays will basically render the rays on top of the car and show the lidar unit and we can also increase the amount of laser 
    // points or lasers in the lidar. One can increase the lidars and get a better resolution or more points
    /* This function renders the lidar rays or laser rays in the viewer */
    //renderRays(viewer,lidar->position, inputCloud);
    /* Rendering the point cloud*/
    //Color colors = Color(255,102,0);
    renderPointCloud(viewer, inputCloud,"inputCloud");

    // Here starts the "segmentation" part of the Lesson!
    // used lidar and created point clouds above and visualized them!

    // TODO:: Create point cloud processor
    ProcessPointClouds<pcl::PointXYZ> ProcessorpointCloud;
    // The segmentation algorithm fits a plan to the points and uses the distance tolerance to decide which points belong
    // to that plane. A large tolerance includes more points in the plane
   
    std::pair<pcl::PointCloud<pcl::PointXYZ>::Ptr,pcl::PointCloud<pcl::PointXYZ>::Ptr> segmentCloud = ProcessorpointCloud.SegmentPlane(inputCloud, 100, 0.2);
     if(render_obst)
    {
        
        renderPointCloud(viewer,segmentCloud.first,"obstCloud",Color(1,0,0));
    }
    if(render_plane)
    {
         
        renderPointCloud(viewer,segmentCloud.second,"obstCloud",Color(1,0,0));
    }
    std::vector<pcl::PointCloud<pcl::PointXYZ>::Ptr> cloudClusters = ProcessorpointCloud.Clustering(segmentCloud.first, 1.0, 3, 30);
    int clusterId = 0;
    std::vector<Color> colors = {Color(1,0,0), Color(0,1,0), Color(0,1,0)};

    for(pcl::PointCloud<pcl::PointXYZ>::Ptr cluster : cloudClusters)
    {
        if(render_clusters)
        {
            std::cout << "cluster size ";
            ProcessorpointCloud.numPoints(cluster);
            renderPointCloud(viewer,cluster,"obstCloud"+std::to_string(clusterId),colors[clusterId]);
        }
        if(render_box)
        {
            Box box = ProcessorpointCloud.BoundingBox(cluster);
            renderBox(viewer,box,clusterId);
        }
      ++clusterId;
    }
    // renderPointCloud(viewer,segmentCloud.first,"obstCloud",Color(1,0,0));
    // renderPointCloud(viewer,segmentCloud.second,"planeCloud",Color(0,1,0));

}


//setAngle: SWITCH CAMERA ANGLE {XY, TopDown, Side, FPS}
void initCamera(CameraAngle setAngle, pcl::visualization::PCLVisualizer::Ptr& viewer)
{

    viewer->setBackgroundColor (0, 0, 0);
    
    // set camera position and angle
    viewer->initCameraParameters();
    // distance away in meters
    int distance = 16;
    
    switch(setAngle)
    {
        case XY : viewer->setCameraPosition(-distance, -distance, distance, 1, 1, 0); break;
        case TopDown : viewer->setCameraPosition(0, 0, distance, 1, 0, 1); break;
        case Side : viewer->setCameraPosition(0, -distance, 0, 0, 0, 1); break;
        case FPS : viewer->setCameraPosition(-10, 0, 0, 0, 0, 1);
    }

    if(setAngle!=FPS)
        viewer->addCoordinateSystem (1.0);
}
void cityBlock(pcl::visualization::PCLVisualizer::Ptr& viewer)
{
  // ----------------------------------------------------
  // -----Open 3D viewer and display City Block     -----
  // ----------------------------------------------------
  // dealing with point clouds with intensity values
  // If use pointer then we initialize in the heap such as below
  ProcessPointClouds<pcl::PointXYZI>* pointProcessorI = new ProcessPointClouds<pcl::PointXYZI>();
  pcl::PointCloud<pcl::PointXYZI>::Ptr inputCloud = pointProcessorI->loadPcd("../src/sensors/data/pcd/data_1/0000000000.pcd");
  // Experiment with the ? values and find what works best
  pcl::PointCloud<pcl::PointXYZI>::Ptr filterCloud;
  filterCloud = pointProcessorI->FilterCloud(inputCloud, ? , Eigen::Vector4f (?, ?, ?, 1), Eigen::Vector4f ( ?, ?, ?, 1));
  renderPointCloud(viewer,filterCloud,"filterCloud");
  //renderPointCloud(viewer,inputCloud,"inputCloud");
}

int main (int argc, char** argv)
{
    int simplehighway= false;
    int cityviewer = true;
    // with the following function calls it will render the point cloud on the opengl window and we will see all the pcd points
    // We can also change the RGB colors of the points to whatever we want by changing the Color Struct
    // if we put renderScene to false in simplehighway() then it will ignore the cars on the road and not render them
    std::cout << "starting enviroment" << std::endl;

    pcl::visualization::PCLVisualizer::Ptr viewer (new pcl::visualization::PCLVisualizer ("3D Viewer"));
    CameraAngle setAngle = XY;
    
    //pcl::PointCloud<pcl::PointXYZ>::Ptr  temp;
    initCamera(setAngle, viewer);
    if(simplehighway)
     simpleHighway(viewer);
    else if(cityviewer)
        cityBlock(viewer);
    while (!viewer->wasStopped ())
    {
        viewer->spinOnce ();

    } 
}