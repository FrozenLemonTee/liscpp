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

auto MalNil::to_string() const -> std::string {
    return "nil";
}

MalNil *MalNil::clone() const {
    return new MalNil(*this);
}

std::nullptr_t& MalNil::get_elem() {
    return this->val_;
}

MalNil::MalNil(std::nullptr_t val) : val_(val) {}

MalBool::MalBool(const bool val) : val_(val) {}

auto MalBool::to_string() const -> std::string {
    return val_ ? "true" : "false";
}

MalBool *MalBool::clone() const {
    return new MalBool(*this);
}

bool &MalBool::get_elem() {
    return this->val_;
}

MalInt::MalInt(const int64_t val) : val_(val) {}

auto MalInt::to_string() const -> std::string {
    return std::to_string(this->val_);
}

MalInt *MalInt::clone() const {
    return new MalInt(*this);
}

int64_t &MalInt::get_elem() {
    return this->val_;
}

MalString::MalString(const std::string& val) : val_(std::move(val.substr(1, val.length() - 2))) {}

auto MalString::to_string() const -> std::string {
    return "\"" + val_ + "\"";
}

MalString *MalString::clone() const {
    return new MalString(*this);
}

std::string &MalString::get_elem() {
    return this->val_;
}

MalSymbol::MalSymbol(std::string name) : name_(std::move(name)) {}

auto MalSymbol::to_string() const -> std::string {
    return name_;
}

MalSymbol *MalSymbol::clone() const {
    return new MalSymbol(*this);
}

std::string MalSymbol::name() const {
    return this->name_;
}

MalList::MalList(std::vector<MalType*> elements)
        : elements_(std::move(elements)) {}

auto MalList::to_string() const -> std::string {
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

MalList::MalList(std::initializer_list<MalType *> elements)
    : elements_(elements) {}

MalList *MalList::clone() const {
    return new MalList(*this);
}

std::vector<MalType *> &MalList::get_elem() {
    return this->elements_;
}

MalVector::MalVector(std::vector<MalType *> elements)
    : elements_(std::move(elements)) {}

auto MalVector::to_string() const -> std::string {
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

MalVector *MalVector::clone() const {
    return new MalVector(*this);
}

std::vector<MalType *> &MalVector::get_elem() {
    return this->elements_;
}

MalKeyword::MalKeyword(std::string name)
        : name_(std::move(name)) {}

auto MalKeyword::to_string() const -> std::string {
    return ":" + this->name_;
}

MalKeyword *MalKeyword::clone() const {
    return new MalKeyword(*this);
}

std::string MalKeyword::name() const {
    return this->name_;
}


MalMap::MalMap(const std::map<MalType*, MalType*>& elements)
        : elements_(elements) {}

auto MalMap::to_string() const -> std::string {
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

MalMap *MalMap::clone() const {
    return new MalMap(*this);
}

std::map<MalType *, MalType *> &MalMap::get_elem() {
    return this->elements_;
}

MalMetaData::MalMetaData(MalMap *map) : data_(map) {}

MalMetaData::~MalMetaData() {
    delete this->data_;
}

std::string MalMetaData::to_string() const {
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

MalMetaData *MalMetaData::clone() const {
    return new MalMetaData(*this);
}

MalSyntaxQuote::MalSyntaxQuote(MalType *expr) : expr_(expr) {}

MalSyntaxQuote::~MalSyntaxQuote() {
    delete this->expr_;
}

MalQuote::MalQuote(MalType *expr) : MalSyntaxQuote(expr) {}

std::string MalQuote::to_string() const {
    std::stringstream ss;
    ss << "(quote " << this->expr_->to_string() << ")";
    return ss.str();
}

MalQuote *MalQuote::clone() const {
    return new MalQuote(*this);
}

MalQuasiQuote::MalQuasiQuote(MalType *expr) : MalSyntaxQuote(expr) {}

std::string MalQuasiQuote::to_string() const {
    std::stringstream ss;
    ss << "(quasiquote " << this->expr_->to_string() << ")";
    return ss.str();
}

MalQuasiQuote *MalQuasiQuote::clone() const {
    return new MalQuasiQuote(*this);
}

MalUnQuote::MalUnQuote(MalType *expr) : MalSyntaxQuote(expr) {}

std::string MalUnQuote::to_string() const {
    std::stringstream ss;
    ss << "(unquote " << this->expr_->to_string() << ")";
    return ss.str();
}

MalUnQuote *MalUnQuote::clone() const {
    return new MalUnQuote(*this);
}

MalUnQuoteSplicing::MalUnQuoteSplicing(MalType *expr) : MalSyntaxQuote(expr) {}

std::string MalUnQuoteSplicing::to_string() const {
    std::stringstream ss;
    ss << "(splice-unquote " << this->expr_->to_string() << ")";
    return ss.str();
}

MalUnQuoteSplicing *MalUnQuoteSplicing::clone() const {
    return new MalUnQuoteSplicing(*this);
}

MalDeref::MalDeref(MalType *expr) : MalSyntaxQuote(expr) {}

std::string MalDeref::to_string() const {
    std::stringstream ss;
    ss << "(deref " << this->expr_->to_string() << ")";
    return ss.str();
}

MalDeref *MalDeref::clone() const {
    return new MalDeref(*this);
}

MalMetaSymbol::MalMetaSymbol(MalType *meta, MalType *value)
    : MalSyntaxQuote(nullptr), meta_(meta), value_(value) {}

MalType *MalMetaSymbol::get_meta() const {
    return this->meta_;
}

MalType *MalMetaSymbol::get_value() const {
    return this->value_;
}

std::string MalMetaSymbol::to_string() const {
    std::stringstream ss;
    ss << "(with-meta " << this->value_->to_string();
    ss << " " << this->meta_->to_string() << ")";
    return ss.str();
}

MalMetaSymbol::~MalMetaSymbol() {
    delete this->meta_;
    delete this->value_;
}

MalMetaSymbol *MalMetaSymbol::clone() const {
    return new MalMetaSymbol(*this);
}


MalFunction::MalFunction(std::function<MalType *(std::vector<MalType *>)> fn)
    : func_(std::move(fn)) {}

MalType *MalFunction::operator()(const std::vector<MalType *> &args) const {
    return this->func_(args);
}

MalType *MalFunction::apply(const std::vector<MalType*> &args) const {
    return (*this)(args);
}

MalFunction *MalFunction::clone() const {
    return new MalFunction(*this);
}

std::string MalFunction::to_string() const {
    return "#<function>";
}
