/**
 * @file main_test.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h> /* For strrchr */  
#include <io.h>     /* For _commit */

/*********************
 *      DEFINES
 *********************/
#ifdef _WIN32
  #include <direct.h>   /* For _mkdir */
  #define MKDIR(p) _mkdir(p)
#else
  #include <sys/stat.h> /* For stat */
  #include <sys/types.h>
  #define MKDIR(p) mkdir(p, 0755)
#endif


/**********************
 *   STATIC FUNCTIONS
 **********************/

 //le little-endian
static int read_exact(void *dst, size_t n, FILE *f)
{
    return fread(dst, 1, n, f) == n;
}
static int read_u16_le(uint16_t *out, FILE *f)
{
    unsigned char b[2];
    if (!read_exact(b, 2, f))
    {
        return 0;
    }
    *out = (uint16_t)(b[0] | ((uint16_t)b[1] << 8));
    return 1;
}
static int read_u32_le(uint32_t *out, FILE *f)
{
    unsigned char b[4];
    if (!read_exact(b, 4, f))
    {
        return 0;
    }
    *out = (uint32_t)b[0] | ((uint32_t)b[1] << 8) | ((uint32_t)b[2] << 16) | ((uint32_t)b[3] << 24);
    return 1;
}
static int read_f32_le(float *out, FILE *f)
{
    uint32_t u;
    if (!read_u32_le(&u, f))
    {
        return 0;
    }
    memcpy(out, &u, 4);  // IEEE-754
    return 1;
}
static int read_f64_le(double *out, FILE *f) {
    unsigned char b[8]; 
    if (!read_exact(b, 8, f))
    {
        return 0;
    }
    uint64_t u = ((uint64_t)b[0]) |
                 ((uint64_t)b[1] << 8) |
                 ((uint64_t)b[2] << 16) |
                 ((uint64_t)b[3] << 24) |
                 ((uint64_t)b[4] << 32) |
                 ((uint64_t)b[5] << 40) |
                 ((uint64_t)b[6] << 48) |
                 ((uint64_t)b[7] << 56);
    memcpy(out, &u, 8);  // IEEE-754
    return 1;
}

static const char* battery_to_str(uint8_t v)
{
    switch (v) 
    {
        case 0: return "normal";
        case 1: return "low";
        case 2: return "emergency";
        default: return "unknown";
    }
}

/**********************
 *   GLOBAL FUNCTIONS
 **********************/
int main(int argc, char **argv)
{
    const char *in_path  = (argc > 1) ? argv[1] : "weather_data.bin";
    const char *out_dir  = "data";
    const char *out_path = (argc > 2) ? argv[2] : "data/weather_data.json";

    FILE *fin = fopen(in_path, "rb"); // rb read-binary
    if (!fin)
    {
        fprintf(stderr, "ERROR: cannot open input file: %s\n", in_path);
        return 1;
    }

    /* Header: id[4], version[2], count[4] */
    unsigned char file_id[4];
    uint16_t version = 0;
    uint32_t count   = 0;

    if (!read_exact(file_id, 4, fin) || !read_u16_le(&version, fin) || !read_u32_le(&count, fin)) {
        fprintf(stderr, "ERROR: cannot read header\n");
        fclose(fin);
        return 1;
    }

    /* (Optional) check size: 10 (Header) + 57*count */
    long after_header = ftell(fin);
    fseek(fin, 0, SEEK_END);
    long fsz = ftell(fin);
    fseek(fin, after_header, SEEK_SET);
    long expected = 10 + (long)57 * (long)count;
    if (fsz != expected)
    {
        fprintf(stderr, "WARNING: file size (%ld) != expected (%ld). Continuing...\n", fsz, expected);
    }

    /* mkdir data/ */
    (void)MKDIR(out_dir);

    FILE *fout = fopen(out_path, "w");
    if (!fout)
    {
        fprintf(stderr, "ERROR: cannot open output file: %s\n", out_path);
        fclose(fin);
        return 1;
    }

    /* JSON header */
    fprintf(fout,
            "{\"file_id\":\"%c%c%c%c\",\"version\":%u,\"count\":%u,\n\"records\":[\n",
            file_id[0], file_id[1], file_id[2], file_id[3],
            (unsigned)version, (unsigned)count);

    for (uint32_t i = 0; i < count; ++i)
    {
        uint32_t sensor_id, timestamp;
        uint8_t  battery;
        double   lat, lon;
        float    temp, hum, pres, wind_spd, rain, uv, light;
        uint16_t co2, wind_dir;

        if (!read_u32_le(&sensor_id, fin)) break;
        if (!read_exact(&battery, 1, fin)) break;
        if (!read_u32_le(&timestamp, fin)) break;
        if (!read_f64_le(&lat, fin)) break;
        if (!read_f64_le(&lon, fin)) break;
        if (!read_f32_le(&temp, fin)) break;
        if (!read_f32_le(&hum, fin)) break;
        if (!read_f32_le(&pres, fin)) break;
        if (!read_u16_le(&co2, fin)) break;
        if (!read_f32_le(&wind_spd, fin)) break;
        if (!read_u16_le(&wind_dir, fin)) break;
        if (!read_f32_le(&rain, fin)) break;
        if (!read_f32_le(&uv, fin)) break;
        if (!read_f32_le(&light, fin)) break;

        fprintf(fout,
            "  {\"sensor_id\":%u,"
            "\"battery\":\"%s\","
            "\"timestamp\":%u,"
            "\"lat\":%.6f,\"lon\":%.6f,"
            "\"temperature\":%.2f,"
            "\"humidity\":%.2f,"
            "\"pressure\":%.2f,"
            "\"co2\":%u,"
            "\"wind_speed\":%.2f,"
            "\"wind_dir\":%u,"
            "\"rain\":%.2f,"
            "\"uv\":%.2f,"
            "\"light\":%.2f}%s\n",
            sensor_id,
            battery_to_str(battery),
            timestamp,
            lat, lon,
            temp, hum, pres,
            co2,
            wind_spd,
            wind_dir,
            rain, uv, light,
            (i + 1 < count ? "," : "")
        );
    }

    fprintf(fout, "]}\n");
    fclose(fout);
    fclose(fin);

    printf("Wrote JSON to %s\n", out_path);
    return 0;
}