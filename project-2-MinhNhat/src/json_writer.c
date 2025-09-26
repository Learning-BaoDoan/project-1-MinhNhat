/**
 * @file json_writer.c
 * @brief JSON writer implementation
 */

/*********************
 *    INCLUDES
 *********************/
#include "json_writer.h"
#include "weather_types.h"
#include <stdio.h>
#include <errno.h>
#include <string.h>

/*********************
 *    DEFINES
 *********************/
#ifdef _WIN32
  #include <direct.h>
  #define MKDIR(p) _mkdir(p)
#else
  #include <sys/stat.h>
  #include <sys/types.h>
  #define MKDIR(p) mkdir(p, 0755)
#endif

/*********************
 *    FUNCTIONS
 *********************/
void write_json_header(const file_header_t *header, FILE *f)
{
    fprintf(f, "{\n");
    fprintf(f, "  \"metadata\": {\n");
    fprintf(f, "    \"file_id\": \"%s\",\n", header->file_id);
    fprintf(f, "    \"version\": %u,\n", header->version);
    fprintf(f, "    \"record_count\": %u\n", header->count);
    fprintf(f, "  },\n");
    fprintf(f, "  \"records\": [\n");
}

void write_json_record(const weather_record_t *record, FILE *f, int is_last)
{
    fprintf(f,
        "    {\n"
        "      \"sensor_id\": %u,\n"
        "      \"battery\": \"%s\",\n"
        "      \"timestamp\": %u,\n"
        "      \"location\": {\n"
        "        \"lat\": %.8f,\n"
        "        \"lon\": %.8f\n"
        "      },\n"
        "      \"measurements\": {\n"
        "        \"temperature\": %.2f,\n"
        "        \"humidity\": %.2f,\n"
        "        \"pressure\": %.2f,\n"
        "        \"co2\": %u,\n"
        "        \"wind\": {\n"
        "          \"speed\": %.2f,\n"
        "          \"direction\": %u\n"
        "        },\n"
        "        \"rain\": %.2f,\n"
        "        \"uv\": %.2f,\n"
        "        \"light\": %.2f\n"
        "      }\n"
        "    }%s\n",
        record->sensor_id,
        battery_status_to_string(record->battery),
        record->timestamp,
        record->lat, record->lon,
        record->temperature,
        record->humidity,
        record->pressure,
        record->co2,
        record->wind_speed,
        record->wind_dir,
        record->rain,
        record->uv,
        record->light,
        is_last ? "" : ","
    );
}

void write_json_footer(FILE *f)
{
    fprintf(f, "  ]\n");
    fprintf(f, "}\n");
}

int create_output_directory(const char *dir_path)
{
    if (MKDIR(dir_path) != 0 && errno != EEXIST)
    {
        fprintf(stderr, "WARNING: Could not create directory '%s': %s\n", 
                dir_path, strerror(errno));
        return -1;
    }
    return 0;
}