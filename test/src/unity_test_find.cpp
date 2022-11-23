#include <gtest/gtest.h>
#include <fstream>

#include "icommunicationmanager.h"

#define FINDSTUB_FILE "findstub.json"
#define FIND_STUB_CONTENT   "{\n"                                                   \
                            "  \"devices\": [\n"                                    \
                            "    {\n"                                               \
                            "      \"mac\": \"9A-DA-C1-D7-51-D7\",\n"               \
                            "      \"ip\": \"127.0.0.1\",\n"                        \
                            "      \"hardware\": {\n"                               \
                            "           \"targetHardwareIdentifier\": \"HNPFMS\",\n"\
                            "           \"targetTypeName\": \"FMS\",\n"             \
                            "           \"targetPosition\": \"L\",\n"               \
                            "           \"literalName\": \"FMS LEFT\",\n"           \
                            "           \"manufacturerCode\": \"HNP\"\n"            \
                            "       }\n"                                            \
                            "    }\n"                                               \
                            "  ]\n"                                                 \
                            "}"

#define DEVICE_INFO "{\"mac\":\"9A-DA-C1-D7-51-D7\",\"ip\":\"127.0.0.1\","   \
                    "\"hardware\":{\"targetHardwareIdentifier\":\"HNPFMS\"," \
                    "\"targetTypeName\":\"FMS\",\"targetPosition\":\"L\","   \
                    "\"literalName\":\"FMS LEFT\",\"manufacturerCode\":\"HNP\"}}"

class CommunicationManagerFindTest : public ::testing::Test
{
protected:
    CommunicationManagerFindTest()
    {
    }

    ~CommunicationManagerFindTest() override
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

void createFindStub() {
    std::ofstream findstub(FINDSTUB_FILE);
    if (findstub.is_open()) {
        findstub << FIND_STUB_CONTENT;
        findstub.close();
    }
}

TEST_F(CommunicationManagerFindTest, FindDeviceTest)
{
    std::string *deviceInfo = nullptr;
    createFindStub();
    find_new_device callback = [](CommunicationHandlerPtr handler,
                                  const char *device,
                                  void *context)
    {
        std::string **deviceInfo = (std::string **)context;
        *deviceInfo = new std::string(device);
        return COMMUNICATION_OPERATION_OK;
    };
    CommunicationOperationResult result = register_find_new_device_callback(handler, callback, &deviceInfo);
    ASSERT_EQ(find(handler), COMMUNICATION_OPERATION_OK);
    ASSERT_STREQ(deviceInfo->c_str(), DEVICE_INFO);

    if (deviceInfo != nullptr) {
        delete deviceInfo;
    }
}