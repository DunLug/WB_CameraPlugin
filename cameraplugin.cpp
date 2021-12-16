#include "gazebo/sensors/DepthCameraSensor.hh"
#include "cameraplugin.h"
#include <opencv4/opencv2/core.hpp>
#include <opencv4/opencv2/core/hal/interface.h>
#include <opencv4/opencv2/imgcodecs.hpp>
#include <opencv4/opencv2/imgproc.hpp>
#include <opencv4/opencv2/opencv.hpp>
#include <boost/filesystem.hpp>


using namespace gazebo;
GZ_REGISTER_SENSOR_PLUGIN(WB_CameraPlugin)

/////////////////////////////////////////////////
WB_CameraPlugin::WB_CameraPlugin()
    : SensorPlugin(), width(0), height(0), depth(0), streamer(NetworkStreamer::get_instance())
{
    boost::filesystem::path dir(outdir);
    if (! boost::filesystem::create_directory(dir))
    {
        gzerr() << "Error while creating " << outdir << ", the camera stream will not work properly" << std::endl;
    }
    cpt = 0;
}

/////////////////////////////////////////////////
WB_CameraPlugin::~WB_CameraPlugin()
{
  this->newFrameConnection.reset();
  this->parentSensor.reset();
  this->camera.reset();
    delete &streamer;
}

/////////////////////////////////////////////////
void WB_CameraPlugin::Load(sensors::SensorPtr _sensor, sdf::ElementPtr /*_sdf*/)
{
  if (!_sensor)
    gzerr << "Invalid sensor pointer.\n";

  this->parentSensor =
    std::dynamic_pointer_cast<sensors::CameraSensor>(_sensor);

  if (!this->parentSensor)
  {
    gzerr << "WB_CameraPlugin requires a CameraSensor.\n";
    if (std::dynamic_pointer_cast<sensors::DepthCameraSensor>(_sensor))
      gzmsg << "It is a depth camera sensor\n";
  }

  this->camera = this->parentSensor->Camera();

  if (!this->parentSensor)
  {
    gzerr << "WB_CameraPlugin not attached to a camera sensor\n";
    return;
  }

  this->width = this->camera->ImageWidth();
  this->height = this->camera->ImageHeight();
  this->depth = this->camera->ImageDepth();
  this->format = this->camera->ImageFormat();
  streamer.set_size(width, height, depth);
  streamer.run();

  this->newFrameConnection = this->camera->ConnectNewImageFrame(
      std::bind(&WB_CameraPlugin::OnNewFrame, this,
        std::placeholders::_1, std::placeholders::_2, std::placeholders::_3,
        std::placeholders::_4, std::placeholders::_5));

  this->parentSensor->SetActive(true);
}

/////////////////////////////////////////////////
void WB_CameraPlugin::OnNewFrame(const unsigned char * image,
                                 unsigned int width,
                                 unsigned int height,
                                 unsigned int depth,
                                 const std::string &format)
{

   // gzmsg << "Received frame" << std::endl;
 //    streamer.update_image(image, width, height, depth, format);
    char filename[100];
    sprintf(filename, "img%d.jpg", cpt);
    boost::filesystem::path path(outdir);
    path /= filename;

    int cvtype;
    if (depth == 1)
    {
        cvtype = CV_8UC1;
    }
    else if (depth == 3)
    {
        cvtype = CV_8UC3;
    }
    else
    {
        gzerr() << "Unknown depth " << depth << std::endl;
        cvtype = CV_8UC1;
    }
    cv::Mat cvimg(height, width, cvtype, const_cast<unsigned char*>(image));
    cv::imwrite(path.string(), cvimg);
    cpt ++;
}

