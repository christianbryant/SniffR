set(srcs "nvs_driver.c")

idf_component_register(
    SRCS ${srcs}
    INCLUDE_DIRS "include"
    REQUIRES driver nvs_flash 
)