#include <gtest/gtest.h>

#include "icommunicationmanager.h"

class CommunicationManagerBasicTest : public ::testing::Test
{
protected:
    CommunicationManagerBasicTest()
    {
    }

    ~CommunicationManagerBasicTest() override
    {
    }

    void SetUp() override
    {
        CommunicationOperationResult result = create_handler(&handler);
        ASSERT_EQ(result, COMMUNICATION_OPERATION_OK);
        ASSERT_NE(handler, nullptr);
    }

    void TearDown() override
    {
        destroy_handler(&handler);
        ASSERT_EQ(handler, nullptr);
    }

    CommunicationHandlerPtr handler;
};

TEST_F(CommunicationManagerBasicTest, RegisterFindStartedCallback)
{
    find_started callback = [](CommunicationHandlerPtr handler,
                               void *context)
    {
        return COMMUNICATION_OPERATION_OK;
    };
    void *context = nullptr;
    CommunicationOperationResult result = register_find_started_callback(handler, callback, context);
    ASSERT_EQ(result, COMMUNICATION_OPERATION_OK);
}

TEST_F(CommunicationManagerBasicTest, RegisterFindFinishedCallback)
{
    find_finished callback = [](CommunicationHandlerPtr handler,
                                void *context)
    {
        return COMMUNICATION_OPERATION_OK;
    };
    void *context = nullptr;
    CommunicationOperationResult result = register_find_finished_callback(handler, callback, context);
    ASSERT_EQ(result, COMMUNICATION_OPERATION_OK);
}

TEST_F(CommunicationManagerBasicTest, RegisterFindNewDeviceCallback)
{
    find_new_device callback = [](CommunicationHandlerPtr handler,
                                  const char *device,
                                  void *context)
    {
        return COMMUNICATION_OPERATION_OK;
    };
    void *context = nullptr;
    CommunicationOperationResult result = register_find_new_device_callback(handler, callback, context);
    ASSERT_EQ(result, COMMUNICATION_OPERATION_OK);
}

TEST_F(CommunicationManagerBasicTest, RegisterUploadInitializationResponseCallback)
{
    upload_initialization_response_callback callback = [](CommunicationHandlerPtr handler,
                                                          const char *response,
                                                          void *context)
    {
        return COMMUNICATION_OPERATION_OK;
    };
    void *context = nullptr;
    CommunicationOperationResult result = register_upload_initialization_response_callback(handler, callback, context);
    ASSERT_EQ(result, COMMUNICATION_OPERATION_OK);
}

TEST_F(CommunicationManagerBasicTest, RegisterUploadInformationStatusCallback)
{
    upload_information_status_callback callback = [](CommunicationHandlerPtr handler,
                                                     const char *status,
                                                     void *context)
    {
        return COMMUNICATION_OPERATION_OK;
    };
    void *context = nullptr;
    CommunicationOperationResult result = register_upload_information_status_callback(handler, callback, context);
    ASSERT_EQ(result, COMMUNICATION_OPERATION_OK);
}

TEST_F(CommunicationManagerBasicTest, RegisterUploadFileNotAvailableCallback)
{
    file_not_available_callback callback = [](CommunicationHandlerPtr handler,
                                              const char *file,
                                              unsigned short *wait_time_s,
                                              void *context)
    {
        return COMMUNICATION_OPERATION_OK;
    };
    void *context = nullptr;
    CommunicationOperationResult result = register_file_not_available_callback(handler, callback, context);
    ASSERT_EQ(result, COMMUNICATION_OPERATION_OK);
}