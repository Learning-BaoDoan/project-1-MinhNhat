#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "cJSON.h"

#define NAME_LEN 33
#define ADDR_LEN 257

typedef struct {
    char    name[NAME_LEN];
    char    address[ADDR_LEN];
    uint8_t age;
    int8_t  age_code;
} person_t;

static int8_t age_to_code(int age) {
    return (age >= 18 && age <= 40) ? (int8_t)(age - 18) : -1;
}

static char* read_all(const char *path) {
    FILE *f = fopen(path, "rb");
    if (!f) return NULL;
    if (fseek(f, 0, SEEK_END) != 0) { fclose(f); return NULL; }
    long sz = ftell(f);
    if (sz < 0) { fclose(f); return NULL; }
    rewind(f);
    char *buf = (char*)malloc((size_t)sz + 1);
    if (!buf) { fclose(f); return NULL; }
    size_t n = fread(buf, 1, (size_t)sz, f);
    fclose(f);
    if (n != (size_t)sz) { free(buf); return NULL; }
    buf[sz] = '\0';
    return buf;
}

int main(void) {

    char *json_text = read_all("data/data.json");
    if (!json_text) {
        fprintf(stderr, "ERROR: data.json not found\n");
        return 1;
    }


    cJSON *root = cJSON_Parse(json_text);
    if (!root || !cJSON_IsArray(root)) {
        fprintf(stderr, "ERROR: invalid JSON (expected array)\n");
        cJSON_Delete(root);
        free(json_text);
        return 1;
    }


    int total = cJSON_GetArraySize(root);
    for (int i = 0; i < total; ++i) {
        cJSON *obj = cJSON_GetArrayItem(root, i);
        if (!cJSON_IsObject(obj)) continue;

        cJSON *jn = cJSON_GetObjectItem(obj, "Name");
        cJSON *ja = cJSON_GetObjectItem(obj, "Address");
        cJSON *jg = cJSON_GetObjectItem(obj, "Age");

        const char *name = (cJSON_IsString(jn) && jn->valuestring) ? jn->valuestring : "Unknown";
        const char *addr = (cJSON_IsString(ja) && ja->valuestring) ? ja->valuestring : "Unknown";
        int age = cJSON_IsNumber(jg) ? (int)jg->valuedouble : 0;

        person_t p;
        strncpy(p.name, name, NAME_LEN - 1);     p.name[NAME_LEN - 1] = '\0';
        strncpy(p.address, addr, ADDR_LEN - 1);  p.address[ADDR_LEN - 1] = '\0';
        if (age < 0) age = 0; if (age > 255) age = 255;
        p.age = (uint8_t)age;
        p.age_code = age_to_code(age);

        printf("%s | %s | age=%u | code=%d\n",
               p.name, p.address, p.age, p.age_code);
    }

    cJSON_Delete(root);
    free(json_text);
    return 0;
}
