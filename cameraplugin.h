#ifndef CAMERAPLUGIN_H
#define CAMERAPLUGIN_H
#include "gazebo/common/Plugin.hh"
#include "gazebo/sensors/CameraSensor.hh"
#include "gazebo/rendering/Camera.hh"
#include "gazebo/util/system.hh"

namespace gazebo
{
class GZ_PLUGIN_VISIBLE WB_CameraPlugin : public SensorPlugin
{
public:
    WB_CameraPlugin();
    virtual ~WB_CameraPlugin();

    virtual void Load(sensors::SensorPtr _sensor, sdf::ElementPtr _sdf);

    virtual void OnNewFrame(const unsigned char *_image,
                            unsigned int _width, unsigned int _height,
                            unsigned int _depth, const std::string &_format);

protected:
    unsigned int width, height, depth;
    std::string format;

    sensors::CameraSensorPtr parentSensor;
    rendering::CameraPtr camera;

private:
    event::ConnectionPtr newFrameConnection;
};
}

#endif // CAMERAPLUGIN_H
