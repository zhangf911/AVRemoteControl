#pragma once

#include <iostream>
#include <chrono>
#include <functional>
#include <thread>
#include <vector>
#include <map>

extern "C"
{
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
#include <libavdevice/avdevice.h>
};

class InputStream;

enum class InputStreamType
{
	Desktop,
	Camera,
	Video,
	Network
};
