#pragma once
#include <openssl/types.h>
#include <vector>
#include <string>

class EncryptionFactory {
private:
    constexpr static size_t IV_LEN  = 12;
    constexpr static size_t TAG_LEN = 16;

    unsigned char key[32];

    std::vector<uint8_t> encrypt(const std::vector<uint8_t>& plaintext, const std::vector<uint8_t>& aad, const EVP_CIPHER* cipher) const;
    std::vector<uint8_t> decrypt(const std::vector<uint8_t>& ciphertext, const std::vector<uint8_t>& aad, const EVP_CIPHER* cipher) const;
public:
    EncryptionFactory(const std::string& key);

    [[nodiscard]] std::vector<uint8_t> AESEncrypt(const std::vector<uint8_t>& plaintext, const std::vector<uint8_t>& aad) const;
    [[nodiscard]] std::vector<uint8_t> AESDecrypt(const std::vector<uint8_t>& ciphertext, const std::vector<uint8_t>& aad) const;

    [[nodiscard]] std::vector<uint8_t> ChaCha20Encrypt(const std::vector<uint8_t>& plaintext, const std::vector<uint8_t>& aad) const;
    [[nodiscard]] std::vector<uint8_t> ChaCha20Decrypt(const std::vector<uint8_t>& ciphertext, const std::vector<uint8_t>& aad) const;
};
