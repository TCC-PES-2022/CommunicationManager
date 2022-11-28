#include <gtest/gtest.h>

#include "icommunicationmanager.h"
#include "InitializationFileARINC615A.h"
#include "LoadUploadStatusFileARINC615A.h"
#include <cjson/cJSON.h>

#define DATALOADER_SERVER_PORT 5959
#define TARGETHARDWARE_SERVER_PORT 59595

class CommunicationManagerUploadTest : public ::testing::Test
{
protected:
    CommunicationManagerUploadTest()
    {
    }

    ~CommunicationManagerUploadTest() override
    {
    }

    void SetUp() override
    {
        CommunicationOperationResult result = create_handler(&handler);
        ASSERT_EQ(result, COMMUNICATION_OPERATION_OK);
        ASSERT_NE(handler, nullptr);

        set_tftp_dataloader_server_port(handler, DATALOADER_SERVER_PORT);
        set_tftp_targethardware_server_port(handler, TARGETHARDWARE_SERVER_PORT);

        blModulePid = 0;
    }

    void TearDown() override
    {
        destroy_handler(&handler);
        ASSERT_EQ(handler, nullptr);

        if (blModulePid != 0)
        {
            kill(blModulePid, SIGINT);
            waitpid(blModulePid, NULL, 0);
        }
    }

    void startBLModule()
    {
        blModulePid = fork();
        printf("blModulePid: %d\n", blModulePid);
        if (blModulePid == 0)
        {
            char *args[] = {"blmodule", NULL};
            setenv("LD_LIBRARY_PATH", "lib", 1);
            execv("blmodule", args);
            printf("Error starting B/L Module");
        }
        else if (blModulePid < 0)
        {
            printf("Error forking process");
        }
    }

    void configTargetHardware()
    {
        set_target_hardware_id(handler, "HNPFMS");
        set_target_hardware_pos(handler, "L");
        set_target_hardware_ip(handler, "127.0.0.1");
    }

    void setLoadList()
    {
        Load loads[4];
        strcpy(loads[0].loadName, "images/00000001_56.bin");
        strcpy(loads[0].partNumber, "00000001");
        strcpy(loads[1].loadName, "images/00000002_56.bin");
        strcpy(loads[1].partNumber, "00000002");
        strcpy(loads[2].loadName, "images/00000003_56.bin");
        strcpy(loads[2].partNumber, "00000003");
        strcpy(loads[3].loadName, "images/ARQ_Compatibilidade.xml");
        strcpy(loads[3].partNumber, "00000000");

        set_load_list(handler, loads, 4);
    }

    void setCertificate()
    {
        Certificate certificate;
        strcpy(certificate.certificatePath, "certificate/pescert.crt");
        set_certificate(handler, certificate);
    }

    CommunicationHandlerPtr handler;
    pid_t blModulePid;
};

TEST_F(CommunicationManagerUploadTest, RegisterUploadInitializationResponseCallback)
{
    upload_initialization_response_callback callback = [](CommunicationHandlerPtr handler,
                                                          const char *device,
                                                          void *context) -> CommunicationOperationResult
    {
        return COMMUNICATION_OPERATION_OK;
    };
    void *context = nullptr;
    CommunicationOperationResult result = register_upload_initialization_response_callback(handler, callback, context);
    ASSERT_EQ(result, COMMUNICATION_OPERATION_OK);
}

TEST_F(CommunicationManagerUploadTest, RegisterUploadInformationStatusCallback)
{
    upload_information_status_callback callback = [](CommunicationHandlerPtr handler,
                                                     const char *upload_information_status_json,
                                                     void *context) -> CommunicationOperationResult
    {
        return COMMUNICATION_OPERATION_OK;
    };
    void *context = nullptr;
    CommunicationOperationResult result = register_upload_information_status_callback(handler, callback, context);
    ASSERT_EQ(result, COMMUNICATION_OPERATION_OK);
}

TEST_F(CommunicationManagerUploadTest, RegisterFileNotAvailableCallback)
{
    file_not_available_callback callback = [](CommunicationHandlerPtr handler,
                                              const char *file_name,
                                              unsigned short *wait_time_s,
                                              void *context) -> CommunicationOperationResult
    {
        return COMMUNICATION_OPERATION_OK;
    };
    void *context = nullptr;
    CommunicationOperationResult result = register_file_not_available_callback(handler, callback, context);
    ASSERT_EQ(result, COMMUNICATION_OPERATION_OK);
}

TEST_F(CommunicationManagerUploadTest, UploadAccepted)
{
    bool uploadAccepted = false;
    startBLModule();

    upload_initialization_response_callback callback = [](CommunicationHandlerPtr handler,
                                                          const char *device,
                                                          void *context) -> CommunicationOperationResult
    {
        cJSON *json = cJSON_Parse(device);
        if (json == nullptr)
        {
            return COMMUNICATION_OPERATION_ERROR;
        }
        cJSON *jsonOperationAcceptanceStatusCode = cJSON_GetObjectItemCaseSensitive(json, "operationAcceptanceStatusCode");
        if (jsonOperationAcceptanceStatusCode == nullptr)
        {
            return COMMUNICATION_OPERATION_ERROR;
        }
        bool *uploadAccepted = (bool *)context;
        *uploadAccepted = jsonOperationAcceptanceStatusCode->valueint == INITIALIZATION_UPLOAD_IS_ACCEPTED;
        return COMMUNICATION_OPERATION_OK;
    };

    register_upload_initialization_response_callback(handler, callback, &uploadAccepted);

    configTargetHardware();
    setLoadList();
    setCertificate();

    upload(handler);
    ASSERT_TRUE(uploadAccepted);
}

TEST_F(CommunicationManagerUploadTest, StatusMessageReceived)
{
    bool statusMessageReceived = false;
    startBLModule();

    upload_information_status_callback callback = [](CommunicationHandlerPtr handler,
                                                     const char *upload_information_status_json,
                                                     void *context) -> CommunicationOperationResult
    {
        bool *statusMessageReceived = (bool *)context;
        *statusMessageReceived = true;
        return COMMUNICATION_OPERATION_OK;
    };

    register_upload_information_status_callback(handler, callback, &statusMessageReceived);

    configTargetHardware();
    setLoadList();
    setCertificate();

    upload(handler);
    ASSERT_TRUE(statusMessageReceived);
}

TEST_F(CommunicationManagerUploadTest, UploadSuccess)
{
    bool uploadSuccess = false;
    startBLModule();

    upload_information_status_callback callback = [](CommunicationHandlerPtr handler,
                                                     const char *upload_information_status_json,
                                                     void *context) -> CommunicationOperationResult
    {
        cJSON *json = cJSON_Parse(upload_information_status_json);
        if (json == nullptr)
        {
            return COMMUNICATION_OPERATION_ERROR;
        }
        cJSON *jsonOperationAcceptanceStatusCode = cJSON_GetObjectItemCaseSensitive(json, "uploadOperationStatusCode");
        if (jsonOperationAcceptanceStatusCode == nullptr)
        {
            return COMMUNICATION_OPERATION_ERROR;
        }
        bool *uploadSuccess = (bool *)context;
        uint16_t statusCode = jsonOperationAcceptanceStatusCode->valueint;
        *uploadSuccess = statusCode == STATUS_UPLOAD_COMPLETED;
        return COMMUNICATION_OPERATION_OK;
    };

    register_upload_information_status_callback(handler, callback, &uploadSuccess);

    configTargetHardware();
    setLoadList();
    setCertificate();

    CommunicationOperationResult result = upload(handler);
    ASSERT_EQ(result, COMMUNICATION_OPERATION_OK);
    ASSERT_TRUE(uploadSuccess);
}

TEST_F(CommunicationManagerUploadTest, UploadFailNoAuthentication)
{
    bool uploadSuccess = false;
    startBLModule();

    upload_information_status_callback callback = [](CommunicationHandlerPtr handler,
                                                     const char *upload_information_status_json,
                                                     void *context) -> CommunicationOperationResult
    {
        cJSON *json = cJSON_Parse(upload_information_status_json);
        if (json == nullptr)
        {
            return COMMUNICATION_OPERATION_ERROR;
        }
        cJSON *jsonOperationAcceptanceStatusCode = cJSON_GetObjectItemCaseSensitive(json, "uploadOperationStatusCode");
        if (jsonOperationAcceptanceStatusCode == nullptr)
        {
            return COMMUNICATION_OPERATION_ERROR;
        }
        bool *uploadSuccess = (bool *)context;
        uint16_t statusCode = jsonOperationAcceptanceStatusCode->valueint;
        *uploadSuccess = statusCode == STATUS_UPLOAD_COMPLETED;
        return COMMUNICATION_OPERATION_OK;
    };

    register_upload_information_status_callback(handler, callback, &uploadSuccess);

    configTargetHardware();
    setLoadList();

    // Do not set certificate so upload will fail
    // setCertificate();

    CommunicationOperationResult result = upload(handler);
    ASSERT_EQ(result, COMMUNICATION_OPERATION_ERROR);
    ASSERT_FALSE(uploadSuccess);
}

TEST_F(CommunicationManagerUploadTest, UploadFailTransmissionError)
{
    bool uploadAbortedByTargetHardware = false;
    startBLModule();

    upload_information_status_callback callback = [](CommunicationHandlerPtr handler,
                                                     const char *upload_information_status_json,
                                                     void *context) -> CommunicationOperationResult
    {
        cJSON *json = cJSON_Parse(upload_information_status_json);
        if (json == nullptr)
        {
            return COMMUNICATION_OPERATION_ERROR;
        }
        cJSON *jsonOperationAcceptanceStatusCode = cJSON_GetObjectItemCaseSensitive(json, "uploadOperationStatusCode");
        if (jsonOperationAcceptanceStatusCode == nullptr)
        {
            return COMMUNICATION_OPERATION_ERROR;
        }
        bool *uploadAbortedByTargetHardware = (bool *)context;
        uint16_t statusCode = jsonOperationAcceptanceStatusCode->valueint;
        *uploadAbortedByTargetHardware = statusCode == STATUS_UPLOAD_ABORTED_BY_THE_TARGET_HARDWARE;
        return COMMUNICATION_OPERATION_OK;
    };

    register_upload_information_status_callback(handler, callback, &uploadAbortedByTargetHardware);

    configTargetHardware();

    // Do not send compatibility file so upload will fail
    Load loads[4];
    strcpy(loads[0].loadName, "images/00000001_56.bin");
    strcpy(loads[0].partNumber, "00000001");
    strcpy(loads[1].loadName, "images/00000002_56.bin");
    strcpy(loads[1].partNumber, "00000002");
    strcpy(loads[2].loadName, "images/00000003_56.bin");
    strcpy(loads[2].partNumber, "00000003");
    set_load_list(handler, loads, 3);

    setCertificate();

    CommunicationOperationResult result = upload(handler);
    ASSERT_EQ(result, COMMUNICATION_OPERATION_ERROR);
    ASSERT_TRUE(uploadAbortedByTargetHardware);
}

TEST_F(CommunicationManagerUploadTest, UploadAbortedByOperator)
{
    bool uploadAbortedByOperator = false;
    CommunicationOperationResult abortResult;
    startBLModule();

    upload_information_status_callback callback = [](CommunicationHandlerPtr handler,
                                                     const char *upload_information_status_json,
                                                     void *context) -> CommunicationOperationResult
    {
        static bool sendAbort = true;
        cJSON *json = cJSON_Parse(upload_information_status_json);
        if (json == nullptr)
        {
            return COMMUNICATION_OPERATION_ERROR;
        }
        cJSON *jsonOperationAcceptanceStatusCode = cJSON_GetObjectItemCaseSensitive(json, "uploadOperationStatusCode");
        if (jsonOperationAcceptanceStatusCode == nullptr)
        {
            return COMMUNICATION_OPERATION_ERROR;
        }
        uint16_t statusCode = jsonOperationAcceptanceStatusCode->valueint;
        if (statusCode == STATUS_UPLOAD_IN_PROGRESS || statusCode == STATUS_UPLOAD_IN_PROGRESS_WITH_DESCRIPTION)
        {
            if (sendAbort)
            {
                //TODO: Sending more than once, the te TargetHardware will abort the operation, is this a bug?
                sendAbort = false;
                abort_upload(handler, OPERATION_ABORTED_BY_THE_OPERATOR);
            }
        }
        else
        {   
            bool *uploadAbortedByOperator = (bool *)context;
            *uploadAbortedByOperator = statusCode == STATUS_UPLOAD_ABORTED_IN_THE_TARGET_OP_REQUEST;
        }
        return COMMUNICATION_OPERATION_OK;
    };

    register_upload_information_status_callback(handler, callback, &uploadAbortedByOperator);

    configTargetHardware();
    Load loads[2];
    strcpy(loads[0].loadName, "images/00000004_20000036.bin");
    strcpy(loads[0].partNumber, "00000004");
    strcpy(loads[1].loadName, "images/ARQ_Compatibilidade.xml");
    strcpy(loads[1].partNumber, "00000000");
    set_load_list(handler, loads, 2);
    setCertificate();

    ASSERT_EQ(COMMUNICATION_OPERATION_ERROR, upload(handler));
    ASSERT_TRUE(uploadAbortedByOperator);
}