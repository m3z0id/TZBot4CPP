#include "EncryptionFactory.h"

#include <span>
#include <cstring>
#include <openssl/evp.h>
#include <openssl/rand.h>
#include <stdexcept>

EncryptionFactory::EncryptionFactory(const std::array<uint8_t, KEY_LEN>& cipherKey) : key(cipherKey) {}

std::vector<uint8_t> EncryptionFactory::encrypt(const std::vector<uint8_t>& plaintext, const std::vector<uint8_t>& aad, const EVP_CIPHER* cipher) const {
    std::vector<uint8_t> out;
    out.resize(IV_LEN + plaintext.size() + TAG_LEN);

    uint8_t* ivPtr = out.data();
    uint8_t* encPtr = out.data() + IV_LEN;
    uint8_t* tagPtr = out.data() + IV_LEN + plaintext.size();

    if (RAND_bytes(ivPtr, IV_LEN) <= 0) throw std::runtime_error("Random generation failed");

    EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
    if (!ctx) throw std::runtime_error("Failed to create CTX");

    int len = 0;
    int ciphertextLen = 0;

    EVP_EncryptInit_ex(ctx, cipher, nullptr, nullptr, nullptr);
    EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_GCM_SET_IVLEN, IV_LEN, nullptr);
    EVP_EncryptInit_ex(ctx, nullptr, nullptr, key.data(), ivPtr);

    if (!aad.empty()) EVP_EncryptUpdate(ctx, nullptr, &len, aad.data(), static_cast<int>(aad.size()));
    EVP_EncryptUpdate(ctx, encPtr, &len, plaintext.data(), static_cast<int>(plaintext.size()));
    ciphertextLen = len;

    EVP_EncryptFinal_ex(ctx, encPtr + ciphertextLen, &len);
    ciphertextLen += len;

    EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_GCM_GET_TAG, TAG_LEN, tagPtr);
    EVP_CIPHER_CTX_free(ctx);

    if (IV_LEN + ciphertextLen + TAG_LEN != out.size()) out.resize(IV_LEN + ciphertextLen + TAG_LEN);

    return out;
}

std::vector<uint8_t> EncryptionFactory::decrypt(const std::vector<uint8_t>& ciphertext, const std::vector<uint8_t>& aad, const EVP_CIPHER* cipher) const {
    if (ciphertext.size() < IV_LEN + TAG_LEN) throw std::runtime_error("Ciphertext too short");

    size_t encLen = ciphertext.size() - IV_LEN - TAG_LEN;
    std::array<uint8_t, IV_LEN> iv{};
    memcpy(iv.data(), ciphertext.data(), IV_LEN);

    std::array<uint8_t, TAG_LEN> tag{};
    memcpy(tag.data(), ciphertext.data() + ciphertext.size() - TAG_LEN, TAG_LEN);

    std::vector<uint8_t> out;
    out.resize(encLen);

    EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
    if (!ctx) throw std::runtime_error("Failed to create CTX");

    int len = 0;
    int plaintextLen = 0;

    EVP_DecryptInit_ex(ctx, cipher, nullptr, nullptr, nullptr);
    EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_GCM_SET_IVLEN, IV_LEN, nullptr);
    EVP_DecryptInit_ex(ctx, nullptr, nullptr, key.data(), iv.data());

    if (!aad.empty()) EVP_DecryptUpdate(ctx, nullptr, &len, aad.data(), static_cast<int>(aad.size()));

    const uint8_t* encPtr = ciphertext.data() + IV_LEN;
    if (!EVP_DecryptUpdate(ctx, out.data(), &len, encPtr, static_cast<int>(encLen))) {
        EVP_CIPHER_CTX_free(ctx);
        throw std::runtime_error("Decrypt step failed");
    }
    plaintextLen = len;

    EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_GCM_SET_TAG, TAG_LEN, tag.data());
    int ret = EVP_DecryptFinal_ex(ctx, out.data() + plaintextLen, &len);
    EVP_CIPHER_CTX_free(ctx);
    if (ret <= 0) throw std::runtime_error("Authentication failed (Integrity check failed)");

    out.resize(plaintextLen + len);
    return out;
}

std::vector<uint8_t> EncryptionFactory::AESEncrypt(const std::vector<uint8_t>& plaintext, const std::vector<uint8_t>& aad) const {
    return encrypt(plaintext, aad,EVP_aes_256_gcm());
}
std::vector<uint8_t> EncryptionFactory::AESDecrypt(const std::vector<uint8_t>& ciphertext, const std::vector<uint8_t>& aad) const {
    return decrypt(ciphertext, aad, EVP_aes_256_gcm());
}
std::vector<uint8_t> EncryptionFactory::ChaCha20Encrypt(const std::vector<uint8_t>& plaintext, const std::vector<uint8_t>& aad) const {
    return encrypt(plaintext, aad, EVP_chacha20_poly1305());
}
std::vector<uint8_t> EncryptionFactory::ChaCha20Decrypt(const std::vector<uint8_t>& ciphertext, const std::vector<uint8_t>& aad) const {
    return decrypt(ciphertext, aad, EVP_chacha20_poly1305());
}