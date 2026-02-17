#include "../include/TZRequest.h"

TZRequest::TZRequest(std::unique_ptr<ITZData> data) : data(std::move(data)) {}
TZRequest::~TZRequest() = default;

TZRequest::TZRequest(TZRequest&&) noexcept = default;
TZRequest &TZRequest::operator=(TZRequest &&) noexcept = default;

void TZRequest::to_json(nlohmann::json& json) const {
    nlohmann::json dataJson = {};
    data->to_json(dataJson);

    json = {};

    if (data->isAPIKeyNeeded()) {
        if (apiKey.empty()) throw std::invalid_argument("API key is required");
        json["apiKey"] = apiKey;
    }
    if (!dataJson.empty()) json["data"] = dataJson;
}
void TZRequest::setApiKey(const std::string& key) {
    this->apiKey = key;
}
uint8_t TZRequest::getRequestType() const {
    if (this->data == nullptr) throw std::invalid_argument("data is required");
    return this->data->getByteRepr();
}
