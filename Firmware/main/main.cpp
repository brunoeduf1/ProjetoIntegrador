
extern "C"{
	#include "settings.h"
	#include <sys/unistd.h>
	#include <sys/stat.h>
	#include "esp_err.h"
	#include "esp_spiffs.h"
	#include "sdcard.h"
}
#include "esp_camera.h"
#include <esp_log.h>
#include <stdio.h>

#include "opencv2/core.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/imgcodecs.hpp"
#include <opencv2/opencv.hpp>

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <esp_freertos_hooks.h>
#include <iostream>
#include <map>


static const char *TAG = "APP_MAIN";

using namespace cv;

void process_image(void *arg)
{
	camera_fb_t *fb = get_picture();

	if (!fb){
	    	ESP_LOGI(TAG,"Camera capture failed");
	}
	else{
		ESP_LOGI(TAG,"Camera capture ok");

	Mat inputImage(fb->height, fb->width, CV_8UC2, fb->buf);
	cvtColor(inputImage, inputImage, COLOR_BGR5652GRAY); // rgb565 is 2 channels of 8-bit unsigned


	ESP_LOGI(TAG,"print ============================");

	vTaskDelete(NULL);
	}
}

extern "C" void app_main() {

	main_func();

	ESP_LOGI(TAG, "***** Working ******");

	xTaskCreatePinnedToCore(process_image, "process_image", 1024 * 9, nullptr, 24, nullptr, 0);
}




