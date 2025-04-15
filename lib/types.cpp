#include "types.h"

#include <iomanip>
#include <regex>
#include <utility>

auto MalType::isKeyword(const std::string &token) -> bool {
    return token[0] == ':';
}

auto MalType::isInt(const std::string& token) -> bool {
    return std::regex_match(token, std::regex("^[-+]?[0-9]{1,19}$"));
}

auto MalType::isNil(const std::string& token) -> bool {
    return token == "nil";
}

auto MalType::isBool(const std::string& token) -> bool {
    return token == "true" || token == "false";
}

auto MalType::isString(const std::string &token) -> bool {
    if (token.length() < 2) {
        return false;
    }

    if (token[0] != '\"' || token[token.length() - 1] != '\"') {
        return false;
    }

    const std::string content = token.substr(1, token.length() - 2);

    for (size_t i = 0; i < content.length(); ++i) {
        if (content[i] == '\\') {
            if (i == content.length() - 1 ||
                (content[i + 1] != 'n' && content[i + 1] != 't' && content[i + 1] != '\\' &&
                 content[i + 1] != '\"' && content[i + 1] != 'r')) {
                return false;
            }
            ++i;
        }
    }

    return true;
}

MalType::~MalType() {
    delete this->meta_;
}

auto MalNil::to_string() -> std::string {
    return "nil";
}

MalBool::MalBool(const bool val) : val_(val) {}

auto MalBool::to_string() -> std::string {
    return val_ ? "true" : "false";
}

MalInt::MalInt(const int64_t val) : val_(val) {}

auto MalInt::to_string() -> std::string {
    return std::to_string(this->val_);
}

MalString::MalString(const std::string& val) : val_(std::move(val.substr(1, val.length() - 2))) {}

auto MalString::to_string() -> std::string {
    return "\"" + val_ + "\"";
}

MalSymbol::MalSymbol(std::string name) : name_(std::move(name)) {}

auto MalSymbol::to_string() -> std::string {
    return name_;
}

MalList::MalList(std::vector<MalType*> elements)
        : elements_(std::move(elements)) {}

auto MalList::to_string() -> std::string {
    std::stringstream ss;
    ss << "(";
    bool first = true;
    for (const auto& element : elements_) {
        if (!first) {
            ss << " ";
        }
        ss << element->to_string();
        first = false;
    }
    ss << ")";
    return ss.str();
}

MalList::~MalList() {
    for (const auto& e: this->elements_) {
        delete e;
    }
}

MalVector::MalVector(std::vector<MalType *> elements)
        : elements_(std::move(elements)) {}

auto MalVector::to_string() -> std::string {
    std::stringstream ss;
    ss << "[";
    bool first = true;
    for (const auto& element : elements_) {
        if (!first) {
            ss << " ";
        }
        ss << element->to_string();
        first = false;
    }
    ss << "]";
    return ss.str();
}

MalVector::~MalVector() {
    for (const auto& e: this->elements_) {
        delete e;
    }
}

MalKeyword::MalKeyword(std::string name)
        : name_(std::move(name)) {}

auto MalKeyword::to_string() -> std::string {
    return ":" + this->name_;
}


MalMap::MalMap(const std::map<MalType*, MalType*>& elements)
        : elements_(elements) {}

auto MalMap::to_string() -> std::string {
    std::stringstream ss;
    ss << "{";
    bool first = true;
    for (const auto& [key, val] : elements_) {
        if (!first) {
            ss << " ";
        }
        ss << key->to_string() << " " << val->to_string();
        first = false;
    }
    ss << "}";
    return ss.str();
}

MalMap::~MalMap() {
    for (const auto& e: this->elements_) {
        delete e.first;
        delete e.second;
    }
}

MalMetaData::MalMetaData(MalMap *map) : data_(map) {}

MalMetaData::~MalMetaData() {
    delete this->data_;
}

std::string MalMetaData::to_string() {
    return std::string();
}

MalMap *MalMetaData::get_map() {
    return nullptr;
}

MalType *MalMetaData::get(MalType *key) {
    return key; // todo
}

void MalMetaData::put(MalType *key, MalType *val) {
    if (key && val) { // todo
    }
}

MalSyntaxQuote::MalSyntaxQuote(MalType *expr) : expr_(expr) {}

MalSyntaxQuote::~MalSyntaxQuote() {
    delete this->expr_;
}

MalQuote::MalQuote(MalType *expr) : MalSyntaxQuote(expr) {}

std::string MalQuote::to_string() {
    std::stringstream ss;
    ss << "(quote " << this->expr_->to_string() << ")";
    return ss.str();
}

MalQuasiQuote::MalQuasiQuote(MalType *expr) : MalSyntaxQuote(expr) {}

std::string MalQuasiQuote::to_string() {
    std::stringstream ss;
    ss << "(quasiquote " << this->expr_->to_string() << ")";
    return ss.str();
}

MalUnQuote::MalUnQuote(MalType *expr) : MalSyntaxQuote(expr) {}

std::string MalUnQuote::to_string() {
    std::stringstream ss;
    ss << "(unquote " << this->expr_->to_string() << ")";
    return ss.str();
}

MalUnQuoteSplicing::MalUnQuoteSplicing(MalType *expr) : MalSyntaxQuote(expr) {}

std::string MalUnQuoteSplicing::to_string() {
    std::stringstream ss;
    ss << "(unquote-splicing " << this->expr_->to_string() << ")";
    return ss.str();
}
