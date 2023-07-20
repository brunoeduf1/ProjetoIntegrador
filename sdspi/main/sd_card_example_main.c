/* SD card and FAT filesystem example.
   This example uses SPI peripheral to communicate with SD card.

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#include <string.h>
#include <sys/unistd.h>
#include <sys/stat.h>
#include "esp_vfs_fat.h"
#include "sdmmc_cmd.h"
#include "esp_camera.h"

static const char *TAG = "example";

#define MOUNT_POINT "/sdcard"
TickType_t delay_ticks = pdMS_TO_TICKS(10 * 60 * 1000);

// Pin assignments can be set in menuconfig, see "SD SPI Example Configuration" menu.
// You can also change the pin assignments here by changing the following 4 lines.
#define PIN_NUM_MISO  CONFIG_EXAMPLE_PIN_MISO
#define PIN_NUM_MOSI  CONFIG_EXAMPLE_PIN_MOSI
#define PIN_NUM_CLK   CONFIG_EXAMPLE_PIN_CLK
#define PIN_NUM_CS    CONFIG_EXAMPLE_PIN_CS

//Take picture
bool takeNewPhoto = false;
bool workInProgress = false;
bool flashEnabled = false;
//#define FILE_PHOTO "photo.png"
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

		        .frame_size = FRAMESIZE_SVGA,
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


void app_main(void)
{
    esp_err_t ret;

    ESP_LOGI(TAG, "Initializing Camera");
    init_camera();

    // Options for mounting the filesystem.
    // If format_if_mount_failed is set to true, SD card will be partitioned and
    // formatted in case when mounting fails.
    esp_vfs_fat_sdmmc_mount_config_t mount_config = {
#ifdef CONFIG_EXAMPLE_FORMAT_IF_MOUNT_FAILED
        .format_if_mount_failed = true,
#else
        .format_if_mount_failed = false,
#endif // EXAMPLE_FORMAT_IF_MOUNT_FAILED
        .max_files = 5,
        .allocation_unit_size = 16 * 1024
    };
    sdmmc_card_t *card;
    const char mount_point[] = MOUNT_POINT;
    ESP_LOGI(TAG, "Initializing SD card");

    // Use settings defined above to initialize SD card and mount FAT filesystem.
    // Note: esp_vfs_fat_sdmmc/sdspi_mount is all-in-one convenience functions.
    // Please check its source code and implement error recovery when developing
    // production applications.
    ESP_LOGI(TAG, "Using SPI peripheral");

    sdmmc_host_t host = SDSPI_HOST_DEFAULT();
    spi_bus_config_t bus_cfg = {
        .mosi_io_num = PIN_NUM_MOSI,
        .miso_io_num = PIN_NUM_MISO,
        .sclk_io_num = PIN_NUM_CLK,
        .quadwp_io_num = -1,
        .quadhd_io_num = -1,
        .max_transfer_sz = 4000,
    };
    ret = spi_bus_initialize(host.slot, &bus_cfg, SDSPI_DEFAULT_DMA);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to initialize bus.");
        return;
    }

    // This initializes the slot without card detect (CD) and write protect (WP) signals.
    // Modify slot_config.gpio_cd and slot_config.gpio_wp if your board has these signals.
    sdspi_device_config_t slot_config = SDSPI_DEVICE_CONFIG_DEFAULT();
    slot_config.gpio_cs = PIN_NUM_CS;
    slot_config.host_id = host.slot;

    ESP_LOGI(TAG, "Mounting filesystem");
    ret = esp_vfs_fat_sdspi_mount(mount_point, &host, &slot_config, &mount_config, &card);

    if (ret != ESP_OK) {
        if (ret == ESP_FAIL) {
            ESP_LOGE(TAG, "Failed to mount filesystem. "
                     "If you want the card to be formatted, set the CONFIG_EXAMPLE_FORMAT_IF_MOUNT_FAILED menuconfig option.");
        } else {
            ESP_LOGE(TAG, "Failed to initialize the card (%s). "
                     "Make sure SD card lines have pull-up resistors in place.", esp_err_to_name(ret));
        }
        return;
    }
    ESP_LOGI(TAG, "Filesystem mounted");

    // Card has been initialized, print its properties
    sdmmc_card_print_info(stdout, card);

    // Use POSIX and C standard library functions to work with files.
    int count = 0;
    char nome[25];

    //while(count < 500)
    //{
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
				const char *photo = "photo";

				const char *file_photo = MOUNT_POINT"/photo";

				ESP_LOGI(TAG, "Opening file %s", file_photo);
				FILE *file = fopen(file_photo, "w");
				if (file == NULL) {
					ESP_LOGE(TAG, "Failed to open file for writing");
					return;
				}
				else
				{
					fwrite(fb->buf, 1, fb->len, file); // payload (image), payload length
					ESP_LOGI(TAG,"The picture has been saved in SD CARD");
					ESP_LOGI(TAG," - Size: %d" ,fb->len);
					ESP_LOGI(TAG," bytes");
				}
				fclose(file);

			    // Rename original file
				sprintf(nome, "%s%d.png", file_photo, count);
			    ESP_LOGI(TAG, "Renaming file %s to %s", file_photo, nome);
			    if (rename(file_photo, nome) != 0) {
			        ESP_LOGE(TAG, "Rename failed");
			        return;
			    }

				ESP_LOGI(TAG, "File written");

			  esp_camera_fb_return(fb);
			  ok = true;

			}while(!ok);

			workInProgress = false;
		//}

		//count++;
		vTaskDelay(delay_ticks);
    }


    // All done, unmount partition and disable SPI peripheral
    esp_vfs_fat_sdcard_unmount(mount_point, card);
    ESP_LOGI(TAG, "Card unmounted");

    //deinitialize the bus after all devices are removed
    spi_bus_free(host.slot);
}

