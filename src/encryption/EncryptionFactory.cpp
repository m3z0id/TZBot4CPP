#include "../../include/encryption/EncryptionFactory.h"

#include <cstring>
#include <openssl/evp.h>
#include <openssl/rand.h>
#include <stdexcept>

EncryptionFactory::EncryptionFactory(const std::string& key) {
    if (key.length() != 32) throw std::invalid_argument("Invalid key length");
    std::memcpy(this->key, key.data(), 32);
}

std::vector<uint8_t> EncryptionFactory::encrypt(const std::vector<uint8_t>& plaintext, const std::vector<uint8_t>& aad, const EVP_CIPHER* cipher) const {
    std::vector<uint8_t> out;
    out.resize(IV_LEN + TAG_LEN + plaintext.size());

    std::vector<uint8_t> iv(IV_LEN);
    std::vector<uint8_t> tag(TAG_LEN);
    std::vector<uint8_t> ciphertext(plaintext.size());

    RAND_bytes(iv.data(), IV_LEN);

    EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
    int len = 0;
    int ciphertextLen = 0;

    EVP_EncryptInit_ex(ctx, cipher, nullptr, nullptr, nullptr);
    EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_GCM_SET_IVLEN, IV_LEN, nullptr);
    EVP_EncryptInit_ex(ctx, nullptr, nullptr, key, iv.data());

    if (!aad.empty()) EVP_EncryptUpdate(ctx, nullptr, &len, aad.data(), static_cast<int>(aad.size()));

    EVP_EncryptUpdate(ctx, ciphertext.data(), &len, plaintext.data(), static_cast<int>(plaintext.size()));
    ciphertextLen = len;

    EVP_EncryptFinal_ex(ctx, ciphertext.data() + ciphertextLen, &len);
    ciphertextLen += len;

    EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_GCM_GET_TAG, TAG_LEN, tag.data());
    EVP_CIPHER_CTX_free(ctx);

    out.clear();
    out.reserve(IV_LEN + ciphertextLen + TAG_LEN);

    out.insert(out.end(), iv.begin(), iv.end());
    out.insert(out.end(), ciphertext.begin(), ciphertext.begin() + ciphertextLen);
    out.insert(out.end(), tag.begin(), tag.end());

    return out;
}

std::vector<uint8_t> EncryptionFactory::decrypt(const std::vector<uint8_t>& ciphertext, const std::vector<uint8_t>& aad, const EVP_CIPHER* cipher) const {
    if (ciphertext.size() < IV_LEN + TAG_LEN) throw std::runtime_error("Ciphertext too short");

    std::vector<uint8_t> out;
    out.resize(ciphertext.size() - IV_LEN - TAG_LEN);

    std::vector iv(ciphertext.begin(), ciphertext.begin() + IV_LEN);
    std::vector tag(ciphertext.end() - TAG_LEN, ciphertext.end());
    std::vector enc(ciphertext.begin() + IV_LEN, ciphertext.end() - TAG_LEN);
    out.resize(enc.size());

    EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
    int len = 0;
    int plaintextLen = 0;

    EVP_DecryptInit_ex(ctx, cipher, nullptr, nullptr, nullptr);
    EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_GCM_SET_IVLEN, IV_LEN, nullptr);
    EVP_DecryptInit_ex(ctx, nullptr, nullptr, key, iv.data());

    if (!aad.empty()) EVP_DecryptUpdate(ctx, nullptr, &len, aad.data(), static_cast<int>(aad.size()));

    EVP_DecryptUpdate(ctx, out.data(), &len, enc.data(), static_cast<int>(enc.size()));
    plaintextLen = len;

    EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_GCM_SET_TAG, TAG_LEN, tag.data());

    int ret = EVP_DecryptFinal_ex(ctx, out.data() + plaintextLen, &len);
    EVP_CIPHER_CTX_free(ctx);

    if (ret <= 0) throw std::runtime_error("Authentication failed");
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


