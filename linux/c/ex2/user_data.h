#ifndef USER_DATA_H
#define USER_DATA_H

#include <stdint.h>
#include <stdbool.h>

#define MAX_NAME_LEN    (50u)
#define MAX_PHONE_LEN    (20u)
#define MAX_PATH_LEN    (128u)
#define IMAGE_SIZE    (1024u)
#define VALID_USER_ID_START    (1000u)

typedef enum e_user_status
{
    USER_INVALID = 0,
    USER_VALID = 1
} e_user_status_t;

typedef struct st_user
{
    uint32_t id;                        
    char name[MAX_NAME_LEN];                    
    char phone[MAX_PHONE_LEN];                     
    char image_path[MAX_PATH_LEN];           
    uint8_t age;                     
    uint8_t is_valid;                    
} st_user_t;


st_user_t* get_user_input(void);
bool validate_user_data(st_user_t* p_user);
void print_user_info(const st_user_t* p_user, const uint8_t* p_image);

#endif // USER_DATA_H
