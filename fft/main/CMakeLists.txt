idf_component_register(SRCS "main.c" "../components/fft/fft.c"  "esp32_s3_szp.c" "app_ui.c"
                    INCLUDE_DIRS "."
                                 "../components/fft/include")
                                 
spiffs_create_partition_image(storage ../spiffs FLASH_IN_PROJECT)