#include "communication_api.h"
#include "AuthenticationManager.h"
#include "CommunicationManager.h"

#include <time.h>

struct CommunicationHandler
{
    unsigned long id;
    CommunicationManager *communicationManager;
    AuthenticationManager *authenticationManager;

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
std::unordered_map<unsigned long, struct CommunicationHandler *> handlers;

static FindOperationResult findStartedCbk(
    void *context)
{
    auto handler = (struct CommunicationHandler *)context;
    if (handler->_findStartedCallback != nullptr)
    {
        handler->_findStartedCallback(handler,
                                      handler->_findStartedContext);
        return FindOperationResult::FIND_OPERATION_OK;
    }
    return FindOperationResult::FIND_OPERATION_ERROR;
}

static FindOperationResult findFinishedCbk(
    void *context)
{
    auto handler = (struct CommunicationHandler *)context;
    if (handler->_findFinishedCallback != nullptr)
    {
        handler->_findFinishedCallback(handler,
                                       handler->_findFinishedContext);
        return FindOperationResult::FIND_OPERATION_OK;
    }
    return FindOperationResult::FIND_OPERATION_ERROR;
}

static FindOperationResult findNewDeviceCbk(
    std::string device,
    void *context)
{
    auto handler = (struct CommunicationHandler *)context;
    if (handler->_findNewDeviceCallback != nullptr)
    {
        handler->_findNewDeviceCallback(handler,
                                        device.c_str(),
                                        handler->_findNewDeviceContext);
        return FindOperationResult::FIND_OPERATION_OK;
    }
    return FindOperationResult::FIND_OPERATION_ERROR;
}

static UploadOperationResult uploadInitializationResponseCbk(
    std::string uploadInitializationResponseJson,
    void *context)
{
    auto handler = (struct CommunicationHandler *)context;
    if (handler != nullptr && handler->_uploadInitializationResponseCallback != nullptr)
    {
        handler->_uploadInitializationResponseCallback(handler,
                                                       uploadInitializationResponseJson.c_str(),
                                                       handler->_uploadInitializationResponseContext);
        return UploadOperationResult::UPLOAD_OPERATION_OK;
    }
    return UploadOperationResult::UPLOAD_OPERATION_ERROR;
}

static UploadOperationResult uploadInformationStatusCbk(
    std::string uploadInformationStatusJson,
    void *context)
{
    auto handler = (struct CommunicationHandler *)context;
    if (handler != nullptr && handler->_uploadInformationStatusCallback != nullptr)
    {
        handler->_uploadInformationStatusCallback(handler,
                                                  uploadInformationStatusJson.c_str(),
                                                  handler->_uploadInformationStatusContext);
        return UploadOperationResult::UPLOAD_OPERATION_OK;
    }
    return UploadOperationResult::UPLOAD_OPERATION_ERROR;
}

static UploadOperationResult fileNotAvailableCbk(
    std::string fileName,
    uint16_t *waitTimeS,
    void *context)
{
    auto handler = (struct CommunicationHandler *)context;
    if (handler != nullptr && handler->_fileNotAvailableCallback != nullptr)
    {
        unsigned short waitTime = 0;
        handler->_fileNotAvailableCallback(handler,
                                           fileName.c_str(),
                                           &waitTime,
                                           handler->_fileNotAvailableContext);
        *waitTimeS = waitTime;
        return UploadOperationResult::UPLOAD_OPERATION_OK;
    }
    return UploadOperationResult::UPLOAD_OPERATION_ERROR;
}

static AuthenticationOperationResult certificateNotAvailableCbk(
    std::string fileName,
    uint16_t *waitTimeS,
    void *context)
{
    auto handler = (struct CommunicationHandler *)context;
    if (handler != nullptr && handler->_fileNotAvailableCallback != nullptr)
    {
        unsigned short waitTime = 0;
        handler->_fileNotAvailableCallback(handler,
                                           fileName.c_str(),
                                           &waitTime,
                                           handler->_fileNotAvailableContext);
        *waitTimeS = waitTime;
        return AuthenticationOperationResult::AUTHENTICATION_OPERATION_OK;
    }
    return AuthenticationOperationResult::AUTHENTICATION_OPERATION_ERROR;
}

CommunicationOperationResult create_handler(CommunicationHandlerPtr *handler)
{
    if (handler == NULL)
    {
        return COMMUNICATION_OPERATION_ERROR;
    }

    struct CommunicationHandler *newHandler = new CommunicationHandler();
    if (newHandler == NULL)
    {
        return COMMUNICATION_OPERATION_ERROR;
    }

    // Seems like a good enough id for now
    newHandler->id = (unsigned long)time(NULL);
    newHandler->communicationManager = new CommunicationManager();
    newHandler->authenticationManager = new AuthenticationManager();

    handlers[newHandler->id] = newHandler;

    *handler = newHandler;

    return COMMUNICATION_OPERATION_OK;
}

CommunicationOperationResult destroy_handler(CommunicationHandlerPtr *handler)
{
    if (handler == NULL)
    {
        return COMMUNICATION_OPERATION_ERROR;
    }

    delete (*handler)->communicationManager;
    delete (*handler)->authenticationManager;

    handlers.erase((*handler)->id);
    (*handler) = NULL;

    return COMMUNICATION_OPERATION_OK;
}

CommunicationOperationResult set_tftp_dataloader_server_port(
    CommunicationHandlerPtr handler, unsigned short port)
{
    if (handler == NULL || handler->communicationManager == NULL)
    {
        return COMMUNICATION_OPERATION_ERROR;
    }

    CommunicationOperationResult authenticationResult =
        handler->authenticationManager->setTftpDataLoaderServerPort(port);
    CommunicationOperationResult communicationResult =
        handler->communicationManager->setTftpDataLoaderServerPort(port);

    if (authenticationResult == COMMUNICATION_OPERATION_OK &&
        communicationResult == COMMUNICATION_OPERATION_OK)
    {
        return COMMUNICATION_OPERATION_OK;
    }

    return COMMUNICATION_OPERATION_ERROR;
}

CommunicationOperationResult set_tftp_targethardware_server_port(
    CommunicationHandlerPtr handler, unsigned short port)
{
    if (handler == NULL || handler->communicationManager == NULL)
    {
        return COMMUNICATION_OPERATION_ERROR;
    }

    CommunicationOperationResult authenticationResult =
        handler->authenticationManager->setTftpTargetHardwareServerPort(port);
    CommunicationOperationResult communicationResult =
        handler->communicationManager->setTftpTargetHardwareServerPort(port);

    if (authenticationResult == COMMUNICATION_OPERATION_OK &&
        communicationResult == COMMUNICATION_OPERATION_OK)
    {
        return COMMUNICATION_OPERATION_OK;
    }

    return COMMUNICATION_OPERATION_ERROR;
}

CommunicationOperationResult set_certificate(
    CommunicationHandlerPtr handler, Certificate certificate)
{
    if (handler == NULL || handler->authenticationManager == NULL)
    {
        return COMMUNICATION_OPERATION_ERROR;
    }

    handler->authenticationManager->setCertificate(certificate);

    return COMMUNICATION_OPERATION_OK;
}

CommunicationOperationResult register_find_started_callback(
    CommunicationHandlerPtr handler, find_started callback, void *context)
{
    if (handler == NULL || handler->communicationManager == NULL)
    {
        return COMMUNICATION_OPERATION_ERROR;
    }
    handler->_findStartedCallback = callback;
    handler->_findStartedContext = context;
    return handler->communicationManager->registerFindStartedCallback(findStartedCbk,
                                                                      handlers[handler->id]);
}

CommunicationOperationResult register_find_finished_callback(
    CommunicationHandlerPtr handler, find_finished callback, void *context)
{
    if (handler == NULL || handler->communicationManager == NULL)
    {
        return COMMUNICATION_OPERATION_ERROR;
    }
    handler->_findFinishedCallback = callback;
    handler->_findFinishedContext = context;
    return handler->communicationManager->registerFindFinishedCallback(findFinishedCbk,
                                                                       handlers[handler->id]);
}

CommunicationOperationResult register_find_new_device_callback(
    CommunicationHandlerPtr handler, find_new_device callback, void *context)
{
    if (handler == NULL || handler->communicationManager == NULL)
    {
        return COMMUNICATION_OPERATION_ERROR;
    }
    handler->_findNewDeviceCallback = callback;
    handler->_findNewDeviceContext = context;
    return handler->communicationManager->registerFindNewDeviceCallback(findNewDeviceCbk,
                                                                        handlers[handler->id]);
}

CommunicationOperationResult find(CommunicationHandlerPtr handler)
{
    if (handler == NULL ||
        // handler->authenticationManager == NULL ||
        handler->communicationManager == NULL)
    {
        return COMMUNICATION_OPERATION_ERROR;
    }
    // if (handler->authenticationManager->authenticate() == COMMUNICATION_OPERATION_OK)
    // {
    return handler->communicationManager->find();
    // }
    // return COMMUNICATION_OPERATION_ERROR;
}

CommunicationOperationResult set_target_hardware_id(
    CommunicationHandlerPtr handler, const char *target_id)
{
    if (handler == NULL || handler->communicationManager == NULL)
    {
        return COMMUNICATION_OPERATION_ERROR;
    }
    CommunicationOperationResult authenticationResult =
        handler->authenticationManager->setTargetHardwareId(target_id);
    CommunicationOperationResult communicationResult =
        handler->communicationManager->setTargetHardwareId(target_id);

    if (authenticationResult == COMMUNICATION_OPERATION_OK &&
        communicationResult == COMMUNICATION_OPERATION_OK)
    {
        return COMMUNICATION_OPERATION_OK;
    }
    return COMMUNICATION_OPERATION_ERROR;
}

CommunicationOperationResult set_target_hardware_pos(
    CommunicationHandlerPtr handler, const char *target_pos)
{
    if (handler == NULL || handler->communicationManager == NULL)
    {
        return COMMUNICATION_OPERATION_ERROR;
    }
    CommunicationOperationResult authenticationResult =
        handler->authenticationManager->setTargetHardwarePosition(target_pos);
    CommunicationOperationResult communicationResult =
        handler->communicationManager->setTargetHardwarePosition(target_pos);

    if (authenticationResult == COMMUNICATION_OPERATION_OK &&
        communicationResult == COMMUNICATION_OPERATION_OK)
    {
        return COMMUNICATION_OPERATION_OK;
    }
    return COMMUNICATION_OPERATION_ERROR;
}

CommunicationOperationResult set_target_hardware_ip(
    CommunicationHandlerPtr handler, const char *target_ip)
{
    if (handler == NULL || handler->communicationManager == NULL)
    {
        return COMMUNICATION_OPERATION_ERROR;
    }
    CommunicationOperationResult authenticationResult =
        handler->authenticationManager->setTargetHardwareIp(target_ip);
    CommunicationOperationResult communicationResult =
        handler->communicationManager->setTargetHardwareIp(target_ip);

    if (authenticationResult == COMMUNICATION_OPERATION_OK &&
        communicationResult == COMMUNICATION_OPERATION_OK)
    {
        return COMMUNICATION_OPERATION_OK;
    }
    return COMMUNICATION_OPERATION_ERROR;
}

CommunicationOperationResult set_load_list(
    CommunicationHandlerPtr handler, Load *load_list, size_t load_list_size)
{
    return handler->communicationManager->setLoadList(load_list, load_list_size);
}

CommunicationOperationResult register_upload_initialization_response_callback(
    CommunicationHandlerPtr handler,
    upload_initialization_response_callback callback, void *context)
{
    if (handler == NULL || handler->communicationManager == NULL)
    {
        return COMMUNICATION_OPERATION_ERROR;
    }
    handler->_uploadInitializationResponseCallback = callback;
    handler->_uploadInitializationResponseContext = context;
    return handler->communicationManager->registerUploadInitializationResponseCallback(uploadInitializationResponseCbk,
                                                                                       handlers[handler->id]);
}

CommunicationOperationResult register_upload_information_status_callback(
    CommunicationHandlerPtr handler,
    upload_information_status_callback callback, void *context)
{
    if (handler == NULL || handler->communicationManager == NULL)
    {
        return COMMUNICATION_OPERATION_ERROR;
    }
    handler->_uploadInformationStatusCallback = callback;
    handler->_uploadInformationStatusContext = context;
    return handler->communicationManager->registerUploadInformationStatusCallback(uploadInformationStatusCbk,
                                                                                  handlers[handler->id]);
}

CommunicationOperationResult register_file_not_available_callback(
    CommunicationHandlerPtr handler,
    file_not_available_callback callback, void *context)
{
    if (handler == NULL || handler->communicationManager == NULL)
    {
        return COMMUNICATION_OPERATION_ERROR;
    }
    handler->_fileNotAvailableCallback = callback;
    handler->_fileNotAvailableContext = context;
    CommunicationOperationResult authenticationResult =
        handler->authenticationManager->registerCertificateNotAvailableCallback(certificateNotAvailableCbk,
                                                                                handlers[handler->id]);
    CommunicationOperationResult communicationResult =
        handler->communicationManager->registerFileNotAvailableCallback(fileNotAvailableCbk,
                                                                        handlers[handler->id]);

    if (authenticationResult == COMMUNICATION_OPERATION_OK &&
        communicationResult == COMMUNICATION_OPERATION_OK)
    {
        return COMMUNICATION_OPERATION_OK;
    }
    return COMMUNICATION_OPERATION_ERROR;
}

CommunicationOperationResult upload(CommunicationHandlerPtr handler)
{
    if (handler == NULL ||
        handler->authenticationManager == NULL ||
        handler->communicationManager == NULL)
    {
        return COMMUNICATION_OPERATION_ERROR;
    }
    if (handler->authenticationManager->authenticate() == COMMUNICATION_OPERATION_OK)
    {
        return handler->communicationManager->upload();
    }
    return COMMUNICATION_OPERATION_ERROR;
}

CommunicationOperationResult abort_upload(
    CommunicationHandlerPtr handler, AbortSource abortSource)
{
    if (handler == NULL ||
        handler->authenticationManager == NULL ||
        handler->communicationManager == NULL)
    {
        return COMMUNICATION_OPERATION_ERROR;
    }

    CommunicationOperationResult authenticationReturn = handler->authenticationManager->abortAuthentication(abortSource);
    CommunicationOperationResult uploadReturn = handler->communicationManager->abortUpload(abortSource);

    if (authenticationReturn == COMMUNICATION_OPERATION_OK ||
        uploadReturn == COMMUNICATION_OPERATION_OK)
    {
        return COMMUNICATION_OPERATION_OK;
    }
    return COMMUNICATION_OPERATION_ERROR;
}