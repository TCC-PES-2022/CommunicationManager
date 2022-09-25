#ifndef COMMUNICATION_API_H
#define COMMUNICATION_API_H

#include <stdlib.h>

/**
 * @brief The commuication handler for ARINC-615A communication.
 */
typedef struct CommunicationHandler *CommunicationHandlerPtr;

/**
 * @brief Enum with possible return from interface functions.
 * Possible return values are:
 * - COMMUNICATION_OPERATION_OK:                    Operation was successful.
 * - COMMUNICATION_OPERATION_ERROR:                 Generic error.
 */
typedef enum
{
    COMMUNICATION_OPERATION_OK = 0,
    COMMUNICATION_OPERATION_ERROR
} CommunicationOperationResult;

typedef enum
{
    OPERATION_ABORTED_BY_THE_DATALOADER,
    OPERATION_ABORTED_BY_THE_OPERATOR
} AbortSource;

#define MAX_NAME_SIZE 255
typedef struct
{
    char loadName[MAX_NAME_SIZE];
    char partNumber[MAX_NAME_SIZE];
} Load;

/*
*******************************************************************************
                                   CALLBACKS
*******************************************************************************
*/

/**
 * @brief Callback for find operation started. This callback is called right
 * before the find operation is started.
 *
 * @param[in] handler the communication handler.
 * @param[in] context the user context.
 *
 * @return COMMUNICATION_OPERATION_OK if success.
 * @return COMMUNICATION_OPERATION_ERROR otherwise.
 */
typedef CommunicationOperationResult (*find_started)(
    CommunicationHandlerPtr handler,
    void *context);

/**
 * @brief Callback for find operation finished
 *
 * @param[in] handler the communication handler.
 * @param[in] context the user context.
 *
 * @return COMMUNICATION_OPERATION_OK if success.
 * @return COMMUNICATION_OPERATION_ERROR otherwise.
 */
typedef CommunicationOperationResult (*find_finished)(
    CommunicationHandlerPtr handler,
    void *context);

/**
 * @brief Callback for new devices found. The device information is
 * returned in a JSON format.
 *
 * TODO: document the JSON format.
 *
 * @param[in] handler the communication handler.
 * @param[in] device JSON with device information.
 * @param[in] context the user context.
 *
 * @return COMMUNICATION_OPERATION_OK if success.
 * @return COMMUNICATION_OPERATION_ERROR otherwise.
 */
typedef CommunicationOperationResult (*find_new_device)(
    CommunicationHandlerPtr handler,
    const char *device,
    void *context);

/**
 * @brief Callback for upload initialization response.
 *
 * TODO: document the JSON format.
 *
 * @param[in] handler the communication handler.
 * @param[in] upload_initialization_response_json JSON with upload
 *                                                initialization response.
 * @param[in] context the user context.
 *
 * @return COMMUNICATION_OPERATION_OK if success.
 * @return COMMUNICATION_OPERATION_ERROR otherwise.
 */
typedef CommunicationOperationResult (*upload_initialization_response_callback)(
    CommunicationHandlerPtr handler,
    const char *upload_initialization_response_json,
    void *context);

/**
 * @brief Callback for upload progress report.
 *
 * //TODO: document the JSON format.
 *
 * @param[in] handler the communication handler.
 * @param[in] upload_information_status_json JSON with upload information status.
 * @param[in] context the user context.
 *
 * @return COMMUNICATION_OPERATION_OK if success.
 * @return COMMUNICATION_OPERATION_ERROR otherwise.
 */
typedef CommunicationOperationResult (*upload_information_status_callback)(
    CommunicationHandlerPtr handler,
    const char *upload_information_status_json,
    void *context);

/**
 * @brief Callback for file not available.
 *
 * //TODO: document the JSON format.
 *
 * @param[in] handler the communication handler.
 * @param[out] wait_time_s time to wait in seconds before next try.
 * @param[in] context the user context.
 *
 * @return COMMUNICATION_OPERATION_OK if success.
 * @return COMMUNICATION_OPERATION_ERROR otherwise.
 */
typedef CommunicationOperationResult (*file_not_available_callback)(
    CommunicationHandlerPtr handler,
    unsigned short *wait_time_s,
    void *context);

/*
*******************************************************************************
                             MANAGEMENT OPERATIONS
*******************************************************************************
*/

/**
 * Create and initialize a new communication handler.
 *
 * @param[out] handler a handler to ARINC-615A communication.
 * @return COMMUNICATION_OPERATION_OK if success.
 * @return COMMUNICATION_OPERATION_ERROR otherwise.
 */
CommunicationOperationResult create_handler(
    CommunicationHandlerPtr *handler);

/**
 * Destroy a communication handler.
 *
 * @param[in] handler a handler to ARINC-615A communication.
 * @return COMMUNICATION_OPERATION_OK if success.
 * @return COMMUNICATION_OPERATION_ERROR otherwise.
 */
CommunicationOperationResult destroy_handler(
    CommunicationHandlerPtr *handler);

/*
*******************************************************************************
                                 FIND OPERATION
*******************************************************************************
*/

/**
 * Register a callback for find operation started.
 *
 * @param[in] handler the communication handler.
 * @param[in] callback the callback.
 * @param[in] context the user context.
 *
 * @return COMMUNICATION_OPERATION_OK if success.
 * @return COMMUNICATION_OPERATION_ERROR otherwise.
 */
CommunicationOperationResult register_find_started_callback(
    CommunicationHandlerPtr handler,
    find_started callback,
    void *context);

/**
 * Register a callback for find operation finished.
 *
 * @param[in] handler the communication handler.
 * @param[in] callback the callback.
 * @param[in] context the user context.
 *
 * @return COMMUNICATION_OPERATION_OK if success.
 * @return COMMUNICATION_OPERATION_ERROR otherwise.
 */
CommunicationOperationResult register_find_finished_callback(
    CommunicationHandlerPtr handler,
    find_finished callback,
    void *context);

/**
 * Register a callback for new devices found.
 *
 * @param[in] handler the communication handler.
 * @param[in] callback the callback.
 * @param[in] context the user context.
 *
 * @return COMMUNICATION_OPERATION_OK if success.
 * @return COMMUNICATION_OPERATION_ERROR otherwise.
 */
CommunicationOperationResult register_find_new_device_callback(
    CommunicationHandlerPtr handler,
    find_new_device callback,
    void *context);

/**
 * Start find operation. This is a non-blocking function.
 * Devices found will be notified through the callback.
 *
 * @param[in] handler the communication handler.
 *
 * @return COMMUNICATION_OPERATION_OK if success.
 * @return COMMUNICATION_OPERATION_ERROR otherwise.
 */
CommunicationOperationResult find(
    CommunicationHandlerPtr handler);

/*
*******************************************************************************
                                UPLOAD OPERATION
*******************************************************************************
*/

/**
 * @brief Set TargetHardware ID. This is the ID of the TargetHardware where the
 *        load will be uploaded. You can get the TargetHardware's ID from the
 *        find operation.
 *
 *        This function must be called before upload operation.
 *
 * @param[in] handler the communication handler.
 * @param[in] target_id the TargetHardware ID.
 *
 * @return COMMUNICATION_OPERATION_OK if success.
 * @return COMMUNICATION_OPERATION_ERROR otherwise.
 */
CommunicationOperationResult set_target_hardware_id(
    CommunicationHandlerPtr handler, const char *target_id);

/**
 * @brief Set TargetHardware position. This is the position of the TargetHardware
 *        where the load will be uploaded. You can get the TargetHardware's
 *        position from the find operation.
 *
 *        This function must be called before upload operation.
 *
 * @param[in] handler the communication handler.
 * @param[in] target_pos the TargetHardware position.
 *
 * @return COMMUNICATION_OPERATION_OK if success.
 * @return COMMUNICATION_OPERATION_ERROR otherwise.
 */
CommunicationOperationResult set_target_hardware_pos(
    CommunicationHandlerPtr handler, const char *target_pos);

/**
 * @brief Set TargetHardware IP. This is the IP of the TargetHardware
 *        where the load will be uploaded. You can get the TargetHardware's
 *        IP from the find operation.
 *
 *        This function must be called before upload operation.
 *
 * @param[in] handler the communication handler.
 * @param[in] target_ip the TargetHardware IP.
 *
 * @return COMMUNICATION_OPERATION_OK if success.
 * @return COMMUNICATION_OPERATION_ERROR otherwise.
 */
CommunicationOperationResult set_target_hardware_ip(
    CommunicationHandlerPtr handler, const char *target_ip);

/**
 * @brief Set load list. This is the list of files to be transmitted using
 *        upload operation.
 *
 *        This function must be called before upload operation.
 *
 * @param[in] handler the communication handler.
 * @param[in] load_list the load list.
 * @param[in] load_list_size the load list size.
 *
 * @return COMMUNICATION_OPERATION_OK if success.
 * @return COMMUNICATION_OPERATION_ERROR otherwise.
 */
CommunicationOperationResult set_load_list(
    CommunicationHandlerPtr handler, Load *load_list, size_t load_list_size);

/**
 * Register a callback for upload initialization response.
 *
 * @param[in] handler the communication handler.
 * @param[in] callback the callback.
 * @param[in] context the user context.
 *
 * @return COMMUNICATION_OPERATION_OK if success.
 * @return COMMUNICATION_OPERATION_ERROR otherwise.
 */
CommunicationOperationResult register_upload_initialization_response_callback(
    CommunicationHandlerPtr handler,
    upload_initialization_response_callback callback,
    void *context);

/**
 * Register a callback for upload information status.
 *
 * @param[in] handler the communication handler.
 * @param[in] callback the callback.
 * @param[in] context the user context.
 *
 * @return COMMUNICATION_OPERATION_OK if success.
 * @return COMMUNICATION_OPERATION_ERROR otherwise.
 */
CommunicationOperationResult register_upload_information_status_callback(
    CommunicationHandlerPtr handler,
    upload_information_status_callback callback,
    void *context);

/**
 * Register a callback for file not available.
 *
 * @param[in] handler the communication handler.
 * @param[in] callback the callback.
 * @param[in] context the user context.
 *
 * @return COMMUNICATION_OPERATION_OK if success.
 * @return COMMUNICATION_OPERATION_ERROR otherwise.
 */
CommunicationOperationResult register_file_not_available_callback(
    CommunicationHandlerPtr handler,
    file_not_available_callback callback,
    void *context);

/**
 * @brief Start upload operation.
 *
 * @param[in] handler the communication handler.
 *
 * @return COMMUNICATION_OPERATION_OK if success.
 * @return COMMUNICATION_OPERATION_ERROR otherwise.
 */
CommunicationOperationResult upload(CommunicationHandlerPtr handler);

/**
 * @brief Abort upload operation.
 *
 * @param[in] handler the communication handler.
 * @param[in] abortSource the abort source.
 *
 * @return COMMUNICATION_OPERATION_OK if success.
 * @return COMMUNICATION_OPERATION_ERROR otherwise.
 */
CommunicationOperationResult abort_upload(
    CommunicationHandlerPtr handler, AbortSource abortSource);

#endif // COMMUNICATION_API_H