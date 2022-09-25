#include "communication_api.h"
#include "CommunicationManager.h"

#include <time.h>

struct CommunicationHandler
{
    unsigned long id;
    CommunicationManager *manager;

    find_started _findStartedCallback;
    void *_findStartedContext;
    find_finished _findFinishedCallback;
    void *_findFinishedContext;
    find_new_device _findNewDeviceCallback;
    void *_findNewDeviceContext;

    upload_initialization_response_callback _uploadInitializationResponseCallback;
    void *_uploadInitializationResponseContext;
    upload_information_status_callback _uploadInformationStatusCallback;
    void *_uploadInformationStatusContext;
    file_not_available_callback _fileNotAvailableCallback;
    void *_fileNotAvailableContext;
};

// TODO: we may limit the number of handlers here, but for now we don't.
std::unordered_map<unsigned long, std::shared_ptr<struct CommunicationHandler>> handlers;

static FindOperationResult findStartedCbk(
    std::shared_ptr<void> context)
{
    auto handler = std::static_pointer_cast<struct CommunicationHandler>(context);
    if (handler->_findStartedCallback != nullptr)
    {
        handler->_findStartedCallback(handler.get(),
                                      handler->_findStartedContext);
        return FindOperationResult::FIND_OPERATION_OK;
    }
    return FindOperationResult::FIND_OPERATION_ERROR;
}

static FindOperationResult findFinishedCbk(
    std::shared_ptr<void> context)
{
    auto handler = std::static_pointer_cast<struct CommunicationHandler>(context);
    if (handler->_findFinishedCallback != nullptr)
    {
        handler->_findFinishedCallback(handler.get(),
                                       handler->_findFinishedContext);
        return FindOperationResult::FIND_OPERATION_OK;
    }
    return FindOperationResult::FIND_OPERATION_ERROR;
}

static FindOperationResult findNewDeviceCbk(
    std::string device,
    std::shared_ptr<void> context)
{
    auto handler = std::static_pointer_cast<struct CommunicationHandler>(context);
    if (handler->_findNewDeviceCallback != nullptr)
    {
        handler->_findNewDeviceCallback(handler.get(),
                                        device.c_str(),
                                        handler->_findNewDeviceContext);
        return FindOperationResult::FIND_OPERATION_OK;
    }
    return FindOperationResult::FIND_OPERATION_ERROR;
}

static UploadOperationResult uploadInitializationResponseCbk (
    std::string uploadInitializationResponseJson,
    std::shared_ptr<void> context)
{
    auto handler = std::static_pointer_cast<struct CommunicationHandler>(context);
    if (handler->_uploadInitializationResponseCallback != nullptr)
    {
        handler->_uploadInitializationResponseCallback(handler.get(),
                                                       uploadInitializationResponseJson.c_str(),
                                                       handler->_uploadInitializationResponseContext);
        return UploadOperationResult::UPLOAD_OPERATION_OK;
    }
    return UploadOperationResult::UPLOAD_OPERATION_ERROR;
}

static UploadOperationResult uploadInformationStatusCbk (
    std::string uploadInformationStatusJson,
    std::shared_ptr<void> context)
{
    auto handler = std::static_pointer_cast<struct CommunicationHandler>(context);
    if (handler->_uploadInformationStatusCallback != nullptr)
    {
        handler->_uploadInformationStatusCallback(handler.get(),
                                                  uploadInformationStatusJson.c_str(),
                                                  handler->_uploadInformationStatusContext);
        return UploadOperationResult::UPLOAD_OPERATION_OK;
    }
    return UploadOperationResult::UPLOAD_OPERATION_ERROR;
}

static UploadOperationResult fileNotAvailableCbk (
    uint16_t *waitTimeS,
    std::shared_ptr<void> context)
{
    auto handler = std::static_pointer_cast<struct CommunicationHandler>(context);
    if (handler->_fileNotAvailableCallback != nullptr)
    {
        unsigned short waitTime = 0;
        handler->_fileNotAvailableCallback(handler.get(),
                                           &waitTime,
                                           handler->_fileNotAvailableContext);
        *waitTimeS = waitTime;
        return UploadOperationResult::UPLOAD_OPERATION_OK;
    }
    return UploadOperationResult::UPLOAD_OPERATION_ERROR;
}

CommunicationOperationResult create_handler(CommunicationHandlerPtr *handler)
{
    if (handler == NULL)
    {
        return COMMUNICATION_OPERATION_ERROR;
    }

    std::shared_ptr<struct CommunicationHandler> localHandler = std::make_shared<struct CommunicationHandler>();
    if (localHandler == NULL)
    {
        return COMMUNICATION_OPERATION_ERROR;
    }

    // Seems like a good enough id for now
    localHandler->id = (unsigned long)time(NULL);
    localHandler->manager = new CommunicationManager();

    handlers[localHandler->id] = localHandler;

    *handler = (CommunicationHandlerPtr)localHandler.get();

    return COMMUNICATION_OPERATION_OK;
}

CommunicationOperationResult destroy_handler(CommunicationHandlerPtr *handler)
{
    if (handler == NULL)
    {
        return COMMUNICATION_OPERATION_ERROR;
    }

    delete (*handler)->manager;

    handlers.erase((*handler)->id);
    (*handler) = NULL;

    return COMMUNICATION_OPERATION_OK;
}

CommunicationOperationResult register_find_started_callback(
    CommunicationHandlerPtr handler, find_started callback, void *context)
{
    if (handler == NULL || handler->manager == NULL)
    {
        return COMMUNICATION_OPERATION_ERROR;
    }
    handler->_findStartedCallback = callback;
    handler->_findStartedContext = context;
    return handler->manager->registerFindStartedCallback(findStartedCbk,
                                                         handlers[handler->id]);
}

CommunicationOperationResult register_find_finished_callback(
    CommunicationHandlerPtr handler, find_finished callback, void *context)
{
    if (handler == NULL || handler->manager == NULL)
    {
        return COMMUNICATION_OPERATION_ERROR;
    }
    handler->_findFinishedCallback = callback;
    handler->_findFinishedContext = context;
    return handler->manager->registerFindFinishedCallback(findFinishedCbk,
                                                          handlers[handler->id]);
}

CommunicationOperationResult register_find_new_device_callback(
    CommunicationHandlerPtr handler, find_new_device callback, void *context)
{
    if (handler == NULL || handler->manager == NULL)
    {
        return COMMUNICATION_OPERATION_ERROR;
    }
    handler->_findNewDeviceCallback = callback;
    handler->_findNewDeviceContext = context;
    return handler->manager->registerFindNewDeviceCallback(findNewDeviceCbk,
                                                           handlers[handler->id]);
}

CommunicationOperationResult find(CommunicationHandlerPtr handler)
{
    if (handler == NULL || handler->manager == NULL)
    {
        return COMMUNICATION_OPERATION_ERROR;
    }
    return handler->manager->find();
}

CommunicationOperationResult set_target_hardware_id(
    CommunicationHandlerPtr handler, const char *target_id)
{
    if (handler == NULL || handler->manager == NULL)
    {
        return COMMUNICATION_OPERATION_ERROR;
    }
    return handler->manager->setTargetHardwareId(target_id);
}

CommunicationOperationResult set_target_hardware_pos(
    CommunicationHandlerPtr handler, const char *target_pos)
{
    if (handler == NULL || handler->manager == NULL)
    {
        return COMMUNICATION_OPERATION_ERROR;
    }
    return handler->manager->setTargetHardwarePosition(target_pos);
}

CommunicationOperationResult set_target_hardware_ip(
    CommunicationHandlerPtr handler, const char *target_ip)
{
    if (handler == NULL || handler->manager == NULL)
    {
        return COMMUNICATION_OPERATION_ERROR;
    }
    return handler->manager->setTargetHardwareIp(target_ip);
}

CommunicationOperationResult set_load_list(
    CommunicationHandlerPtr handler, Load *load_list, size_t load_list_size)
{
    return handler->manager->setLoadList(load_list, load_list_size);
}

CommunicationOperationResult register_upload_initialization_response_callback(
    CommunicationHandlerPtr handler, 
    upload_initialization_response_callback callback, void *context)
{
    if (handler == NULL || handler->manager == NULL)
    {
        return COMMUNICATION_OPERATION_ERROR;
    }
    handler->_uploadInitializationResponseCallback = callback;
    handler->_uploadInitializationResponseContext = context;
    return handler->manager->registerUploadInitializationResponseCallback(uploadInitializationResponseCbk,
                                                                          handlers[handler->id]);
}

CommunicationOperationResult register_upload_information_status_callback(
    CommunicationHandlerPtr handler, 
    upload_information_status_callback callback, void *context)
{
    if (handler == NULL || handler->manager == NULL)
    {
        return COMMUNICATION_OPERATION_ERROR;
    }
    handler->_uploadInformationStatusCallback = callback;
    handler->_uploadInformationStatusContext = context;
    return handler->manager->registerUploadInformationStatusCallback(uploadInformationStatusCbk,
                                                                     handlers[handler->id]);
}

CommunicationOperationResult register_file_not_available_callback(
    CommunicationHandlerPtr handler, 
    file_not_available_callback callback, void *context)
{
    if (handler == NULL || handler->manager == NULL)
    {
        return COMMUNICATION_OPERATION_ERROR;
    }
    handler->_fileNotAvailableCallback = callback;
    handler->_fileNotAvailableContext = context;
    return handler->manager->registerFileNotAvailableCallback(fileNotAvailableCbk,
                                                              handlers[handler->id]);
}

CommunicationOperationResult upload(CommunicationHandlerPtr handler)
{
    if (handler == NULL || handler->manager == NULL)
    {
        return COMMUNICATION_OPERATION_ERROR;
    }
    return handler->manager->upload();
}

CommunicationOperationResult abort_upload(
    CommunicationHandlerPtr handler, AbortSource abortSource)
{
    if (handler == NULL || handler->manager == NULL)
    {
        return COMMUNICATION_OPERATION_ERROR;
    }
    return handler->manager->abortUpload(abortSource);
}