
extern "C"{
	#include "settings.h"
	#include <sys/unistd.h>
	#include <sys/stat.h>
	#include "esp_err.h"
	#include "esp_spiffs.h"
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

void process_image()
{
	camera_fb_t *fb = get_picture();
	//Mat inputImage(fb->height, fb->width, CV_8UC2, fb->buf);
	//Rect roi(50, 50, 100, 100); // X, Y, largura, altura
	//Mat cropped = inputImage(roi);

	//Mat gray;

	//int width = inputImage.cols;
	//int height = inputImage.rows;
	//int channels = inputImage.channels();

	// criar uma nova estrutura camera_fb_t
	//camera_fb_t* fb2 = NULL;

	// definir os parâmetros da estrutura camera_fb_t
	//fb2->format = PIXFORMAT_RGB888;
	//fb2->width = width;
	//fb2->height = height;
	//fb2->len = width * height * channels;
	//fb2->buf = (uint8_t*)inputImage.data;

	//set_result(fb2);
	//cvtColor(inputImage, gray, COLOR_RGB2GRAY);
	//threshold(inputImage, gray, 128, 255, THRESH_BINARY);

	//bool hasObject = (thresholded) > 0);

	if (!fb){
	    	ESP_LOGI(TAG,"Camera capture failed");
	}
	else{
		ESP_LOGI(TAG,"Camera capture ok");
	}
}

extern "C" void app_main() {

	main_func();

	ESP_LOGI(TAG, "***** Working ******");

	//process_image();
	//xTaskCreatePinnedToCore(process_image, "image", 1024 * 9, nullptr, 24, nullptr, 0);
	//xTaskCreate(&process_image, "process_image", 8192, NULL, 5, NULL);
}




