idf_component_register(SRCS "lvgl_dome.c" 
                            "../components/BSP/lib_lcd7735.c" 
                            "lv_port/lv_port_disp.c"
                            "lv_port/lv_port_fs.c"
                            "lv_port/lv_port_indev.c"
                    INCLUDE_DIRS "." 
                                "../components/BSP/."
                                "lv_port/.")
