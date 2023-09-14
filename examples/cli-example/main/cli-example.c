/* PPP_SERVER Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include <string.h>

#include "esp_console.h"
#include "esp_log.h"
#include "esp_netif.h"
#include "esp_netif_ppp.h"
#include "nvs_flash.h"
#include "esp_vfs_fat.h"

#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"

#include "cmd_system.h"
#include "cmd_wifi.h"
#include "cli_client.h"
#include "cli_server.h"

static const char *TAG = "cli-example";

#define MOUNT_PATH "/data"
#define HISTORY_PATH MOUNT_PATH "/history.txt"


static int cmd_cli_server(int argc, char **argv)
{
    cli_server_config_t cli_server = DEFAULT_CLI_SERVER_CONFIG;

    cli_server_init(&cli_server);
    return 0;
}

static void initialize_nvs(void)
{
    esp_err_t err = nvs_flash_init();
    if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK( nvs_flash_erase() );
        err = nvs_flash_init();
    }
    ESP_ERROR_CHECK(err);
}

void app_main(void)
{

    esp_console_repl_t *repl = NULL;
    esp_console_repl_config_t repl_config = ESP_CONSOLE_REPL_CONFIG_DEFAULT();
    esp_console_dev_uart_config_t uart_config = ESP_CONSOLE_DEV_UART_CONFIG_DEFAULT();
    repl_config.prompt = "cli-client>";

    // init console REPL environment
    ESP_ERROR_CHECK(esp_console_new_repl_uart(&uart_config, &repl_config, &repl));
    esp_log_level_set("cli_server", ESP_LOG_VERBOSE);
    esp_log_level_set("*", ESP_LOG_INFO);


    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    initialize_nvs();

    /* Register commands */
    register_system_common();
    register_wifi();

    const esp_console_cmd_t cli_server_cmd = {
        .command = "cli_server",
        .help = "Start cli server",
        .hint = NULL,
        .func = &cmd_cli_server,
    };
    ESP_ERROR_CHECK(esp_console_cmd_register(&cli_server_cmd));


    cli_client_config_t client_config = DEFAULT_CLI_CLIENT_CONFIG;
    cli_client_init(&client_config);


    // start console REPL
    ESP_ERROR_CHECK(esp_console_start_repl(repl));
}
