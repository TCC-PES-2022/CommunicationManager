#include <gtest/gtest.h>

#include "communication_api.h"
#include "InitializationFileARINC615A.h"
#include "LoadUploadStatusFileARINC615A.h"
#include <cjson/cJSON.h>

#define DATALOADER_SERVER_PORT      5959
#define TARGETHARDWARE_SERVER_PORT  59595

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

        bcModulePid = 0;
    }

    void TearDown() override
    {
        destroy_handler(&handler);
        ASSERT_EQ(handler, nullptr);

        if (bcModulePid != 0)
        {
            kill(bcModulePid, SIGINT);
            waitpid(bcModulePid, NULL, 0);
        }
    }

    void startBLModule()
    {
        bcModulePid = fork();
        printf("\n\n bcModulePid = %d \n\n", bcModulePid);
        if (bcModulePid == 0)
        {
            char *args[] = {"blmodule", NULL};
            execv("blmodule", args);
            printf("Error starting B/L Module");
        }
        else if (bcModulePid < 0)
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
        strcpy(loads[0].loadName, "images/load1.bin");
        strcpy(loads[0].partNumber, "00000001");
        strcpy(loads[1].loadName, "images/load2.bin");
        strcpy(loads[1].partNumber, "00000002");
        strcpy(loads[2].loadName, "images/load3.bin");
        strcpy(loads[2].partNumber, "00000003");
        strcpy(loads[3].loadName, "images/ARQ_Compatibilidade.xml");
        strcpy(loads[3].partNumber, "00000000");

        set_load_list(handler, loads, 4);
    }

    void authenticate()
    {
        // TODO: implement
    }

    CommunicationHandlerPtr handler;
    pid_t bcModulePid;
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
    authenticate();

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
    authenticate();

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
    authenticate();

    CommunicationOperationResult result = upload(handler);
    ASSERT_EQ(result, COMMUNICATION_OPERATION_OK);
    ASSERT_TRUE(uploadSuccess);
}
