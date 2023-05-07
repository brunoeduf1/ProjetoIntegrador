#include <stdio.h>
#include <stdlib.h>
#include <string.h> //Requires by memset
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "spi_flash_mmap.h"
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
#include "page.h"
#include "esp_event_loop.h"

#include "esp_timer.h"
#include "img_converters.h"

#include "esp_http_client.h"
#include "cJson.h"
#include "esp_crt_bundle.h"
#include "esp_tls.h"

#include "main2.h"

#if CONFIG_MBEDTLS_CERTIFICATE_BUNDLE
#include "esp_crt_bundle.h"
#endif

extern const char howsmyssl_com_root_cert_pem_start[] asm("_binary_howsmyssl_com_root_cert_pem_start");
extern const char howsmyssl_com_root_cert_pem_end[]   asm("_binary_howsmyssl_com_root_cert_pem_end");

#define MAX_HTTP_RECV_BUFFER 512
#define MAX_HTTP_OUTPUT_BUFFER 2048

#define PART_BOUNDARY "123456789000000000000987654321"

static const char* _STREAM_CONTENT_TYPE = "multipart/x-mixed-replace;boundary=" PART_BOUNDARY;
static const char* _STREAM_BOUNDARY = "\r\n--" PART_BOUNDARY "\r\n";
static const char* _STREAM_PART = "Content-Type: image/jpeg\r\nContent-Length: %u\r\n\r\n";

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

esp_err_t _http_event_handler(esp_http_client_event_t *evt)
{
    static char *output_buffer;  // Buffer to store response of http request from event handler
    static int output_len;       // Stores number of bytes read
    switch(evt->event_id) {
        case HTTP_EVENT_ERROR:
            ESP_LOGD(TAG, "HTTP_EVENT_ERROR");
            break;
        case HTTP_EVENT_ON_CONNECTED:
            ESP_LOGD(TAG, "HTTP_EVENT_ON_CONNECTED");
            break;
        case HTTP_EVENT_HEADER_SENT:
            ESP_LOGD(TAG, "HTTP_EVENT_HEADER_SENT");
            break;
        case HTTP_EVENT_ON_HEADER:
            ESP_LOGD(TAG, "HTTP_EVENT_ON_HEADER, key=%s, value=%s", evt->header_key, evt->header_value);
            break;
        case HTTP_EVENT_ON_DATA:
            ESP_LOGD(TAG, "HTTP_EVENT_ON_DATA, len=%d", evt->data_len);
            /*
             *  Check for chunked encoding is added as the URL for chunked encoding used in this example returns binary data.
             *  However, event handler can also be used in case chunked encoding is used.
             */
            if (!esp_http_client_is_chunked_response(evt->client)) {
                // If user_data buffer is configured, copy the response into the buffer
                int copy_len = 0;
                if (evt->user_data) {
                    copy_len = MIN(evt->data_len, (MAX_HTTP_OUTPUT_BUFFER - output_len));
                    if (copy_len) {
                        memcpy(evt->user_data + output_len, evt->data, copy_len);
                    }
                } else {
                    const int buffer_len = esp_http_client_get_content_length(evt->client);
                    if (output_buffer == NULL) {
                        output_buffer = (char *) malloc(buffer_len);
                        output_len = 0;
                        if (output_buffer == NULL) {
                            ESP_LOGE(TAG, "Failed to allocate memory for output buffer");
                            return ESP_FAIL;
                        }
                    }
                    copy_len = MIN(evt->data_len, (buffer_len - output_len));
                    if (copy_len) {
                        memcpy(output_buffer + output_len, evt->data, copy_len);
                    }
                }
                output_len += copy_len;
            }

            break;
        case HTTP_EVENT_ON_FINISH:
            ESP_LOGD(TAG, "HTTP_EVENT_ON_FINISH");
            if (output_buffer != NULL) {
                // Response is accumulated in output_buffer. Uncomment the below line to print the accumulated response
                // ESP_LOG_BUFFER_HEX(TAG, output_buffer, output_len);
                free(output_buffer);
                output_buffer = NULL;
            }
            output_len = 0;
            break;
        case HTTP_EVENT_DISCONNECTED:
            ESP_LOGI(TAG, "HTTP_EVENT_DISCONNECTED");
            int mbedtls_err = 0;
            esp_err_t err = esp_tls_get_and_clear_last_error((esp_tls_error_handle_t)evt->data, &mbedtls_err, NULL);
            if (err != 0) {
                ESP_LOGI(TAG, "Last esp error code: 0x%x", err);
                ESP_LOGI(TAG, "Last mbedtls failure: 0x%x", mbedtls_err);
            }
            if (output_buffer != NULL) {
                free(output_buffer);
                output_buffer = NULL;
            }
            output_len = 0;
            break;
        case HTTP_EVENT_REDIRECT:
            ESP_LOGD(TAG, "HTTP_EVENT_REDIRECT");
            esp_http_client_set_header(evt->client, "From", "user@example.com");
            esp_http_client_set_header(evt->client, "Accept", "text/html");
            esp_http_client_set_redirection(evt->client);
            break;
    }
    return ESP_OK;
}

void connect_wifi(void)
{
    s_wifi_event_group = xEventGroupCreate();

    ESP_ERROR_CHECK(esp_netif_init());

    ESP_ERROR_CHECK(esp_event_loop_create_default());
    esp_netif_t *sta_netif = esp_netif_create_default_wifi_sta();
    assert(sta_netif);
    esp_netif_set_hostname(sta_netif, "ESP32_Tutorials");

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    esp_event_handler_instance_t instance_any_id;
    esp_event_handler_instance_t instance_got_ip;
    ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT,
                                                        ESP_EVENT_ANY_ID,
                                                        &event_handler,
														sta_netif,
                                                        &instance_any_id));
    ESP_ERROR_CHECK(esp_event_handler_instance_register(IP_EVENT,
                                                        IP_EVENT_STA_GOT_IP,
                                                        &event_handler,
														sta_netif,
                                                        &instance_got_ip));

    wifi_config_t wifi_config = {
        .sta = {
            .ssid = EXAMPLE_ESP_WIFI_SSID,
            .password = EXAMPLE_ESP_WIFI_PASS,
            /* Setting a password implies station will connect to all security modes including WEP/WPA.
             * However these modes are deprecated and not advisable to be used. Incase your Access point
             * doesn't support WPA2, these mode can be enabled by commenting below line */
			  .scan_method = WIFI_FAST_SCAN,
			  .sort_method = WIFI_CONNECT_AP_BY_SIGNAL,
			  .threshold.rssi = -127,
			  .threshold.authmode = WIFI_AUTH_OPEN
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

static esp_err_t capture_handler(httpd_req_t *req){
    //Serial.println("Capture image");
    camera_fb_t * fb = NULL;
    esp_err_t res = ESP_OK;
    fb = esp_camera_fb_get();
    if (!fb) {
        //Serial.println("Camera capture failed");
        httpd_resp_send_500(req);
        return ESP_FAIL;
    }
    httpd_resp_set_type(req, "image/jpeg");
    httpd_resp_set_hdr(req, "Content-Disposition", "inline; filename=capture.jpg");
    httpd_resp_set_hdr(req, "Access-Control-Allow-Origin", "*");

    res = httpd_resp_send(req, (const char *)fb->buf, fb->len);
    esp_camera_fb_return(fb);
    return res;
}

static esp_err_t stream_handler(httpd_req_t *req){
    camera_fb_t * fb = NULL;
    esp_err_t res = ESP_OK;
    size_t _jpg_buf_len = 0;
    uint8_t * _jpg_buf = NULL;
    char * part_buf[64];

    res = httpd_resp_set_type(req, _STREAM_CONTENT_TYPE);
    if(res != ESP_OK){
        return res;
    }
    httpd_resp_set_hdr(req, "Access-Control-Allow-Origin", "*");
    while(true){
        fb = esp_camera_fb_get();
        if (!fb) {
            //Serial.println("Camera capture failed");
            res = ESP_FAIL;
        } else {

                if(fb->format != PIXFORMAT_JPEG){
                    bool jpeg_converted = frame2jpg(fb, 80, &_jpg_buf, &_jpg_buf_len);
                    esp_camera_fb_return(fb);
                    fb = NULL;
                    if(!jpeg_converted){
                        //Serial.println("JPEG compression failed");
                        res = ESP_FAIL;
                    }
                } else {
                    _jpg_buf_len = fb->len;
                    _jpg_buf = fb->buf;
                }
             }
        if(res == ESP_OK){
            res = httpd_resp_send_chunk(req, _STREAM_BOUNDARY, strlen(_STREAM_BOUNDARY));
        }
        if(res == ESP_OK){
            size_t hlen = snprintf((char *)part_buf, 64, _STREAM_PART, _jpg_buf_len);
            res = httpd_resp_send_chunk(req, (const char *)part_buf, hlen);
        }
        if(res == ESP_OK){
            res = httpd_resp_send_chunk(req, (const char *)_jpg_buf, _jpg_buf_len);
        }
        if(fb){
            esp_camera_fb_return(fb);
            fb = NULL;
            _jpg_buf = NULL;
        } else if(_jpg_buf){
            free(_jpg_buf);
            _jpg_buf = NULL;
        }
        if(res != ESP_OK){
            break;
        }
    }
    return res;
}

esp_err_t get_req_handler(httpd_req_t *req)
{
    return send_web_page(req);
}

esp_err_t photo_handler(httpd_req_t *req)
{
    return send_web_page(req);
}

esp_err_t control_handler(httpd_req_t *req)
{
    const char* remote_control = "Send";
    ESP_LOGI(TAG, "Signal received OK");
	return httpd_resp_send(req, remote_control, sizeof(remote_control));
}

static void http_rest_with_hostname_path(void)
{
    esp_http_client_config_t config = {
        .host = "httpbin.org",
        .path = "/get",
        .transport_type = HTTP_TRANSPORT_OVER_SSL,
        .event_handler = _http_event_handler,
		.crt_bundle_attach = esp_crt_bundle_attach,
		.cert_pem = howsmyssl_com_root_cert_pem_start,
    };
    esp_http_client_handle_t client = esp_http_client_init(&config);
    esp_err_t err;

    //  POST PUSH NOTIFICATION
	char *token = "key=AAAAVclZv2E:APA91bHtv5D6GnQtQofDniqrIzfGxlqNGZToedPo1ixYaXPrDzyCgXqqDzjWqdoZ9V3EMglMBR2Z2Uro0z8nkf7m04SFPtyZstQu8aWdD9yWtZBSSnhZZuB3cu0bEYiVWQKJ2qLYb7HC";
    const char *post_data =
    		"{\"to\":"
    			"\"dBGIcwIoQbOQdqIqtmXC47:APA91bF66Ao8saiu9U_A21tAasFKLszbYDVzD5wJEehWuz7v3PhiZXm1TRQ6NbsXDJO75xvMXjOyIzU1s5hqm34R3Xi-evRqT65Xj2lFDKx1Z4-p-YukaKugOvjXLXjrVTnRaVbYENFn\","
    		"\"notification\":"
    			"{\"body\":\"Corpo da notificacao\","
    			"\"title\":\"Titulo da notificacao\"}}";
    esp_http_client_set_url(client, "https://fcm.googleapis.com/fcm/send");
    esp_http_client_set_method(client, HTTP_METHOD_POST);
    esp_http_client_set_header(client, "Content-Type", "application/json");
    esp_http_client_set_header(client, "Authorization", token);
    esp_http_client_set_post_field(client, post_data, strlen(post_data));
    err = esp_http_client_perform(client);
    if (err == ESP_OK) {
	   ESP_LOGI(TAG, "HTTP POST Status = %d, content_length = %lld",
			   esp_http_client_get_status_code(client),
			   esp_http_client_get_content_length(client));
    } else {
	   ESP_LOGE(TAG, "HTTP POST request failed: %s", esp_err_to_name(err));
    }

    esp_http_client_cleanup(client);
}

httpd_uri_t page_uri = {
    .uri       = "/ts",
    .method    = HTTP_GET,
    .handler   = stream_handler,
    .user_ctx  = NULL
};

httpd_uri_t uri_photo = {
    .uri = "/photo",
    .method = HTTP_GET,
	.handler = photo_handler,
    .user_ctx = NULL};

httpd_uri_t capture_uri = {
    .uri       = "/capture",
    .method    = HTTP_GET,
    .handler   = capture_handler,
    .user_ctx  = NULL
};

httpd_uri_t control_uri = {
    .uri       = "/control",
    .method    = HTTP_GET,
    .handler   = control_handler,
    .user_ctx  = NULL
};

httpd_uri_t get_uri = {
    .uri       = "/",
    .method    = HTTP_GET,
    .handler   = control_handler,
    .user_ctx  = NULL
};

httpd_handle_t setup_server(void)
{
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    httpd_handle_t server = NULL;

    if (httpd_start(&server, &config) == ESP_OK)
    {
    	httpd_register_uri_handler(server, &capture_uri);
        httpd_register_uri_handler(server, &uri_photo);
        httpd_register_uri_handler(server, &page_uri);
        httpd_register_uri_handler(server, &control_uri);
        httpd_register_uri_handler(server, &get_uri);
    }

    return server;
}

//SPIFFS
// ledPin refers to ESP32-CAM GPIO 4 (flashlight)
#define FLASH_GPIO_NUM 4

// Photo File Name to save in SPIFFS
#define FILE_PHOTO "photo.jpg"

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
			        ESP_LOGI(TAG,"The picture has been saved in /spiffs");
			        ESP_LOGI(TAG," - Size: %d" ,fb->len);
			        ESP_LOGI(TAG," bytes");

			      }

			      fclose(file);
			      esp_camera_fb_return(fb);
			      ok = true;

	    	}while(!ok);

	    	workInProgress = false;
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
				.grab_mode = CAMERA_GRAB_WHEN_EMPTY,
		        .pixel_format = PIXFORMAT_JPEG,

		        .frame_size = FRAMESIZE_QVGA,
		        .jpeg_quality = 10,
		        .fb_count = 2,
		        };

static esp_err_t init_camera()
{
	esp_err_t err = esp_camera_init(&camera_config);

    if (err != ESP_OK)
    {
        ESP_LOGE(TAG, "Camera Init Failed");
        return err;
    }

    return ESP_OK;
}

void func()
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
    //ESP_ERROR_CHECK(connect_wifi);
    connect_wifi();

    //xTaskCreate(&http_test_task, "http_test_task", 8192, NULL, 5, NULL);
    http_rest_with_hostname_path();

	// Initialize camera
    init_camera();

    sensor_t * s = esp_camera_sensor_get();
    // initial sensors are flipped vertically and colors are a bit saturated
    if (s->id.PID == OV3660_PID) {
      s->set_vflip(s, 1); // flip it back
      s->set_brightness(s, 1); // up the brightness just a bit
      s->set_saturation(s, -2); // lower the saturation
    }
    // drop down frame size for higher initial frame rate
    s->set_framesize(s, FRAMESIZE_QVGA);

	// Initialize SPIFFS
    init_spiffs();

    // Initialize server
    setup_server();

    //Capture photo
    //while(1) {
         capturePhotoSaveSpiffs();
   //      vTaskDelay(1500);
   // }
}
