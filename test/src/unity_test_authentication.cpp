#include <gtest/gtest.h>

#include "communication_api.h"
#include "AuthenticationManager.h"
#include <cjson/cJSON.h>

#define DATALOADER_SERVER_PORT 5959
#define TARGETHARDWARE_SERVER_PORT 59595

class CommunicationManagerAuthenticationTest : public ::testing::Test
{
protected:
    CommunicationManagerAuthenticationTest()
    {
        authenticator = new AuthenticationManager();
    }

    ~CommunicationManagerAuthenticationTest() override
    {
        delete authenticator;
    }

    void SetUp() override
    {
        authenticator->setTftpDataLoaderServerPort(DATALOADER_SERVER_PORT);
        authenticator->setTftpTargetHardwareServerPort(TARGETHARDWARE_SERVER_PORT);
        blModulePid = 0;
    }

    void TearDown() override
    {
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
        authenticator->setTargetHardwareId("HNPFMS");
        authenticator->setTargetHardwarePosition("L");
        authenticator->setTargetHardwareIp("127.0.0.1");
    }

    void setCertificate()
    {
        Certificate certificate;
        strcpy(certificate.certificatePath, "certificate/pescert.crt");
        authenticator->setCertificate(certificate);
    }

    AuthenticationManager *authenticator;
    pid_t blModulePid;
};

TEST_F(CommunicationManagerAuthenticationTest, AuthenticationSuccess)
{
    startBLModule();

    configTargetHardware();
    setCertificate();

    ASSERT_EQ(authenticator->authenticate(), COMMUNICATION_OPERATION_OK);
}

TEST_F(CommunicationManagerAuthenticationTest, AuthenticationFail)
{
    startBLModule();

    configTargetHardware();

    Certificate certificate;
    strcpy(certificate.certificatePath, "certificate/invalid_pescert.crt");
    authenticator->setCertificate(certificate);

    ASSERT_EQ(authenticator->authenticate(), COMMUNICATION_OPERATION_ERROR);
}