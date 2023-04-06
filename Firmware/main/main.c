#include <stdio.h>
#include <stdlib.h>
#include <string.h> //Requires by memset
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_spi_flash.h"
#include <esp_http_server.h>
#include "esp_wifi.h"
#include "esp_event.h"
#include "freertos/event_groups.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "esp_netif.h"
#include "driver/gpio.h"
#include <lwip/sockets.h>
#include <lwip/sys.h>
#include <lwip/api.h>
#include <lwip/netdb.h>
//SPIFFS
#include <sys/unistd.h>
#include <sys/stat.h>
#include "esp_err.h"
#include "esp_spiffs.h"
//Take picture
#include <sys/param.h>
#include "esp_camera.h"
#include "driver/sdmmc_host.h"
#include "driver/sdmmc_defs.h"
#include "sdmmc_cmd.h"
#include "esp_vfs_fat.h"
#include "driver/gpio.h"

#define LED_PIN 4

static const char *TAG = "espressif"; // TAG for debug
int led_state = 0;

#define EXAMPLE_ESP_WIFI_SSID "IMMF"
#define EXAMPLE_ESP_WIFI_PASS "Marcelle2017"
#define EXAMPLE_ESP_MAXIMUM_RETRY 5

/* FreeRTOS event group to signal when we are connected*/
static EventGroupHandle_t s_wifi_event_group;

/* The event group allows multiple bits for each event, but we only care about two events:
 * - we are connected to the AP with an IP
 * - we failed to connect after the maximum amount of retries */
#define WIFI_CONNECTED_BIT BIT0
#define WIFI_FAIL_BIT BIT1

static int s_retry_num = 0;

static void event_handler(void *arg, esp_event_base_t event_base,
                          int32_t event_id, void *event_data)
{
    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START)
    {
        esp_wifi_connect();
    }
    else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED)
    {
        if (s_retry_num < EXAMPLE_ESP_MAXIMUM_RETRY)
        {
            esp_wifi_connect();
            s_retry_num++;
            ESP_LOGI(TAG, "retry to connect to the AP");
        }
        else
        {
            xEventGroupSetBits(s_wifi_event_group, WIFI_FAIL_BIT);
        }
        ESP_LOGI(TAG, "connect to the AP fail");
    }
    else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP)
    {
        ip_event_got_ip_t *event = (ip_event_got_ip_t *)event_data;
        ESP_LOGI(TAG, "got ip:" IPSTR, IP2STR(&event->ip_info.ip));
        s_retry_num = 0;
        xEventGroupSetBits(s_wifi_event_group, WIFI_CONNECTED_BIT);
    }
}

void connect_wifi(void)
{
    s_wifi_event_group = xEventGroupCreate();

    ESP_ERROR_CHECK(esp_netif_init());

    ESP_ERROR_CHECK(esp_event_loop_create_default());
    esp_netif_create_default_wifi_sta();

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    esp_event_handler_instance_t instance_any_id;
    esp_event_handler_instance_t instance_got_ip;
    ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT,
                                                        ESP_EVENT_ANY_ID,
                                                        &event_handler,
                                                        NULL,
                                                        &instance_any_id));
    ESP_ERROR_CHECK(esp_event_handler_instance_register(IP_EVENT,
                                                        IP_EVENT_STA_GOT_IP,
                                                        &event_handler,
                                                        NULL,
                                                        &instance_got_ip));

    wifi_config_t wifi_config = {
        .sta = {
            .ssid = EXAMPLE_ESP_WIFI_SSID,
            .password = EXAMPLE_ESP_WIFI_PASS,
            /* Setting a password implies station will connect to all security modes including WEP/WPA.
             * However these modes are deprecated and not advisable to be used. Incase your Access point
             * doesn't support WPA2, these mode can be enabled by commenting below line */
            .threshold.authmode = WIFI_AUTH_WPA2_PSK,
        },
    };
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config));
    ESP_ERROR_CHECK(esp_wifi_start());

    ESP_LOGI(TAG, "wifi_init_sta finished.");

    /* Waiting until either the connection is established (WIFI_CONNECTED_BIT) or connection failed for the maximum
     * number of re-tries (WIFI_FAIL_BIT). The bits are set by event_handler() (see above) */
    EventBits_t bits = xEventGroupWaitBits(s_wifi_event_group,
                                           WIFI_CONNECTED_BIT | WIFI_FAIL_BIT,
                                           pdFALSE,
                                           pdFALSE,
                                           portMAX_DELAY);

    /* xEventGroupWaitBits() returns the bits before the call returned, hence we can test which event actually
     * happened. */
    if (bits & WIFI_CONNECTED_BIT)
    {
        ESP_LOGI(TAG, "connected to ap SSID:%s password:%s",
                 EXAMPLE_ESP_WIFI_SSID, EXAMPLE_ESP_WIFI_PASS);
    }
    else if (bits & WIFI_FAIL_BIT)
    {
        ESP_LOGI(TAG, "Failed to connect to SSID:%s, password:%s",
                 EXAMPLE_ESP_WIFI_SSID, EXAMPLE_ESP_WIFI_PASS);
    }
    else
    {
        ESP_LOGE(TAG, "UNEXPECTED EVENT");
    }
    vEventGroupDelete(s_wifi_event_group);
}

esp_err_t send_web_page(httpd_req_t *req)
{
	char* image_path = "/spiffs/photo.jpg";
	char* image_type = "image/jpeg";

	FILE* fp = fopen(image_path, "r");
	if(fp == NULL){
		ESP_LOGE(TAG, "NÃ£o foi possÃ­vel abrir o arquivo de imagem");
		return ESP_FAIL;
	}

	/* Define o tipo MIME da resposta */
	httpd_resp_set_type(req, image_type);

	/* Define o cabeÃ§alho de comprimento de conteÃºdo para o tamanho do arquivo de imagem */
	fseek(fp, 0L, SEEK_END);
	size_t content_length = ftell(fp);
	rewind(fp);
	httpd_resp_set_hdr(req, "Content-Length", (char*) &content_length);

	/* Enviar conteÃºdo do arquivo de imagem como parte da resposta HTTP */
	char buf[1024];
	size_t read_len;
	while((read_len = fread(buf, 1, sizeof(buf), fp)) > 0) {
		httpd_resp_send_chunk(req, buf, read_len);
	}
	fclose(fp);

    return ESP_OK;
}

esp_err_t get_req_handler(httpd_req_t *req)
{
    return send_web_page(req);
}

esp_err_t photo_handler(httpd_req_t *req)
{
    return send_web_page(req);
}

httpd_uri_t uri_get = {
    .uri = "/",
    .method = HTTP_GET,
    .handler = get_req_handler,
    .user_ctx = NULL};

httpd_uri_t uri_photo = {
    .uri = "/photo",
    .method = HTTP_GET,
	.handler = photo_handler,
    .user_ctx = NULL};

httpd_handle_t setup_server(void)
{
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    httpd_handle_t server = NULL;

    if (httpd_start(&server, &config) == ESP_OK)
    {
        httpd_register_uri_handler(server, &uri_photo);
    }

    return server;
}

//SPIFFS
// juntando as coisas
// ledPin refers to ESP32-CAM GPIO 4 (flashlight)
#define FLASH_GPIO_NUM 4

// Photo File Name to save in SPIFFS
#define FILE_PHOTO "/photo.jpg"

bool takeNewPhoto = false;
bool workInProgress = false;
bool flashEnabled = false;

void init_spiffs()
{
	 ESP_LOGI(TAG, "Initializing SPIFFS");

		    esp_vfs_spiffs_conf_t conf = {
		      .base_path = "/spiffs",
		      .partition_label = NULL,
		      .max_files = 5,
		      .format_if_mount_failed = true
		    };

		    esp_err_t ret = esp_vfs_spiffs_register(&conf);

		    if (ret != ESP_OK) {
		        if (ret == ESP_FAIL) {
		            ESP_LOGE(TAG, "Failed to mount or format filesystem");
		        } else if (ret == ESP_ERR_NOT_FOUND) {
		            ESP_LOGE(TAG, "Failed to find SPIFFS partition");
		        } else {
		            ESP_LOGE(TAG, "Failed to initialize SPIFFS (%s)", esp_err_to_name(ret));
		        }
		        return;
		    }

		    size_t total = 0, used = 0;
		    ret = esp_spiffs_info(conf.partition_label, &total, &used);
		    if (ret != ESP_OK) {
		        ESP_LOGE(TAG, "Failed to get SPIFFS partition information (%s). Formatting...", esp_err_to_name(ret));
		        esp_spiffs_format(conf.partition_label);
		        return;
		    } else {
		        ESP_LOGI(TAG, "Partition size: total: %d, used: %d", total, used);
		    }

		    // Check consistency of reported partiton size info.
		    if (used > total) {
		        ESP_LOGW(TAG, "Number of used bytes cannot be larger than total. Performing SPIFFS_check().");
		        ret = esp_spiffs_check(conf.partition_label);
		        // Could be also used to mend broken files, to clean unreferenced pages, etc.
		        // More info at https://github.com/pellepl/spiffs/wiki/FAQ#powerlosses-contd-when-should-i-run-spiffs_check
		        if (ret != ESP_OK) {
		            ESP_LOGE(TAG, "SPIFFS_check() failed (%s)", esp_err_to_name(ret));
		            return;
		        } else {
		            ESP_LOGI(TAG, "SPIFFS_check() successful");
		        }
		    }
		    // All done, unmount partition and disable SPIFFS
		    //esp_vfs_spiffs_unregister(conf.partition_label);
		    ESP_LOGI(TAG, "SPIFFS unmounted");
}

void capturePhotoSaveSpiffs()
{

		TAG = "capturePhotoSave";
	    if (workInProgress == false)
	    {
	    	workInProgress = true;
	    	bool ok = 0;
	    	camera_fb_t *fb = NULL;
	    	do{
	    		ESP_LOGI(TAG, "Taking picture...");

	    		fb = esp_camera_fb_get();
			    if (!fb)
			    {
				  ESP_LOGI(TAG,"Camera capture failed");
				  workInProgress = false;
				  return;
			    }

			    ESP_LOGI(TAG,"Camera capture ok");
			    ESP_LOGI(TAG,"Picture file name: %s\n", FILE_PHOTO);
			    FILE* file = fopen("/spiffs/photo.jpg", "w");
			    if (file == NULL) {
			        ESP_LOGE(TAG, "Failed to open file in writing mode");
			        return;
			    }
			      else
			      {
			    	fwrite(fb->buf, 1, fb->len, file); // payload (image), payload length
			        ESP_LOGI(TAG,"The picture has been saved in ");
			        ESP_LOGI(TAG,FILE_PHOTO);
			        ESP_LOGI(TAG," - Size: %d" ,fb->len);
			        ESP_LOGI(TAG," bytes");
			        ok = true;
			      }
			      // Close the file
			      fclose(file);
			      esp_camera_fb_return(fb);

			      // check if file has been correctly saved in SPIFFS
			      //ok = checkPhoto(SPIFFS);

	    	}while(!ok);
	    }
}

// Take picture

#define BOARD_ESP32CAM_AITHINKER

#ifndef portTICK_RATE_MS
#define portTICK_RATE_MS portTICK_PERIOD_MS
#endif

#ifdef BOARD_ESP32CAM_AITHINKER

#define CAM_PIN_PWDN 32
#define CAM_PIN_RESET -1 //software reset will be performed
#define CAM_PIN_XCLK 0
#define CAM_PIN_SIOD 26
#define CAM_PIN_SIOC 27
#define CAM_PIN_D7 35
#define CAM_PIN_D6 34
#define CAM_PIN_D5 39
#define CAM_PIN_D4 36
#define CAM_PIN_D3 21
#define CAM_PIN_D2 19
#define CAM_PIN_D1 18
#define CAM_PIN_D0 5
#define CAM_PIN_VSYNC 25
#define CAM_PIN_HREF 23
#define CAM_PIN_PCLK 22

#endif

static camera_config_t camera_config = {
		 	 	 .pin_pwdn  = CAM_PIN_PWDN,
		        .pin_reset = CAM_PIN_RESET,
		        .pin_xclk = CAM_PIN_XCLK,
		        .pin_sccb_sda = CAM_PIN_SIOD,
		        .pin_sccb_scl = CAM_PIN_SIOC,

		        .pin_d7 = CAM_PIN_D7,
		        .pin_d6 = CAM_PIN_D6,
		        .pin_d5 = CAM_PIN_D5,
		        .pin_d4 = CAM_PIN_D4,
		        .pin_d3 = CAM_PIN_D3,
		        .pin_d2 = CAM_PIN_D2,
		        .pin_d1 = CAM_PIN_D1,
		        .pin_d0 = CAM_PIN_D0,
		        .pin_vsync = CAM_PIN_VSYNC,
		        .pin_href = CAM_PIN_HREF,
		        .pin_pclk = CAM_PIN_PCLK,

		        .xclk_freq_hz = 20000000,
		        .ledc_timer = LEDC_TIMER_0,
		        .ledc_channel = LEDC_CHANNEL_0,

		        .pixel_format = PIXFORMAT_JPEG,
		        .frame_size = FRAMESIZE_VGA,

		        .jpeg_quality = 10,
		        .fb_count = 1,
		        .grab_mode = CAMERA_GRAB_WHEN_EMPTY};

static esp_err_t init_camera()
{
	gpio_config_t gpio_pwr_config;
	gpio_pwr_config.pin_bit_mask = (1ULL << 32);
	gpio_pwr_config.mode = GPIO_MODE_OUTPUT;
	gpio_pwr_config.pull_down_en = GPIO_PULLDOWN_DISABLE;
	gpio_pwr_config.pull_up_en = GPIO_PULLUP_DISABLE;
	gpio_pwr_config.intr_type = GPIO_INTR_DISABLE;
	gpio_config(&gpio_pwr_config);
	gpio_set_level(32,0);
	vTaskDelay(10/ portTICK_PERIOD_MS);

	esp_err_t err = esp_camera_init(&camera_config);

    if (err != ESP_OK)
    {
        ESP_LOGE(TAG, "Camera Init Failed");
        return err;
    }

    return ESP_OK;
}

void app_main()
{
    // Initialize NVS
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND)
    {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    ESP_LOGI(TAG, "ESP_WIFI_MODE_STA");
    connect_wifi();

	// Initialize camera
    if(ESP_OK != init_camera()) {
        return;
    }

	// Initialize SPIFFS and capture photo
    init_spiffs();
    capturePhotoSaveSpiffs();

    // Initialize server
    setup_server();
}
