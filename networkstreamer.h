#ifndef NETWORKSTREAMER_H
#define NETWORKSTREAMER_H
#include <string>
#include <pthread.h>
#include <gst/gst.h>

class NetworkStreamer
{

private:
    GMainLoop *loop;
    GstElement* appsrc, *pipeline, *decode, *colorspace, *jpegenc, *multipartmux, *tcpsink;
    int width, height, depth;
    unsigned char* data;
    pthread_t thread;

    int argc;
    char** argv;
    static NetworkStreamer* _instance;

    NetworkStreamer();

public:
    static NetworkStreamer& get_instance();
    void set_size(int width, int height, int depth);
    void run(int argc, char** argv);
    void run();
    void run_thread();
    void update_image(const unsigned char * image,
                      unsigned int width,
                      unsigned int height,
                      unsigned int depth,
                      const std::string & format);
    void send_image(GstElement* appsrc);
    ~NetworkStreamer();
};

#endif // NETWORKSTREAMER_H
