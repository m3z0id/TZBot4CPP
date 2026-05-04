#pragma once
#include <array>
#include <openssl/types.h>
#include <vector>
#include <string>

class EncryptionFactory {
public:
    constexpr static size_t KEY_LEN = 32;
private:
    constexpr static size_t IV_LEN  = 12;
    constexpr static size_t TAG_LEN = 16;

    const std::array<uint8_t, KEY_LEN> key;

    std::vector<uint8_t> encrypt(const std::vector<uint8_t>& plaintext, const std::vector<uint8_t>& aad, const EVP_CIPHER* cipher) const;
    std::vector<uint8_t> decrypt(const std::vector<uint8_t>& ciphertext, const std::vector<uint8_t>& aad, const EVP_CIPHER* cipher) const;
public:
    EncryptionFactory(const std::array<uint8_t, KEY_LEN>& cipherKey);

    [[nodiscard]] std::vector<uint8_t> AESEncrypt(const std::vector<uint8_t>& plaintext, const std::vector<uint8_t>& aad) const;
    [[nodiscard]] std::vector<uint8_t> AESDecrypt(const std::vector<uint8_t>& ciphertext, const std::vector<uint8_t>& aad) const;

    [[nodiscard]] std::vector<uint8_t> ChaCha20Encrypt(const std::vector<uint8_t>& plaintext, const std::vector<uint8_t>& aad) const;
    [[nodiscard]] std::vector<uint8_t> ChaCha20Decrypt(const std::vector<uint8_t>& ciphertext, const std::vector<uint8_t>& aad) const;
};
