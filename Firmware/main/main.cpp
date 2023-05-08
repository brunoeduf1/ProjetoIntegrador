
extern "C"{
	#include "settings.h"
}
#include <esp_log.h>
#include <stdio.h>
#include "opencv2/core.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/imgcodecs.hpp"

using namespace cv;

static const char *TAG = "espressif";

extern "C" void app_main() {
	main_func();

	ESP_LOGI(TAG, "***** Working ******");
	Mat img;
	Mat imgCopy;

}


