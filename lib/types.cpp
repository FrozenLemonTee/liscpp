#include "types.h"
#include "env.h"
#include "error.h"
#include "evaluator.h"
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

    for (std::size_t i = 0; i < content.length(); ++i) {
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

bool MalNil::equal(const MalType* type) const {
    auto other_nil = dynamic_cast<const MalNil*>(type);
    return other_nil;
}

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

bool MalBool::equal(const MalType *type) const {
    auto other_bool = dynamic_cast<const MalBool*>(type);
    return other_bool && this->val_ == other_bool->val_;
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

bool MalInt::equal(const MalType *type) const {
    auto other_int = dynamic_cast<const MalInt*>(type);
    return other_int && this->val_ == other_int->val_;
}

MalString::MalString(const std::string& val) {
    if (val.length() >= 2 && val.front() == '"' && val.back() == '"') {
        val_ = val.substr(1, val.length() - 2);
    } else {
        val_ = val;
    }
}

auto MalString::to_string() const -> std::string {
    return "\"" + val_ + "\"";
}

MalString *MalString::clone() const {
    return new MalString(*this);
}

std::string &MalString::get_elem() {
    return this->val_;
}

bool MalString::equal(const MalType *type) const {
    auto other_str = dynamic_cast<const MalString*>(type);
    return other_str && this->val_ == other_str->val_;
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

bool MalSymbol::equal(const MalType *type) const {
    auto other_symbol = dynamic_cast<const MalSymbol*>(type);
    return other_symbol && this->name_ == other_symbol->name_;
}

MalSequence::MalSequence(std::vector<MalType *> elements)
    : elements_(std::move(elements)) {}

MalSequence::MalSequence(std::initializer_list<MalType *> elements)
    : elements_(elements) {}

std::vector<MalType *> &MalSequence::get_elem() {
    return this->elements_;
}

std::string MalSequence::to_string() const {
    std::stringstream ss;
    bool first = true;
    for (const auto& element : this->elements_) {
        if (!first) {
            ss << " ";
        }
        ss << element->to_string();
        first = false;
    }
    return ss.str();
}

MalSequence::~MalSequence() {
    for (const auto& e: this->elements_) {
        delete e;
    }
}

std::vector<MalType *> MalSequence::elem_clone() const {
    std::vector<MalType*> copied;
    for (auto* e : elements_) {
        copied.push_back(e->clone());
    }
    return copied;
}

MalList::MalList(std::vector<MalType*> elements)
        : MalSequence(std::move(elements)) {}

auto MalList::to_string() const -> std::string {
    std::stringstream ss;
    ss << "(";
    ss << MalSequence::to_string();
    ss << ")";
    return ss.str();
}

MalList::MalList(std::initializer_list<MalType *> elements)
    : MalSequence(elements) {}

MalList *MalList::clone() const {
    return new MalList(this->elem_clone());
}

bool MalList::equal(const MalType *type) const {
    auto other_list = dynamic_cast<const MalList*>(type);
    if (!other_list){
        return false;
    }
    std::size_t i;
    for (i = 0; i < this->elements_.size() && i < other_list->elements_.size(); ++i) {
        if (!this->elements_[i]->equal(other_list->elements_[i])){
            return false;
        }
    }
    return i == this->elements_.size() && i == other_list->elements_.size();
}

MalVector::MalVector(std::vector<MalType *> elements)
    : MalSequence(std::move(elements)) {}

auto MalVector::to_string() const -> std::string {
    std::stringstream ss;
    ss << "[";
    ss << MalSequence::to_string();
    ss << "]";
    return ss.str();
}

MalVector *MalVector::clone() const {
    return new MalVector(this->elem_clone());
}

MalVector::MalVector(std::initializer_list<MalType *> elements)
    : MalSequence(elements) {}

bool MalVector::equal(const MalType *type) const {
    auto other_vector = dynamic_cast<const MalVector*>(type);
    if (!other_vector){
        return false;
    }
    std::size_t i;
    for (i = 0; i < this->elements_.size() && i < other_vector->elements_.size(); ++i) {
        if (!this->elements_[i]->equal(other_vector->elements_[i])){
            return false;
        }
    }
    return i == this->elements_.size() && i == other_vector->elements_.size();
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

bool MalKeyword::equal(const MalType *type) const {
    auto other_keyword = dynamic_cast<const MalKeyword*>(type);
    return other_keyword && this->name_ == other_keyword->name_;
}


MalMap::MalMap(const std::set<MalPair*>& elements)
        : elements_(elements) {}

auto MalMap::to_string() const -> std::string {
    std::stringstream ss;
    ss << "{";
    bool first = true;
    for (const auto& e : elements_) {
        if (!first) {
            ss << " ";
        }
        ss << e->to_string();
        first = false;
    }
    ss << "}";
    return ss.str();
}

MalMap::~MalMap() {
    for (const auto& e: this->elements_) {
        delete e;
    }
}

MalMap* MalMap::clone() const {
    std::set<MalPair*> copied;
    for (const auto& e : elements_) {
        copied.insert(e);
    }
    return new MalMap(copied);
}

std::set<MalPair*>& MalMap::get_elem() {
    return this->elements_;
}

bool MalMap::equal(const MalType *type) const {
    auto other_map = dynamic_cast<const MalMap*>(type);
    if (!other_map) return false;

    if (this->elements_.size() != other_map->elements_.size()) {
        return false;
    }

    for (const auto& pair : this->elements_) {
        MalType* key = pair->key();
        MalType* value = pair->value();

        MalType* other_val = other_map->get(key);
        if (!other_val || !value->equal(other_val)) {
            return false;
        }
    }

    return true;
}

MalType *MalMap::get(MalType *key) const {
    for (const auto& pair : elements_) {
        if (pair->key()->equal(key)) {
            return pair->value();
        }
    }
    return nullptr;
}

void MalMap::put(MalType *key, MalType *value) {
    for (const auto& pair : this->elements_) {
        if (pair->key()->equal(key)) {
            const_cast<MalPair*>(pair)->setValue(value);
            return;
        }
    }
    this->elements_.insert(new MalPair(key, value));
}

MalMetaData::MalMetaData(MalMap *map) : data_(map) {}

MalMetaData::~MalMetaData() {
    delete this->data_;
}

std::string MalMetaData::to_string() const {
    return std::string(); // todo
}

MalMap *MalMetaData::get_map() {
    return nullptr; // todo
}

MalType *MalMetaData::get(MalType *key) {
    return key; // todo
}

void MalMetaData::put(MalType *key, MalType *val) {
    if (key && val) { // todo
    }
}

MalMetaData *MalMetaData::clone() const {
    return new MalMetaData(this->data_->clone());
}

bool MalMetaData::equal(const MalType *type) const {
    auto other_metadata = dynamic_cast<const MalMetaData*>(type);
    return other_metadata && this->data_->equal(other_metadata->data_);
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

bool MalQuote::equal(const MalType *type) const {
    return dynamic_cast<const MalQuote*>(type);
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

bool MalQuasiQuote::equal(const MalType *type) const {
    return dynamic_cast<const MalQuasiQuote*>(type);
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

bool MalUnQuote::equal(const MalType *type) const {
    return dynamic_cast<const MalUnQuote*>(type);
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

bool MalUnQuoteSplicing::equal(const MalType *type) const {
    return dynamic_cast<const MalUnQuoteSplicing*>(type);
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

bool MalDeref::equal(const MalType *type) const {
    return dynamic_cast<const MalDeref*>(type);
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
    return new MalMetaSymbol(meta_->clone(), value_->clone());
}

bool MalMetaSymbol::equal(const MalType *type) const {
    auto other_meta_symbol = dynamic_cast<const MalMetaSymbol*>(type);
    return other_meta_symbol &&
           this->meta_->equal(other_meta_symbol->meta_) &&
           this->value_->equal(other_meta_symbol->value_);
}


MalFunction::MalFunction(std::function<mal_func_type> fn)
    : func_(std::move(fn)), is_builtin(true), params_list(nullptr), body_(nullptr), env_(nullptr) {}

MalFunction::MalFunction(MalList *params, MalType *body, Env &env)
    : func_(), is_builtin(false), params_list(params), body_(body), env_(&env) {}

MalType *MalFunction::operator()(mal_func_args_list_type& params) const {
    if (this->is_builtin){
        return this->func_(params);
    }
    const auto& args_list_elems = this->args_list->get_elem();
    const auto size = args_list_elems.size();
    std::vector<std::string> args_names(size);
    for (std::size_t i = 0; i < size; ++i) {
        const auto sym = dynamic_cast<MalSymbol*>(args_list_elems[i]);
        if (!sym) {
            throw typeError("fn* parameters must be symbols");
        }
        args_names[i] = sym->name();
    }
    const auto local_env = new Env(this->env_, args_names, params);
    return Evaluator::eval(this->body_, *local_env);
}

MalType *MalFunction::apply(mal_func_args_list_type& args) const {
    return (*this)(args);
}

MalFunction *MalFunction::clone() const {
    return new MalFunction(*this);
}

std::string MalFunction::to_string() const {
    return "#<function>";
}

bool MalFunction::equal(const MalType *type) const {
    return false;
}

MalPair::MalPair(MalType *key, MalType *value)
    : data_(key, value) {}

MalType *MalPair::key() const {
    return this->data_.first;
}

MalType *MalPair::value() const {
    return this->data_.second;
}

bool MalPair::equal(const MalType *other) const {
    auto other_pair = dynamic_cast<const MalPair*>(other);
    return other_pair &&
           this->key()->equal(other_pair->key()) &&
           this->value()->equal(other_pair->value());
}

MalPair* MalPair::clone() const {
    return new MalPair(this->key()->clone(), this->value()->clone());
}

std::string MalPair::to_string() const {
    return this->key()->to_string() + " " + this->value()->to_string();
}

MalPair::~MalPair() {
    delete this->data_.first;
    delete this->data_.second;
}

void MalPair::setValue(MalType* val) {
    delete this->data_.second;
    this->data_.second = val;
}
