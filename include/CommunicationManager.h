#ifndef COMMUNICATION_MANAGER_H
#define COMMUNICATION_MANAGER_H

#include "icommunicationmanager.h"
#include "UploadDataLoaderARINC615A.h"
#include "FindARINC615A.h"

#include <memory>

/**
 * @brief Communication manager. This class is responsible for managing all
 *       communication operations.
 */
class CommunicationManager
{
public:
    CommunicationManager();
    ~CommunicationManager();

    /*
    ****************************************************************************
                                    GENERAL
    ****************************************************************************
    */

    /**
     * @brief Set TFTP server port for DataLoader TFTP server.
     *        This is the port to be used by the DataLoader's
     *        TFTP server to send/receive files to/from the TargetHardware.
     *        The default port for TFTP in ARINC-615A is 59, but this port is
     *        used by the TargetHardware, so if you're running both the
     *        TargetHardware and the DataLoader in the same machine, you must
     *        change the default port
     *
     * @param[in] handler the communication handler.
     * @param[in] port the port to be used by DataLoader's TFTP server.
     *
     * @return COMMUNICATION_OPERATION_OK if success.
     * @return COMMUNICATION_OPERATION_ERROR otherwise.
     */
    CommunicationOperationResult setTftpDataLoaderServerPort(unsigned short port);
    
    /**
     * @brief Set TFTP server port for TargetHardware TFTP server.
     *        This is the port to be used by the Dataloader's
     *        TFTP client to connect to the TargetHardware's TFTP server.
     *        If this function is not called, the default port is 59 will be used.
     *
     * @param[in] handler the communication handler.
     * @param[in] port the port to be used by ARINC-615A TFTP server.
     *
     * @return COMMUNICATION_OPERATION_OK if success.
     * @return COMMUNICATION_OPERATION_ERROR otherwise.
     */
    CommunicationOperationResult setTftpTargetHardwareServerPort(unsigned short port);

    /*
    ****************************************************************************
                                     FIND OPERATION
    ****************************************************************************
    */
    /**
     * @brief Register find started callback.
     *
     * @param[in] callback the callback.
     * @param[in] context the user context.
     *
     * @return COMMUNICATION_OPERATION_OK if success.
     * @return COMMUNICATION_OPERATION_ERROR otherwise.
     */
    CommunicationOperationResult registerFindStartedCallback(
        findStarted callback,
         void *context);

    /**
     * @brief Register find finished callback.
     *
     * @param[in] callback the callback.
     * @param[in] context the user context.
     *
     * @return COMMUNICATION_OPERATION_OK if success.
     * @return COMMUNICATION_OPERATION_ERROR otherwise.
     */
    CommunicationOperationResult registerFindFinishedCallback(
        findFinished callback,
         void *context);

    /**
     * @brief Register find new device callback.
     *
     * @param[in] callback the callback.
     * @param[in] context the user context.
     *
     * @return COMMUNICATION_OPERATION_OK if success.
     * @return COMMUNICATION_OPERATION_ERROR otherwise.
     */
    CommunicationOperationResult registerFindNewDeviceCallback(
        findNewDevice callback,
         void *context);

    /**
     * @brief Start find operation. This is a non-blocking function.
     *        Devices found will be notified through the callback.
     *
     * @return COMMUNICATION_OPERATION_OK if success.
     * @return COMMUNICATION_OPERATION_ERROR otherwise.
     */
    CommunicationOperationResult find();

    /*
    ****************************************************************************
                                    UPLOAD OPERATION
    ****************************************************************************
    */
    /**
     * @brief Set TargetHardware ID. This is the ID of the TargetHardware where the
     *        load will be uploaded. You can get the TargetHardware's ID from the
     *        find operation.
     *
     * @param[in] targetHardwareId the TargetHardware ID.
     *
     * @return COMMUNICATION_OPERATION_OK if success.
     * @return COMMUNICATION_OPERATION_ERROR otherwise.
     */
    CommunicationOperationResult setTargetHardwareId(const char *targetHardwareId);

    /**
     * @brief Set TargetHardware position. This is the position of the TargetHardware
     *        where the load will be uploaded. You can get the TargetHardware's
     *        position from the find operation.
     *
     * @param[in] targetHardwarePosition the TargetHardware position.
     *
     * @return COMMUNICATION_OPERATION_OK if success.
     * @return COMMUNICATION_OPERATION_ERROR otherwise.
     */
    CommunicationOperationResult setTargetHardwarePosition(
        const char *targetHardwarePosition);

    /**
     * @brief Set TargetHardware IP. This is the IP of the TargetHardware
     *        where the load will be uploaded. You can get the TargetHardware's
     *        IP from the find operation.
     *
     * @param[in] targetHardwareIp the TargetHardware IP.
     *
     * @return COMMUNICATION_OPERATION_OK if success.
     * @return COMMUNICATION_OPERATION_ERROR otherwise.
     */
    CommunicationOperationResult setTargetHardwareIp(const char *targetHardwareIp);

    /**
     * @brief Set load list. This is the list of files to be transmitted using
     *        upload operation. Call this method before calling upload.
     *
     * @param[in] load_list the load list.
     * @param[in] load_list_size the load list size.
     *
     * @return COMMUNICATION_OPERATION_OK if success.
     * @return COMMUNICATION_OPERATION_ERROR otherwise.
     */
    CommunicationOperationResult setLoadList(Load *load_list, size_t load_list_size);

    /**
     * Register a callback for upload initialization response.
     *
     * @param[in] callback the callback.
     * @param[in] context the user context.
     *
     * @return COMMUNICATION_OPERATION_OK if success.
     * @return COMMUNICATION_OPERATION_ERROR otherwise.
     */
    CommunicationOperationResult registerUploadInitializationResponseCallback(
        uploadInitializationResponseCallback callback,
         void *context);

    /**
     * Register a callback for upload information status.
     *
     * @param[in] callback the callback.
     * @param[in] context the user context.
     *
     * @return COMMUNICATION_OPERATION_OK if success.
     * @return COMMUNICATION_OPERATION_ERROR otherwise.
     */
    CommunicationOperationResult registerUploadInformationStatusCallback(
        uploadInformationStatusCallback callback,
         void *context);

    /**
     * Register a callback for file not available.
     *
     * @param[in] callback the callback.
     * @param[in] context the user context.
     *
     * @return COMMUNICATION_OPERATION_OK if success.
     * @return COMMUNICATION_OPERATION_ERROR otherwise.
     */
    CommunicationOperationResult registerFileNotAvailableCallback(
        fileNotAvailableCallback callback,
         void *context);

    /**
     * @brief Start upload operation. This method must called by the dataloader
     * to start the upload to the target hardware.
     *
     * @return COMMUNICATION_OPERATION_OK if success.
     * @return COMMUNICATION_OPERATION_ERROR otherwise.
     */
    CommunicationOperationResult upload();

    /**
     * @brief Abort upload operation.
     *
     * @param[in] abortSource the abort source.
     *
     * @return COMMUNICATION_OPERATION_OK if success.
     * @return COMMUNICATION_OPERATION_ERROR otherwise.
     */
    CommunicationOperationResult abortUpload(AbortSource abortSource);

private:
    // TODO: Create an interface if you ever want to be able to use this class
    //       with different loaders.
    std::unique_ptr<UploadDataLoaderARINC615A> uploader;
    std::unique_ptr<FindARINC615A> finder;
};

#endif // COMMUNICATION_MANAGER_H