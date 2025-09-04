/**
 * @file main_test.c
 *
 */

#include <stdio.h>
#include <direct.h>   /* For _mkdir */
#include <sys/stat.h> /* For stat */
#include <string.h>   /* For strrchr */
#include <io.h>       /* For _commit */
#include <stdlib.h>
#include <stdint.h>
#include "cJSON.h"
#include "cJSON_Utils.h"

#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif

/**
 * @enum
 * @brief Process exit codes for the JSON people parser.
 */
enum {
    EXIT_OK = 0,
    EXIT_FAIL = 1,
    ERR_INVALID_JSON = 2,
    ERR_NO_PEOPLE_ARRAY = 3,
    // ...
};

/**
 * @enum age_code_t
 * 
 * @brief Discrete code values for ages 18..40.
 *
 * @note Each enumerator maps an exact age N to code (N - AGE_MIN).
 * @note AGE_CODE_OUT_OF_RANGE (-1) marks an age outside [AGE_MIN, AGE_MAX].
 */
typedef enum {
    AGE_CODE_18 = 0, AGE_CODE_19, AGE_CODE_20, AGE_CODE_21, AGE_CODE_22,
    AGE_CODE_23, AGE_CODE_24, AGE_CODE_25, AGE_CODE_26, AGE_CODE_27,
    AGE_CODE_28, AGE_CODE_29, AGE_CODE_30, AGE_CODE_31, AGE_CODE_32,
    AGE_CODE_33, AGE_CODE_34, AGE_CODE_35, AGE_CODE_36, AGE_CODE_37,
    AGE_CODE_38, AGE_CODE_39, AGE_CODE_40,
    AGE_CODE_OUT_OF_RANGE = -1
} age_code_t;

/**
 * @typedef person_t
 * 
 * @brief Compact person record.
 */
typedef struct {
    char       name[33];      /**< Name (max 32 chars + '\0'). */
    char       address[257];  /**< Address (max 256 chars + '\0'). */
    uint8_t    age;           /**< Actual age in years (0..255). */
    age_code_t age_code;      /**< Age code: 0..22 for ages 18..40, else AGE_CODE_OUT_OF_RANGE. */
} person_t;

/**
 * @brief Convert age [18..40] to age_code_t [0..22] (code = age - 18).
 * 
 * @param age Age in years.
 * 
 * @return AGE_CODE_18..AGE_CODE_40, or AGE_CODE_OUT_OF_RANGE if outside [18..40].
 */
static inline age_code_t age_to_code(int age) {
    if (age >= 18 && age <= 40) {
        return (age_code_t)(age - 18);
    }
    return AGE_CODE_OUT_OF_RANGE;
}

/**
 * @brief Safely copy a C string into a fixed-size buffer.
 *
 * @param dest       Destination buffer.
 * @param dest_size  Size of @p dest in bytes.
 * @param src        Source C string (may be NULL).
 */
static void safe_string_copy(char *dest, size_t dest_size, const char *src) {
    if (!dest || dest_size == 0) return;
    
    if (!src) {
        dest[0] = '\0';
        return;
    }
    
#ifdef _MSC_VER
    strncpy_s(dest, dest_size, src, _TRUNCATE);
#else
    // Ensure null-terminate
    strncpy(dest, src, dest_size - 1);
    dest[dest_size - 1] = '\0';
#endif
}

/**
 * @brief Read an entire file into a newly allocated NUL-terminated buffer.
 *
 * @param filename  Path to the file.
 * @param file_size Optional out; set to number of bytes read on success.
 * 
 * @return char*    malloc'ed buffer on success, or NULL on error.
 *
 * @note Caller must free() the returned buffer.
 * @note Binary-safe: data may contain embedded NULs; the extra '\0' is for convenience.
 */
static char* read_file_to_string(const char *filename, size_t *file_size) {
    FILE *file = fopen(filename, "rb");
    if (!file) return NULL;
    
    // Get size file
    if (fseek(file, 0, SEEK_END) != 0) {
        fclose(file);
        return NULL;
    }
    
    long size = ftell(file);
    if (size < 0) {
        fclose(file);
        return NULL;
    }
    
    rewind(file);
    
    // malloc + 1  null terminator
    char *buffer = (char*)malloc((size_t)size + 1);
    if (!buffer) {
        fclose(file);
        return NULL;
    }
    
    // Read file
    size_t bytes_read = fread(buffer, 1, (size_t)size, file);
    fclose(file);
    
    if (bytes_read != (size_t)size) {
        free(buffer);
        return NULL;
    }
    
    buffer[size] = '\0';  // Null terminate
    if (file_size) *file_size = (size_t)size;
    return buffer;
}

/**
 * @brief Get a string from a JSON object using two alternative keys.
 *
 * @param object  JSON object to query (non-NULL).
 * @param key1    Primary key.
 * @param key2    Fallback key.
 * 
 * @return const char* Pointer to cJSON-managed string.
 * @return NULL if not found/invalid.
 * 
 * @note The returned pointer is owned by cJSON and remains valid only while @p object exists. Do not free.
 */
static const char* get_string_value(const cJSON *object, const char *key1, const char *key2) {
    const cJSON *json_item = cJSON_GetObjectItemCaseSensitive(object, key1);
    if (!cJSON_IsString(json_item) || !json_item->valuestring) {
        json_item = cJSON_GetObjectItemCaseSensitive(object, key2);
        if (!cJSON_IsString(json_item) || !json_item->valuestring) {
            return NULL;
        }
    }
    return json_item->valuestring;
}

/**
 * @brief Fetch a numeric value from a JSON object using two alternative keys.
 *
 * @param object JSON object (non-NULL).
 * @param key1   Primary key.
 * @param key2   Fallback key.
 * @param result Out param for the integer value.
 * 
 * @return EXIT_FAIL if found and numeric.
 * @return EXIT_OK otherwise.
 */
static int get_number_value(const cJSON *object, const char *key1, const char *key2, int *result) {
    const cJSON *json_item = cJSON_GetObjectItemCaseSensitive(object, key1);
    if (!cJSON_IsNumber(json_item)) {
        json_item = cJSON_GetObjectItemCaseSensitive(object, key2);
        if (!cJSON_IsNumber(json_item)) {
            return EXIT_OK;
        }
    }
    *result = (int)json_item->valuedouble;
    return EXIT_FAIL;
}

/**
 * @brief Locate the main array in a JSON document.
 *
 * @param root JSON value to inspect (non-NULL).
 * 
 * @return const cJSON* Pointer to the found array.
 * @return NULL if not found.
 */
static const cJSON* find_people_array(const cJSON *root) {

    if (cJSON_IsArray(root)) {
        return root;
    }
    
    const cJSON *array = cJSON_GetObjectItemCaseSensitive(root, "data");
    if (cJSON_IsArray(array)) return array;
    
    array = cJSON_GetObjectItemCaseSensitive(root, "people");
    if (cJSON_IsArray(array)) return array;
    
    array = cJSON_GetObjectItemCaseSensitive(root, "persons");
    if (cJSON_IsArray(array)) return array;
    
    return NULL;
}

/**
 * @brief Parse one person object from JSON into @p person and log it.
 *
 * @param json_person  cJSON object representing a person (non-NULL).
 * @param person       Output struct to fill (non-NULL).
 * @param index        Display index for logging.
 */
static void process_person(const cJSON *json_person, person_t *person, int index) {
    // Initialize the output struct
    memset(person, 0, sizeof(person_t));
    
    // Fetch name (try "Name" then "name")
    const char *name = get_string_value(json_person, "Name", "name");
    safe_string_copy(person->name, sizeof(person->name), name ? name : "Unknown");
    
    // Fetch address (try "Address" then "address")
    const char *address = get_string_value(json_person, "Address", "address");
    safe_string_copy(person->address, sizeof(person->address), address ? address : "Unknown");
    
    // Fetch age (try "Age" then "age")
    int age_value = 0;
    if (get_number_value(json_person, "Age", "age", &age_value)) {
        if (age_value < 0) age_value = 0;
        if (age_value > 255) age_value = 255;
        person->age = (uint8_t)age_value;
    }
    
    // Derive age_code from age
    person->age_code = age_to_code((int)person->age);
    
    // Print summary; mark out-of-range if age_code is invalid
    if (person->age_code == AGE_CODE_OUT_OF_RANGE) {
        printf("[%03d] Name: '%s', Address: '%s', Age: %u -> OUT_OF_RANGE (not 18-40)\n",
               index, person->name, person->address, person->age);
    } else {
        printf("[%03d] Name: '%s', Address: '%s', Age: %u, Age Code: %d\n",
               index, person->name, person->address, person->age, person->age_code);
    }
}

int main(void) {
    printf("=== PROJECT_1_MINH_NHAT ===\n");
    
    // Try possible paths for data.json
    const char *possible_paths[] = {
        "data/data.json",     // in CWD/data/
        // ... Add other path
        NULL
    };
    
    char *json_text = NULL;
    const char *used_path = NULL;
    
    // Try each candidate path
    for (int i = 0; possible_paths[i]; i++) {
        json_text = read_file_to_string(possible_paths[i], NULL);
        if (json_text) {
            used_path = possible_paths[i];
            break;
        }
    }
    
    if (!json_text) {
        fprintf(stderr, "ERROR: data.json not found\n");
        fprintf(stderr, "Please ensure data.json is in the same directory as the executable (or update the search paths).\n");
        return EXIT_FAIL;
    }

    printf("Opened file: %s\n", used_path);
    
    // Parse JSON
    cJSON *root = cJSON_Parse(json_text);
    if (!root) {
        const char *error_ptr = cJSON_GetErrorPtr();
        fprintf(stderr, "ERROR: invalid JSON");
        if (error_ptr) {
            fprintf(stderr, " tại: %.20s", error_ptr);
        }
        fprintf(stderr, "\n");
        free(json_text);
        return ERR_INVALID_JSON;
    }
    
    // Locate the array containing person records
    const cJSON *people_array = find_people_array(root);
    if (!people_array) {
        fprintf(stderr, "ERROR: People array not found in JSON\n");
        fprintf(stderr, "JSON must be an array or an object containing 'data', 'people', or 'persons'.\n");
        cJSON_Delete(root);
        free(json_text);
        return ERR_NO_PEOPLE_ARRAY;
    }
    
    int total_records = cJSON_GetArraySize(people_array);
    printf("Total records: %d\n", total_records);
    printf("---\n");
    
    // Process each person record
    for (int i = 0; i < total_records; i++) {
        const cJSON *json_person = cJSON_GetArrayItem(people_array, i);
        if (!cJSON_IsObject(json_person)) {
            printf("[%03d] SKIP: not a JSON object\n", i);
            continue;
        }
        
        person_t person;
        process_person(json_person, &person, i);
        
        // TODO: If needed, append `person` to a dynamic array here.
    }
    
    printf("---\n");
    printf("Finished processing %d record(s)\n", total_records);
    
    // Cleanup
    cJSON_Delete(root);
    free(json_text);
    
    return EXIT_OK;
}