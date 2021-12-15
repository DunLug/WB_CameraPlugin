#include "gazebo/sensors/DepthCameraSensor.hh"
#include "cameraplugin.h"

using namespace gazebo;
GZ_REGISTER_SENSOR_PLUGIN(WB_CameraPlugin)

/////////////////////////////////////////////////
WB_CameraPlugin::WB_CameraPlugin()
    : SensorPlugin(), width(0), height(0), depth(0), streamer(NetworkStreamer::get_instance())
{
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
    streamer.update_image(image, width, height, depth, format);
}

