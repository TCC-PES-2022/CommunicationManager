#ifndef AUTHENTICATION_MANAGER_H
#define AUTHENTICATION_MANAGER_H

#include "communication_api.h"
#include "AuthenticationDataLoader.h"

class AuthenticationManager
{
public:
    AuthenticationManager();
    ~AuthenticationManager();

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

    /**
     * @brief Set TargetHardware ID. This is the ID of the TargetHardware to
     *        request authentication. You can get the TargetHardware's ID from the
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
     *        to request authentication. You can get the TargetHardware's
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
     *        to request authentication. You can get the TargetHardware's
     *        IP from the find operation.
     *
     * @param[in] targetHardwareIp the TargetHardware IP.
     *
     * @return COMMUNICATION_OPERATION_OK if success.
     * @return COMMUNICATION_OPERATION_ERROR otherwise.
     */
    CommunicationOperationResult setTargetHardwareIp(const char *targetHardwareIp);

    /**
     * @brief Set certificate. This is the certificate to be used to request
     *       authentication. Call this method before calling authenticate.
     *
     * @param[in] certificate the certificate.
     *
     * @return COMMUNICATION_OPERATION_OK if success.
     * @return COMMUNICATION_OPERATION_ERROR otherwise.
     */
    CommunicationOperationResult setCertificate(Certificate certificate);

    /**
     * Register a callback for authentication initialization response.
     *
     * @param[in] callback the callback.
     * @param[in] context the user context.
     *
     * @return COMMUNICATION_OPERATION_OK if success.
     * @return COMMUNICATION_OPERATION_ERROR otherwise.
     */
    // CommunicationOperationResult registerAuthenticationInitializationResponseCallback(
    //     authenticationInitializationResponseCallback callback,
    //     std::shared_ptr<void> context);

    /**
     * Register a callback for authentication information status.
     *
     * @param[in] callback the callback.
     * @param[in] context the user context.
     *
     * @return COMMUNICATION_OPERATION_OK if success.
     * @return COMMUNICATION_OPERATION_ERROR otherwise.
     */
    // CommunicationOperationResult registerAuthenticationInformationStatusCallback(
    //     authenticationInformationStatusCallback callback,
    //     std::shared_ptr<void> context);

    /**
     * Register a callback for certificate not available.
     *
     * @param[in] callback the callback.
     * @param[in] context the user context.
     *
     * @return COMMUNICATION_OPERATION_OK if success.
     * @return COMMUNICATION_OPERATION_ERROR otherwise.
     */
    CommunicationOperationResult registerCertificateNotAvailableCallback(
        certificateNotAvailableCallback callback,
        void *context);

    /**
     * @brief Start authentication operation. This method must called by the dataloader
     * to start the authentication to the target hardware.
     *
     * @return COMMUNICATION_OPERATION_OK if success.
     * @return COMMUNICATION_OPERATION_ERROR otherwise.
     */
    CommunicationOperationResult authenticate();

    /**
     * @brief Abort authentication operation.
     *
     * @param[in] abortSource the abort source.
     *
     * @return COMMUNICATION_OPERATION_OK if success.
     * @return COMMUNICATION_OPERATION_ERROR otherwise.
     */
    CommunicationOperationResult abortAuthentication(AbortSource abortSource);

private:
    std::unique_ptr<AuthenticationDataLoader> authenticator;

    class CryptoContext {
        public:
            CryptoContext()
            {
                key.clear();
                cypheredData = NULL;
            }
            ~CryptoContext()
            {
                key.clear();
                if (cypheredData != NULL)
                {
                    free(cypheredData);
                    cypheredData = NULL;
                }
            }

            std::string key;
            char *cypheredData;
            size_t cypheredDataSize;
    };
    CryptoContext *cryptoContext;

    static AuthenticationOperationResult authenticationInitializationResponseCbk(
        std::string authenticationInitializationResponseJson,
        void *context);

    static AuthenticationOperationResult authenticationInformationStatusCbk(
        std::string authenticationInformationStatusJson,
        void *context);

    static AuthenticationOperationResult loadPrepareCbk(
        std::string fileName,
        FILE **fp,
        size_t *bufferSize,
        void *context);
};

#endif // AUTHENTICATION_MANAGER_H
