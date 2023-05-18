
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
//#include "py/dynruntime.h"

static const char *TAG = "APP_MAIN";

extern "C" void app_main() {

	main_func();

	ESP_LOGI(TAG, "***** Working ******");

	camera_fb_t *fb = get_picture();

	if (!fb){
	    	ESP_LOGI(TAG,"Camera capture failed");
	}
	else{
		ESP_LOGI(TAG,"Camera capture ok");
	}

}


