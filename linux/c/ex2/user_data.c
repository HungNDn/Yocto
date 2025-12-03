#include "user_data.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <sys/stat.h>

/* ID tracker: static for file scope */ 
static uint32_t s_current_id = VALID_USER_ID_START;
static uint32_t s_used_ids[100] = {0};

/**
 * @brief Check if the given ID is already used
 * @param id ID to check
 * @return true if ID is used, false otherwise
 */
static bool is_id_used(uint32_t id)
{
    for (size_t i = 0; i < (sizeof(s_used_ids)/sizeof(s_used_ids[0])); ++i)
    {
        if (s_used_ids[i] == id)
        {
            return true;
        }
    }
    return false;
}

/**
 * @brief Assign a unique ID to a new user
 * @return new unique ID or 0 on failure
 */
static uint32_t assign_unique_id(void)
{
    while (is_id_used(s_current_id))
    {
        s_current_id++;
    }

    for (size_t i = 0; i < (sizeof(s_used_ids)/sizeof(s_used_ids[0])); ++i)
    {
        if (s_used_ids[i] == 0)
        {
            s_used_ids[i] = s_current_id;
            return s_current_id++;
        }
    }
    return 0; // Failure: no available slot
}

/**
 * @brief Get user input from standard input
 * @return pointer to allocated user struct, or NULL on error
 */
st_user_t* get_user_input(void)
{
    st_user_t* user = (st_user_t*) malloc(sizeof(st_user_t));
    if (user == NULL)
    {
        perror("malloc failed");
        return NULL;
    }

    printf("\n== Enter user information ==\n");

    printf("Name: ");
    if (fgets(user->name, MAX_NAME_LEN, stdin) == NULL)
    {
        free(user);
        return NULL;
    }
    user->name[strcspn(user->name, "\n")] = '\0';

    char age_str[4];
    printf("Age: ");
    if (fgets(age_str, sizeof(age_str), stdin) == NULL)
    {
        free(user);
        return NULL;
    }
    user->age = (uint8_t)atoi(age_str);

    printf("Phone number: ");
    if (fgets(user->phone, MAX_PHONE_LEN, stdin) == NULL)
    {
        free(user);
        return NULL;
    }
    user->phone[strcspn(user->phone, "\n")] = '\0';

    printf("Image path: ");
    if (fgets(user->image_path, MAX_PATH_LEN, stdin) == NULL)
    {
        free(user);
        return NULL;
    }
    user->image_path[strcspn(user->image_path, "\n")] = '\0';

    user->id = assign_unique_id();
    if (user->id == 0)
    {
        fprintf(stderr, "Error: Unable to assign a valid ID\n");
        free(user);
        return NULL;
    }

    user->is_valid = USER_INVALID;

    return user;
}

/**
 * @brief Validate the user's name: length between 2 and MAX_NAME_LEN-1, letters and spaces only
 * @param p_name string to validate
 * @return true if valid, false otherwise
 */
static bool is_valid_name(const char* p_name)
{
    if (p_name == NULL)
    {
        return false;
    }

    size_t len = strlen(p_name);
    if (len < 2 || len > MAX_NAME_LEN - 1)
    {
        return false;
    }

    for (size_t i = 0; i < len; ++i)
    {
        unsigned char ch = (unsigned char) p_name[i];
        if (!(isalpha(ch) || isspace(ch)))
        {
            return false;
        }
    }
    return true;
}

/**
 * @brief Validate age is in range 1 to 120
 * @param age age to validate
 * @return true if valid, false otherwise
 */
static bool is_valid_age(uint8_t age)
{
    return (age > 0) && (age <= 120);
}

/**
 * @brief Validate phone number: must be 10 digits and start with '0'
 * @param p_phone string representing the phone number
 * @return true if valid, false otherwise
 */
static bool is_valid_phone(const char* p_phone)
{
    if (p_phone == NULL)
    {
        return false;
    }

    if (p_phone[0] != '0')
    {
        return false;
    }

    size_t len = strlen(p_phone);
    if (len != 10)
    {
        return false;
    }

    for (size_t i = 0; i < len; ++i)
    {
        unsigned char ch = (unsigned char) p_phone[i];
        if (!isdigit(ch))
        {
            return false;
        }
    }

    return true;
}

/**
 * @brief Validate image path: file must exist and have valid image extension (jpg, jpeg, png, bmp)
 * @param p_path file path string
 * @return true if valid, false otherwise
 */
static bool is_valid_image_path(const char* p_path)
{
    if (p_path == NULL)
    {
        return false;
    }

    size_t len = strlen(p_path);
    if (len < 5)
    {
        return false;
    }

    struct stat st;
    if (stat(p_path, &st) != 0)
    {
        return false; // File does not exist
    }

    const char* ext = strrchr(p_path, '.');
    if (ext == NULL)
    {
        return false;
    }

    if ((strcasecmp(ext, ".jpg") == 0) ||
        (strcasecmp(ext, ".jpeg") == 0) ||
        (strcasecmp(ext, ".png") == 0) ||
        (strcasecmp(ext, ".bmp") == 0))
    {
        return true;
    }

    return false;
}

/**
 * @brief Validate all user data fields
 * @param p_user pointer to user struct
 * @return true if all data valid, false otherwise
 */
bool validate_user_data(st_user_t* p_user)
{
    if (p_user == NULL)
    {
        return false;
    }

    if (!is_valid_name(p_user->name))
    {
        return false;
    }

    if (!is_valid_age(p_user->age))
    {
        return false;
    }

    if (!is_valid_phone(p_user->phone))
    {
        return false;
    }

    if (!is_valid_image_path(p_user->image_path))
    {
        return false;
    }

    p_user->is_valid = USER_VALID;
    return true;
}

/**
 * @brief Print user information and the last 100 bytes of the image as uint32_t words
 * @param p_user pointer to user struct
 * @param p_image pointer to image byte array of size IMAGE_SIZE
 */
void print_user_info(const st_user_t* p_user, const uint8_t* p_image)
{
    if (p_user == NULL || p_image == NULL)
    {
        return;
    }

    printf("\n--- USER INFORMATION ---\n");
    printf("ID: %u\n", p_user->id);
    printf("Name: %s\n", p_user->name);
    printf("Age: %u\n", p_user->age);
    printf("Phone number: %s\n", p_user->phone);
    printf("Image path: %s\n", p_user->image_path);
    printf("Status: %s\n", (p_user->is_valid == USER_VALID) ? "VALID" : "INVALID");

    printf("\n>> Last 100 bytes of image (as uint32_t):\n");
    for (size_t i = IMAGE_SIZE - 100; i < IMAGE_SIZE; i += 4)
    {
        uint32_t word;
        memcpy(&word, &p_image[i], sizeof(word));  
        printf("0x%08X ", word);

        if (((i - (IMAGE_SIZE - 100)) % 16) == 12)  // 4 words per line
        {
            printf("\n");
        }
    }
    printf("\n");
}
