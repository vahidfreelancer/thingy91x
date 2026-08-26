#ifndef HW_TEST_APP_H
#define HW_TEST_APP_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Initialize the Hardware Diagnostic & Remote Test Suite Profile.
 * 
 * @return 0 on success, negative POSIX error code on failure.
 */
int app_init(void);

/**
 * @brief Run the Hardware Diagnostic & Remote Test Suite Application loop.
 */
void app_run(void);

#ifdef __cplusplus
}
#endif

#endif /* HW_TEST_APP_H */
