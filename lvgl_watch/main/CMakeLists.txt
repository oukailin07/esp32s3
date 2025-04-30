#idf_component_register(SRCS "esp32_s3_szp.c" "main.c" "wifi.c" "sd_card.c" "ui_mjpeg_player.c" "bsp_spiffs.c"  "tcp_save_file.c"
                    #INCLUDE_DIRS ".")

file(GLOB_RECURSE srcs *.c
                    ui/custom/*.c
                    ui/generated/*.c
                    ui/generated/guider_customer_fonts/*.c
                    ui/generated/guider_fonts/*.c
                    ui/generated/images/*.c
                    wifi/src/*.c
                    esp-sr/*.c
                    http/*.c
                    myFont/*.c
                    )
 
set(include_dirs 
    . 
    ui/custom 
    ui/generated 
    ui/generated/guider_customer_fonts
    ui/generated/guider_fonts
    ui/generated/images
    wifi/include
    esp-sr/
    http/
    )
 
idf_component_register(
    SRCS ${srcs}
    INCLUDE_DIRS ${include_dirs}
    )
 

spiffs_create_partition_image(ui_img ../img FLASH_IN_PROJECT)
