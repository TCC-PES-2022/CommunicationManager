#include "AuthenticationManager.h"
#include <cjson/cJSON.h>
#include <gcrypt.h>
#include <algorithm>
#include <sstream>
#include <iomanip>

// If you change here, remember to change on BLModule
// (BLAuthenticator) as well.
#define KEY_SIZE 2048           // 2048 bits    
#define DATA_SIZE_FIELD_SIZE 4

AuthenticationManager::AuthenticationManager()
{
    cryptoContext = nullptr;

    authenticator = std::unique_ptr<AuthenticationDataLoader>(new AuthenticationDataLoader());
    authenticator->registerAuthenticationInitializationResponseCallback(
        authenticationInitializationResponseCbk, this);
    // authenticator->registerAuthenticationInformationStatusCallback(
    //     authenticationInformationStatusCbk, this);
    authenticator->registerAuthenticationLoadPrepare(
        loadPrepareCbk, this);
}

AuthenticationManager::~AuthenticationManager()
{
    if (authenticator != nullptr)
    {
        authenticator.reset();
    }
}

CommunicationOperationResult AuthenticationManager::setTftpDataLoaderServerPort(
    unsigned short port)
{
    return authenticator->setTftpDataLoaderServerPort(port) == AuthenticationOperationResult::AUTHENTICATION_OPERATION_OK
               ? COMMUNICATION_OPERATION_OK
               : COMMUNICATION_OPERATION_ERROR;
}

CommunicationOperationResult AuthenticationManager::setTftpTargetHardwareServerPort(
    unsigned short port)
{
    return authenticator->setTftpTargetHardwareServerPort(port) == AuthenticationOperationResult::AUTHENTICATION_OPERATION_OK
               ? COMMUNICATION_OPERATION_OK
               : COMMUNICATION_OPERATION_ERROR;
}

CommunicationOperationResult AuthenticationManager::setTargetHardwareId(
    const char *targetHardwareId)
{
    std::string targetHardwareIdStr(targetHardwareId);
    return authenticator->setTargetHardwareId(targetHardwareIdStr) == AuthenticationOperationResult::AUTHENTICATION_OPERATION_OK
               ? COMMUNICATION_OPERATION_OK
               : COMMUNICATION_OPERATION_ERROR;
}

CommunicationOperationResult AuthenticationManager::setTargetHardwarePosition(
    const char *targetHardwarePosition)
{
    std::string targetHardwarePositionStr(targetHardwarePosition);
    return authenticator->setTargetHardwarePosition(targetHardwarePositionStr) == AuthenticationOperationResult::AUTHENTICATION_OPERATION_OK
               ? COMMUNICATION_OPERATION_OK
               : COMMUNICATION_OPERATION_ERROR;
}

CommunicationOperationResult AuthenticationManager::setTargetHardwareIp(
    const char *targetHardwareIp)
{
    std::string targetHardwareIpStr(targetHardwareIp);
    return authenticator->setTargetHardwareIp(targetHardwareIpStr) == AuthenticationOperationResult::AUTHENTICATION_OPERATION_OK
               ? COMMUNICATION_OPERATION_OK
               : COMMUNICATION_OPERATION_ERROR;
}

CommunicationOperationResult AuthenticationManager::setCertificate(
    Certificate certificate)
{
    std::vector<AuthenticationLoad> loadList;
    loadList.clear();
    loadList.push_back(std::make_tuple(std::string(certificate.certificatePath), "0"));
    return authenticator->setLoadList(loadList) == AuthenticationOperationResult::AUTHENTICATION_OPERATION_OK
               ? COMMUNICATION_OPERATION_OK
               : COMMUNICATION_OPERATION_ERROR;
    return COMMUNICATION_OPERATION_OK;
}

AuthenticationOperationResult AuthenticationManager::loadPrepareCbk(
    std::string fileName,
    FILE **fp,
    size_t *bufferSize,
    void *context)
{
    if (context == NULL || fp == NULL || (*fp) == NULL || bufferSize == NULL)
    {
        return AuthenticationOperationResult::AUTHENTICATION_OPERATION_ERROR;
    }

    AuthenticationManager *thiz = (AuthenticationManager *)context;

    std::string hexFileContent;
    fseek(*fp, 0, SEEK_SET);
    while (!feof(*fp))
    {
        char hexChar[3];
        unsigned char byte;
        fread(&byte, 1, 1, *fp);
        sprintf(hexChar, "%02X", byte);
        hexFileContent += hexChar;
    }

    std::string asciiKey = "";
    for (size_t i = 0; i < thiz->cryptoContext->key.length(); i += 2)
    {
        std::string part = thiz->cryptoContext->key.substr(i, 2);
        char ch = stoul(part, nullptr, 16);
        asciiKey += ch;
    }

    /*
     * We can only encript data up to the key size. So we need to split the data
     * into chunks of key size and encrypt each chunk separately.
     * Each chunk will contain DATA_SIZE_FIELD_SIZE bytes for the chunk size 
     * and the chunk data, so the chunk size is 
     * KEY_SIZE_BYTES - 2 - DATA_SIZE_FIELD_SIZE.
     */
    thiz->cryptoContext->cypheredDataSize = 0;
    if (thiz->cryptoContext->cypheredData != NULL)
    {
        free(thiz->cryptoContext->cypheredData);
        thiz->cryptoContext->cypheredData = NULL;
    }
    size_t maxChunkSize = (KEY_SIZE / 8) - 2 - DATA_SIZE_FIELD_SIZE;
    uint8_t nchunks = (hexFileContent.length() / maxChunkSize) + 1;
    char **cypheredChunkData = (char **)malloc(sizeof(char *) * (nchunks));

    for (int i = 0; i < nchunks; ++i)
    {
        std::string chunk = hexFileContent.substr(i*maxChunkSize, maxChunkSize);

        gcry_error_t error;
        gcry_mpi_t r_mpi;
        if ((error = gcry_mpi_scan(&r_mpi, GCRYMPI_FMT_HEX, chunk.c_str(), 0, NULL)))
        {
            // printf("Error in gcry_mpi_scan() in encrypt(): %s\nSource: %s\n", gcry_strerror(error), gcry_strsource(error));
            return AuthenticationOperationResult::AUTHENTICATION_OPERATION_ERROR;
        }

        gcry_sexp_t data;
        size_t erroff;
        if ((error = gcry_sexp_build(&data, &erroff, "(data (flags raw) (value %m))", r_mpi)))
        {
            // printf("Error in gcry_sexp_build() in encrypt() at %ld: %s\nSource: %s\n", erroff, gcry_strerror(error), gcry_strsource(error));
            return AuthenticationOperationResult::AUTHENTICATION_OPERATION_ERROR;
        }

        gcry_sexp_t public_sexp;
        gcry_sexp_t r_ciph;
        if ((error = gcry_sexp_new(&public_sexp, asciiKey.c_str(), asciiKey.size(), 1)))
        {
            // printf("Error in sexp_new(%s): %s\nSource: %s\n", asciiKey.c_str(), gcry_strerror(error), gcry_strsource(error));
            return AuthenticationOperationResult::AUTHENTICATION_OPERATION_ERROR;
        }
        if ((error = gcry_pk_encrypt(&r_ciph, data, public_sexp)))
        {
            // printf("Error in gcry_pk_encrypt(): %s\nSource: %s\n", gcry_strerror(error), gcry_strsource(error));
            return AuthenticationOperationResult::AUTHENTICATION_OPERATION_ERROR;
        }

        size_t cypheredChunkSize = DATA_SIZE_FIELD_SIZE;
        cypheredChunkSize += gcry_sexp_sprint(r_ciph, GCRYSEXP_FMT_ADVANCED, NULL, 0);

        cypheredChunkData[i] = (char *)gcry_malloc(cypheredChunkSize);
        if (cypheredChunkData[i] == NULL)
        {
            // printf("gcry_malloc(%ld) returned NULL in sexp_string()!\n", thiz->cryptoContext->cypheredDataSize);
            return AuthenticationOperationResult::AUTHENTICATION_OPERATION_ERROR;
        }
        size_t effectiveCypheredChunkSize = cypheredChunkSize - DATA_SIZE_FIELD_SIZE;
        for (size_t j = 0, k = DATA_SIZE_FIELD_SIZE - 1; j < DATA_SIZE_FIELD_SIZE; j++, k--)
        {
            cypheredChunkData[i][k] = ((effectiveCypheredChunkSize >> (j * 8)) & 0xFF);
        }
        if (0 == gcry_sexp_sprint(r_ciph, GCRYSEXP_FMT_ADVANCED,
                                  cypheredChunkData[i] + DATA_SIZE_FIELD_SIZE,
                                  effectiveCypheredChunkSize))
        {
            // printf("gcry_sexp_sprint() lies!\n");
            return AuthenticationOperationResult::AUTHENTICATION_OPERATION_ERROR;
        }

        thiz->cryptoContext->cypheredDataSize += cypheredChunkSize;
    }

    // Merge chunks into a single buffer
    thiz->cryptoContext->cypheredData = (char *)malloc(thiz->cryptoContext->cypheredDataSize);
    if (thiz->cryptoContext->cypheredData == NULL)
    {
        // printf("malloc(%ld) returned NULL in sexp_string()!\n", thiz->cryptoContext->cypheredDataSize);
        return AuthenticationOperationResult::AUTHENTICATION_OPERATION_ERROR;
    }

    for (int i = 0, j = 0; i < nchunks; ++i)
    {
        //Read chunk size
        size_t cypheredChunkSize = 0;
        for (size_t k = 0; k < DATA_SIZE_FIELD_SIZE; k++)
        {
            // printf("\n%02x", cypheredChunkData[i][k]);
            cypheredChunkSize <<= 8;
            cypheredChunkSize |= cypheredChunkData[i][k];
        }
        cypheredChunkSize += DATA_SIZE_FIELD_SIZE;
        // printf("\ncypheredChunkSize: %ld\n", cypheredChunkSize);
        memcpy(thiz->cryptoContext->cypheredData + j, cypheredChunkData[i], cypheredChunkSize);
        j += cypheredChunkSize;
    }

    //Free chunks
    for (int i = 0; i < nchunks; ++i)
    {
        free(cypheredChunkData[i]);
    }
    free(cypheredChunkData);

    // Move file pointer to encrypted data
    fclose(*fp);
    *fp = fmemopen(thiz->cryptoContext->cypheredData, thiz->cryptoContext->cypheredDataSize, "r");
    *bufferSize = thiz->cryptoContext->cypheredDataSize;

    return AuthenticationOperationResult::AUTHENTICATION_OPERATION_OK;
}

AuthenticationOperationResult
AuthenticationManager::authenticationInitializationResponseCbk(
    std::string authenticationInitializationResponseJson,
    void *context)
{
    if (context == NULL)
    {
        return AuthenticationOperationResult::AUTHENTICATION_OPERATION_ERROR;
    }

    cJSON *root = cJSON_Parse(authenticationInitializationResponseJson.c_str());
    if (root == NULL)
    {
        return AuthenticationOperationResult::AUTHENTICATION_OPERATION_ERROR;
    }

    cJSON *fileName = cJSON_GetObjectItem(root, "fileName");
    if (fileName == NULL)
    {
        cJSON_Delete(root);
        return AuthenticationOperationResult::AUTHENTICATION_OPERATION_ERROR;
    }

    cJSON *cryptographicKey = cJSON_GetObjectItem(root, "cryptographicKey");
    if (cryptographicKey == NULL)
    {
        cJSON_Delete(root);
        return AuthenticationOperationResult::AUTHENTICATION_OPERATION_ERROR;
    }

    AuthenticationManager *thiz = (AuthenticationManager *)context;
    thiz->cryptoContext->key = std::string(cryptographicKey->valuestring);

    return AuthenticationOperationResult::AUTHENTICATION_OPERATION_OK;
}

CommunicationOperationResult
AuthenticationManager::registerCertificateNotAvailableCallback(
    certificateNotAvailableCallback callback, void *context)
{
    return authenticator->registerCertificateNotAvailableCallback(callback, context) == AuthenticationOperationResult::AUTHENTICATION_OPERATION_OK
               ? COMMUNICATION_OPERATION_OK
               : COMMUNICATION_OPERATION_ERROR;
}

CommunicationOperationResult AuthenticationManager::authenticate()
{

    cryptoContext = new CryptoContext();
    AuthenticationOperationResult result = authenticator->authenticate();
    delete cryptoContext;

    return (result == AuthenticationOperationResult::AUTHENTICATION_OPERATION_OK)
               ? COMMUNICATION_OPERATION_OK
               : COMMUNICATION_OPERATION_ERROR;
}

CommunicationOperationResult AuthenticationManager::abortAuthentication(
    AbortSource abortSource)
{
    uint16_t abortSourceInt = AUTHENTICATION_ABORT_SOURCE_NONE;
    switch (abortSource)
    {
    case OPERATION_ABORTED_BY_THE_DATALOADER:
        abortSourceInt = AUTHENTICATION_ABORT_SOURCE_DATALOADER;
        break;
    case OPERATION_ABORTED_BY_THE_OPERATOR:
        abortSourceInt = AUTHENTICATION_ABORT_SOURCE_OPERATOR;
        break;
    default:
        abortSourceInt = AUTHENTICATION_ABORT_SOURCE_NONE;
        break;
    }

    return authenticator->abort(abortSourceInt) == AuthenticationOperationResult::AUTHENTICATION_OPERATION_OK
               ? COMMUNICATION_OPERATION_OK
               : COMMUNICATION_OPERATION_ERROR;
}
