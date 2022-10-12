#include "CommunicationManager.h"

CommunicationManager::CommunicationManager()
{
    finder = std::unique_ptr<FindARINC615A>(new FindARINC615A());
    uploader = std::unique_ptr<UploadDataLoaderARINC615A>(new UploadDataLoaderARINC615A());
}

CommunicationManager::~CommunicationManager()
{
    if (finder != nullptr)
    {
        finder.reset();
    }
    if (uploader != nullptr)
    {
        uploader.reset();
    }
}

CommunicationOperationResult CommunicationManager::setTftpDataLoaderServerPort(
    unsigned short port)
{
    return uploader->setTftpDataLoaderServerPort(port) == UploadOperationResult::UPLOAD_OPERATION_OK
               ? COMMUNICATION_OPERATION_OK
               : COMMUNICATION_OPERATION_ERROR;
}

CommunicationOperationResult CommunicationManager::setTftpTargetHardwareServerPort(
    unsigned short port)
{
    return uploader->setTftpTargetHardwareServerPort(port) == UploadOperationResult::UPLOAD_OPERATION_OK
               ? COMMUNICATION_OPERATION_OK
               : COMMUNICATION_OPERATION_ERROR;
}

CommunicationOperationResult CommunicationManager::registerFindStartedCallback(
    findStarted callback, void *context)
{
    return finder->registerFindStartedCallback(callback, context) == FindOperationResult::FIND_OPERATION_OK
               ? COMMUNICATION_OPERATION_OK
               : COMMUNICATION_OPERATION_ERROR;
}

CommunicationOperationResult CommunicationManager::registerFindFinishedCallback(
    findFinished callback, void *context)
{
    return finder->registerFindFinishedCallback(callback, context) == FindOperationResult::FIND_OPERATION_OK
               ? COMMUNICATION_OPERATION_OK
               : COMMUNICATION_OPERATION_ERROR;
}

CommunicationOperationResult CommunicationManager::registerFindNewDeviceCallback(
    findNewDevice callback, void *context)
{
    return finder->registerFindNewDeviceCallback(callback, context) == FindOperationResult::FIND_OPERATION_OK
               ? COMMUNICATION_OPERATION_OK
               : COMMUNICATION_OPERATION_ERROR;
}

CommunicationOperationResult CommunicationManager::find()
{
    return finder->find() == FindOperationResult::FIND_OPERATION_OK ? COMMUNICATION_OPERATION_OK
                                                                    : COMMUNICATION_OPERATION_ERROR;
}

CommunicationOperationResult CommunicationManager::setTargetHardwareId(
    const char *targetHardwareId)
{
    std::string targetHardwareIdStr(targetHardwareId);
    return uploader->setTargetHardwareId(targetHardwareIdStr) == UploadOperationResult::UPLOAD_OPERATION_OK
               ? COMMUNICATION_OPERATION_OK
               : COMMUNICATION_OPERATION_ERROR;
}

CommunicationOperationResult CommunicationManager::setTargetHardwarePosition(
    const char *targetHardwarePosition)
{
    std::string targetHardwarePositionStr(targetHardwarePosition);
    return uploader->setTargetHardwarePosition(targetHardwarePositionStr) == UploadOperationResult::UPLOAD_OPERATION_OK
               ? COMMUNICATION_OPERATION_OK
               : COMMUNICATION_OPERATION_ERROR;
}

CommunicationOperationResult CommunicationManager::setTargetHardwareIp(
    const char *targetHardwareIp)
{
    std::string targetHardwareIpStr(targetHardwareIp);
    return uploader->setTargetHardwareIp(targetHardwareIpStr) == UploadOperationResult::UPLOAD_OPERATION_OK
               ? COMMUNICATION_OPERATION_OK
               : COMMUNICATION_OPERATION_ERROR;
}

CommunicationOperationResult CommunicationManager::setLoadList(
    Load *load_list, size_t load_list_size)
{
    std::vector<ArincLoad> loadList;
    loadList.clear();
    for (size_t i = 0; i < load_list_size; i++)
    {
        loadList.push_back(std::make_tuple(std::string(load_list[i].loadName),
                                           std::string(load_list[i].partNumber)));
    }
    return uploader->setLoadList(loadList) == UploadOperationResult::UPLOAD_OPERATION_OK
               ? COMMUNICATION_OPERATION_OK
               : COMMUNICATION_OPERATION_ERROR;
    return COMMUNICATION_OPERATION_OK;
}

CommunicationOperationResult
CommunicationManager::registerUploadInitializationResponseCallback(
    uploadInitializationResponseCallback callback, void *context)
{
    return uploader->registerUploadInitializationResponseCallback(callback, context) == UploadOperationResult::UPLOAD_OPERATION_OK
               ? COMMUNICATION_OPERATION_OK
               : COMMUNICATION_OPERATION_ERROR;
}

CommunicationOperationResult
CommunicationManager::registerUploadInformationStatusCallback(
    uploadInformationStatusCallback callback, void *context)
{
    return uploader->registerUploadInformationStatusCallback(callback, context) == UploadOperationResult::UPLOAD_OPERATION_OK
               ? COMMUNICATION_OPERATION_OK
               : COMMUNICATION_OPERATION_ERROR;
}

CommunicationOperationResult
CommunicationManager::registerFileNotAvailableCallback(
    fileNotAvailableCallback callback, void *context)
{
    return uploader->registerFileNotAvailableCallback(callback, context) == UploadOperationResult::UPLOAD_OPERATION_OK
               ? COMMUNICATION_OPERATION_OK
               : COMMUNICATION_OPERATION_ERROR;
}

CommunicationOperationResult CommunicationManager::upload()
{
    return uploader->upload() == UploadOperationResult::UPLOAD_OPERATION_OK
               ? COMMUNICATION_OPERATION_OK
               : COMMUNICATION_OPERATION_ERROR;
}

CommunicationOperationResult CommunicationManager::abortUpload(
    AbortSource abortSource)
{
    uint16_t abortSourceInt = UPLOAD_ABORT_SOURCE_NONE;
    switch (abortSource)
    {
    case OPERATION_ABORTED_BY_THE_DATALOADER:
        abortSourceInt = UPLOAD_ABORT_SOURCE_DATALOADER;
        break;
    case OPERATION_ABORTED_BY_THE_OPERATOR:
        abortSourceInt = UPLOAD_ABORT_SOURCE_OPERATOR;
        break;
    default:
        abortSourceInt = UPLOAD_ABORT_SOURCE_NONE;
        break;
    }

    return uploader->abort(abortSourceInt) == UploadOperationResult::UPLOAD_OPERATION_OK
               ? COMMUNICATION_OPERATION_OK
               : COMMUNICATION_OPERATION_ERROR;
}
