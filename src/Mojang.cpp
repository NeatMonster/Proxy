#include "Mojang.h"

#include "Encryption.h"

#include "json11/json11.hpp"
#include "polarssl/ctr_drbg.h"
#include "polarssl/ssl.h"
#include "polarssl/x509.h"

#include <cstring>
#include <iomanip>
#include <sstream>

bool Mojang::authentificate(Profile *profile, string_t username, string_t serverId,
                            ubytes_t sharedSecret, ubytes_t publicKey) {
    sha1_context ctx;
    sha1_init(&ctx);
    sha1_starts(&ctx);

    sha1_update(&ctx, (ubyte_t*) serverId.data(), serverId.size());
    sha1_update(&ctx, sharedSecret.data(), sharedSecret.size());
    sha1_update(&ctx, publicKey.data(), publicKey.size());

    ubyte_t hashData[20];
    sha1_finish(&ctx, hashData);

    bool negative = hashData[0] & 0x80;
    if (negative) {
        bool carry = true;
        for (int i = 19; i >= 0; i--) {
            hashData[i] = ~hashData[i];
            if (carry) {
                carry = hashData[i] == 0xff;
                hashData[i]++;
            }
        }
    }

    std::stringstream ss;
    if (negative)
        ss << "-";
    for (int i = 0; i < 20; i++)
        if (i > 0 || hashData[i] > 0x0f)
            ss << std::setw(2) << std::hex << std::setfill('0') << (int) hashData[i];
        else
            ss << std::setw(1) << std::hex << (int) hashData[i];
    std::cout << std::setfill(' ');

    sha1_free(&ctx);
    string_t hash = ss.str();

    ssl_context ssl;
    x509_crt cacert;

    std::memset(&ssl, 0, sizeof(ssl_context));
    x509_crt_init(&cacert);

    int ret, server_fd = -1;
    if ((ret = x509_crt_parse(&cacert, (ubyte_t*) certificates.data(), certificates.size())) != 0)
        throw SSLException("x509_crt_parse a retourné " + std::to_string(ret));

    if ((ret = net_connect(&server_fd, "sessionserver.mojang.com", 443)) != 0)
        throw SSLException("net_connect a retourné " + std::to_string(ret));

    if ((ret = ssl_init(&ssl)) != 0)
        throw SSLException("ssl_init a retourné " + std::to_string(ret));

    ssl_set_endpoint(&ssl, SSL_IS_CLIENT);
    ssl_set_authmode(&ssl, SSL_VERIFY_OPTIONAL);
    ssl_set_ca_chain(&ssl, &cacert, NULL, "*.mojang.com");

    ssl_set_rng(&ssl, ctr_drbg_random, Encryption::getRandom());
    ssl_set_dbg(&ssl, nullptr, stdout);
    ssl_set_bio(&ssl, net_recv, &server_fd, net_send, &server_fd);

    while ((ret = ssl_handshake(&ssl)) != 0)
        if (ret != POLARSSL_ERR_NET_WANT_READ && ret != POLARSSL_ERR_NET_WANT_WRITE)
            throw SSLException("ssl_handshake a retourné " + std::to_string(ret));

    if ((ret = ssl_get_verify_result(&ssl)) != 0) {
        if ((ret & BADCERT_EXPIRED) != 0)
            throw SSLException("le certificat du serveur a expiré");
        if ((ret & BADCERT_REVOKED) != 0)
           throw SSLException("le certificat du serveur a été révoqué");
        if ((ret & BADCERT_CN_MISMATCH) != 0)
            throw SSLException("le nom commun ne correspond pas (*.mojang.com attendu)");
        if ((ret & BADCERT_NOT_TRUSTED) != 0)
            throw SSLException("certificat auto-signé ou non signé par une autorité de certification");
    }

    ss.str("");
    ss << "GET /session/minecraft/hasJoined?username=" << username << "&serverId=" << hash << " HTTP/1.1\r\n";
    ss << "Host: sessionserver.mojang.com\r\n";
    ss << "Connection: close\r\n";
    ss << "\r\n";
    string_t req = ss.str();
    while ((ret = ssl_write(&ssl, (ubyte_t*) req.data(), req.size())) <= 0)
        if (ret != POLARSSL_ERR_NET_WANT_READ && ret != POLARSSL_ERR_NET_WANT_WRITE)
            throw SSLException("ssl_write a retourné " + std::to_string(ret));

    ss.str("");
    ubyte_t buf[1024];
    do {
        std::memset(buf, 0, sizeof(buf));
        ret = ssl_read(&ssl, buf, sizeof(buf) - 1);
        if (ret == POLARSSL_ERR_NET_WANT_READ || ret == POLARSSL_ERR_NET_WANT_WRITE)
            continue;
        if (ret == POLARSSL_ERR_SSL_PEER_CLOSE_NOTIFY || ret == 0)
            break;
        if (ret < 0)
            throw SSLException("ssl_read a retourné " + std::to_string(ret));
        ss << string_t(reinterpret_cast<char*>(buf));
    } while (true);
    string_t resp = ss.str();

    ssl_close_notify(&ssl);

    if (server_fd != -1)
        net_close(server_fd);

    x509_crt_free(&cacert);
    ssl_free(&ssl);

    std::memset(&ssl, 0, sizeof(ssl));

    if (resp.find("HTTP/1.1 200 OK") != string_t::npos) {
        string_t err;
        size_t start = resp.find("{");
        size_t end = resp.rfind("}");
        string_t body = resp.substr(start, 1 + end - start);
        json11::Json response = json11::Json::parse(body.data(), err);
        string_t uuid = string_t(response["id"].string_value());
        uuid.insert(uuid.begin() + 8, '-');
        uuid.insert(uuid.begin() + 13, '-');
        uuid.insert(uuid.begin() + 18, '-');
        uuid.insert(uuid.begin() + 23, '-');
        profile->uuid = uuid;
        profile->name = string_t(response["name"].string_value());
        for (const json11::Json &element : response["properties"].array_items()) {
            Profile::Property property;
            property.name = string_t(element["name"].string_value());
            property.value = string_t(element["value"].string_value());
            if (element["signature"].is_string()) {
                property.isSigned = true;
                property.signature = string_t(element["signature"].string_value());
            } else
                property.isSigned = false;
            profile->properties.push_back(property);
        }
        return true;
    }
    return false;
}

const string_t Mojang::certificates =
    "-----BEGIN CERTIFICATE-----\n"
    "MIID3TCCAsWgAwIBAgIBADANBgkqhkiG9w0BAQsFADCBjzELMAkGA1UEBhMCVVMx\n"
    "EDAOBgNVBAgTB0FyaXpvbmExEzARBgNVBAcTClNjb3R0c2RhbGUxJTAjBgNVBAoT\n"
    "HFN0YXJmaWVsZCBUZWNobm9sb2dpZXMsIEluYy4xMjAwBgNVBAMTKVN0YXJmaWVs\n"
    "ZCBSb290IENlcnRpZmljYXRlIEF1dGhvcml0eSAtIEcyMB4XDTA5MDkwMTAwMDAw\n"
    "MFoXDTM3MTIzMTIzNTk1OVowgY8xCzAJBgNVBAYTAlVTMRAwDgYDVQQIEwdBcml6\n"
    "b25hMRMwEQYDVQQHEwpTY290dHNkYWxlMSUwIwYDVQQKExxTdGFyZmllbGQgVGVj\n"
    "aG5vbG9naWVzLCBJbmMuMTIwMAYDVQQDEylTdGFyZmllbGQgUm9vdCBDZXJ0aWZp\n"
    "Y2F0ZSBBdXRob3JpdHkgLSBHMjCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoC\n"
    "ggEBAL3twQP89o/8ArFvW59I2Z154qK3A2FWGMNHttfKPTUuiUP3oWmb3ooa/RMg\n"
    "nLRJdzIpVv257IzdIvpy3Cdhl+72WoTsbhm5iSzchFvVdPtrX8WJpRBSiUZV9Lh1\n"
    "HOZ/5FSuS/hVclcCGfgXcVnrHigHdMWdSL5stPSksPNkN3mSwOxGXn/hbVNMYq/N\n"
    "Hwtjuzqd+/x5AJhhdM8mgkBj87JyahkNmcrUDnXMN/uLicFZ8WJ/X7NfZTD4p7dN\n"
    "dloedl40wOiWVpmKs/B/pM293DIxfJHP4F8R+GuqSVzRmZTRouNjWwl2tVZi4Ut0\n"
    "HZbUJtQIBFnQmA4O5t78w+wfkPECAwEAAaNCMEAwDwYDVR0TAQH/BAUwAwEB/zAO\n"
    "BgNVHQ8BAf8EBAMCAQYwHQYDVR0OBBYEFHwMMh+n2TB/xH1oo2Kooc6rB1snMA0G\n"
    "CSqGSIb3DQEBCwUAA4IBAQARWfolTwNvlJk7mh+ChTnUdgWUXuEok21iXQnCoKjU\n"
    "sHU48TRqneSfioYmUeYs0cYtbpUgSpIB7LiKZ3sx4mcujJUDJi5DnUox9g61DLu3\n"
    "4jd/IroAow57UvtruzvE03lRTs2Q9GcHGcg8RnoNAX3FWOdt5oUwF5okxBDgBPfg\n"
    "8n/Uqgr/Qh037ZTlZFkSIHc40zI+OIF1lnP6aI+xy84fxez6nH7PfrHxBy22/L/K\n"
    "pL/QlwVKvOoYKAKQvVR4CSFx09F9HdkWsKlhPdAKACL8x3vLCWRFCztAgfd9fDL1\n"
    "mMpYjn0q7pBZc2T5NnReJaH1ZgUufzkVqSr7UIuOhWn0\n"
    "-----END CERTIFICATE-----\n\n"
    "-----BEGIN CERTIFICATE-----\n"
    "MIIEDzCCAvegAwIBAgIBADANBgkqhkiG9w0BAQUFADBoMQswCQYDVQQGEwJVUzEl\n"
    "MCMGA1UEChMcU3RhcmZpZWxkIFRlY2hub2xvZ2llcywgSW5jLjEyMDAGA1UECxMp\n"
    "U3RhcmZpZWxkIENsYXNzIDIgQ2VydGlmaWNhdGlvbiBBdXRob3JpdHkwHhcNMDQw\n"
    "NjI5MTczOTE2WhcNMzQwNjI5MTczOTE2WjBoMQswCQYDVQQGEwJVUzElMCMGA1UE\n"
    "ChMcU3RhcmZpZWxkIFRlY2hub2xvZ2llcywgSW5jLjEyMDAGA1UECxMpU3RhcmZp\n"
    "ZWxkIENsYXNzIDIgQ2VydGlmaWNhdGlvbiBBdXRob3JpdHkwggEgMA0GCSqGSIb3\n"
    "DQEBAQUAA4IBDQAwggEIAoIBAQC3Msj+6XGmBIWtDBFk385N78gDGIc/oav7PKaf\n"
    "8MOh2tTYbitTkPskpD6E8J7oX+zlJ0T1KKY/e97gKvDIr1MvnsoFAZMej2YcOadN\n"
    "+lq2cwQlZut3f+dZxkqZJRRU6ybH838Z1TBwj6+wRir/resp7defqgSHo9T5iaU0\n"
    "X9tDkYI22WY8sbi5gv2cOj4QyDvvBmVmepsZGD3/cVE8MC5fvj13c7JdBmzDI1aa\n"
    "K4UmkhynArPkPw2vCHmCuDY96pzTNbO8acr1zJ3o/WSNF4Azbl5KXZnJHoe0nRrA\n"
    "1W4TNSNe35tfPe/W93bC6j67eA0cQmdrBNj41tpvi/JEoAGrAgEDo4HFMIHCMB0G\n"
    "A1UdDgQWBBS/X7fRzt0fhvRbVazc1xDCDqmI5zCBkgYDVR0jBIGKMIGHgBS/X7fR\n"
    "zt0fhvRbVazc1xDCDqmI56FspGowaDELMAkGA1UEBhMCVVMxJTAjBgNVBAoTHFN0\n"
    "YXJmaWVsZCBUZWNobm9sb2dpZXMsIEluYy4xMjAwBgNVBAsTKVN0YXJmaWVsZCBD\n"
    "bGFzcyAyIENlcnRpZmljYXRpb24gQXV0aG9yaXR5ggEAMAwGA1UdEwQFMAMBAf8w\n"
    "DQYJKoZIhvcNAQEFBQADggEBAAWdP4id0ckaVaGsafPzWdqbAYcaT1epoXkJKtv3\n"
    "L7IezMdeatiDh6GX70k1PncGQVhiv45YuApnP+yz3SFmH8lU+nLMPUxA2IGvd56D\n"
    "eruix/U0F47ZEUD0/CwqTRV/p2JdLiXTAAsgGh1o+Re49L2L7ShZ3U0WixeDyLJl\n"
    "xy16paq8U4Zt3VekyvggQQto8PT7dL5WXXp59fkdheMtlb71cZBDzI0fmgAKhynp\n"
    "VSJYACPq4xJDKVtHCN2MQWplBqjlIapBtJUhlbl90TSrE9atvNziPTnNvT51cKEY\n"
    "WQPJIrSPnNVeKtelttQKbfi3QBFGmh95DmK/D5fs4C8fF5Q=\n"
    "-----END CERTIFICATE-----\n";
