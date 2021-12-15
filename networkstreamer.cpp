#include "networkstreamer.h"
#include <boost/bind.hpp>
#include <iostream>

NetworkStreamer* NetworkStreamer::_instance = nullptr;

void* run_streamer(void* args)
{
    NetworkStreamer * streamer = (NetworkStreamer*) args;
    streamer->run_thread();
}

void need_image(GstElement* appsrc, guint, gpointer)
{
    std::cerr << "needimage" << std::endl;
    NetworkStreamer& streamer = NetworkStreamer::get_instance();
    streamer.send_image(appsrc);
}

static void cb_new_pad (GstElement *element, GstPad *pad, gpointer data)
{
  gchar *name;
  GstElement *other = (GstElement*)data;

  name = gst_pad_get_name (pad);
  g_print ("A new pad %s was created for %s\n", name, gst_element_get_name(element));
  g_free (name);

  g_print ("element %s will be linked to %s\n",
           gst_element_get_name(element),
           gst_element_get_name(other));
  gst_element_link(element, other);
}

NetworkStreamer::NetworkStreamer()
{
    data = nullptr;
}

void NetworkStreamer::run(int argc, char** argv)
{
    this->argc = argc;
    this->argv = argv;
    pthread_create (&thread, NULL, run_streamer, this);
}

void NetworkStreamer::run()
{
    run(0, nullptr);
}

void NetworkStreamer::run_thread()
{
    /* init GStreamer */
    gst_init (&argc, &argv);
    loop = g_main_loop_new (NULL, FALSE);

    /* setup pipeline */
    pipeline = gst_pipeline_new ("pipeline");
    appsrc = gst_element_factory_make ("appsrc", "source");
    decode = gst_element_factory_make ("decodebin", "dec");

    colorspace = gst_element_factory_make("videoconvert", "colorspace");
    jpegenc = gst_element_factory_make("jpegenc", "jpeg");
    multipartmux = gst_element_factory_make("multipartmux", "mux");
    tcpsink = gst_element_factory_make("tcpserversink", "sink");

    /* setup */
    g_object_set (G_OBJECT (appsrc), "caps",
          gst_caps_new_simple ("video/x-raw",
                       "format", G_TYPE_STRING, "RGB",
                       "width", G_TYPE_INT, width,
                       "height", G_TYPE_INT, height,
                       "framerate", GST_TYPE_FRACTION, 1, 1,
                       NULL), NULL);
    g_object_set (G_OBJECT (tcpsink), "host", "127.0.0.1", "port", 8090, NULL);
    gst_bin_add_many (GST_BIN (pipeline), appsrc, decode, colorspace, jpegenc, multipartmux, tcpsink, NULL);
    std::cerr << "add1" << std::endl;
    gst_element_link_many (appsrc, decode, NULL);
    std::cerr << "add1" << std::endl;
    gst_element_link_many(colorspace, jpegenc, multipartmux, tcpsink, NULL);

    std::cerr << "add1" << std::endl;
    g_signal_connect(decode, "pad-added", G_CALLBACK(cb_new_pad), colorspace);
    g_object_set (G_OBJECT (appsrc),
          "stream-type", 0,
          "format", GST_FORMAT_TIME, NULL);
    g_signal_connect (appsrc, "need-data", G_CALLBACK (need_image), NULL);

    /* play */
    gst_element_set_state (pipeline, GST_STATE_PLAYING);
    std::cerr << "running loop" << std::endl;
    g_main_loop_run (loop);
    std::cerr << "stopped" << std::endl;
}


NetworkStreamer& NetworkStreamer::get_instance()
{
    if (! _instance)
    {
        _instance = new NetworkStreamer();
    }
    return *_instance;
}

void NetworkStreamer::set_size(int width, int height, int depth)
{
    this->width = width;
    this->height = height;
    this->depth = depth;
    delete data;
    data = new unsigned char[width*height*depth];
}

void NetworkStreamer::update_image(const unsigned char * image,
                                    unsigned int width,
                                    unsigned int height,
                                    unsigned int depth,
                                    const std::string & format)
{
    if (width == this->width && height == this->height && depth == this->depth)
    {
        memcpy(data, image, width*height*depth);
    }
    else
    {
        std::cerr << "Error, image size is different" << std::endl;
    }
}

void NetworkStreamer::send_image(GstElement *appsrc)
{
    static GstClockTime timestamp = 0;
    GstBuffer *buffer;
    guint size;
    GstFlowReturn ret;
    GstMapInfo map;


    size = height*width*depth;
    buffer = gst_buffer_new_allocate (NULL, size, NULL);
    gst_buffer_map (buffer, &map, GST_MAP_WRITE);
    memcpy( (guchar *)map.data, data,  gst_buffer_get_size( buffer ) );

    GST_BUFFER_PTS (buffer) = timestamp;
    GST_BUFFER_DURATION (buffer) = gst_util_uint64_scale_int (1, GST_SECOND, 2);

    timestamp += GST_BUFFER_DURATION (buffer);

    g_signal_emit_by_name (appsrc, "push-buffer", buffer, &ret);

    if (ret != GST_FLOW_OK) {
        /* something wrong, stop pushing */
        std::cerr << "problem" << std::endl;
        g_main_loop_quit (loop);
    }
    std::cerr << "ok " << std::endl;
}

NetworkStreamer::~NetworkStreamer()
{
    /* clean up */
    gst_element_set_state (pipeline, GST_STATE_NULL);
    gst_object_unref (GST_OBJECT (pipeline));
    g_main_loop_unref (loop);
    _instance = nullptr;
}
