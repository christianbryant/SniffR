set(srcs "scd40.c")

idf_component_register(
    SRCS ${srcs}
    INCLUDE_DIRS "include"
    REQUIRES driver
    REQUIRES esp_adc
)