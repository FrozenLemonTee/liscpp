#ifndef TYPES_H
#define TYPES_H
#include <set>
#include <string>
#include <vector>
#include <cstdint>
#include <functional>


class Env;
class MalMetaData;

class MalType {
    protected:
        MalMetaData* meta_ = nullptr;
    public:
        static bool isKeyword(const std::string& token);
        static bool isInt(const std::string& token);
        static bool isNil(const std::string& token);
        static bool isBool(const std::string& token);
        static bool isString(const std::string& token);

        virtual ~MalType();
        virtual bool equal(const MalType*) const = 0;
        [[nodiscard]] virtual MalType* clone() const = 0;
        [[nodiscard]] virtual std::string to_string(bool print_readably) const = 0;
};

class MalAtom : public MalType {
    public:
        [[nodiscard]] MalAtom* clone() const override = 0;
        ~MalAtom() override = default;
};

class MalStruct : public MalType {
    public:
        [[nodiscard]] MalStruct* clone() const override = 0;
        ~MalStruct() override = default;
};

class MalNil final : public MalAtom {
        std::nullptr_t val_;
        bool printable;
    public:
        explicit MalNil(bool printable = true, std::nullptr_t val = std::nullptr_t{});
        [[nodiscard]] std::nullptr_t& get_elem();

        bool equal(const MalType *type) const override;
        [[nodiscard]] MalNil* clone() const override;
        [[nodiscard]] std::string to_string(bool print_readably) const override;
};

class MalBool final : public MalAtom {
        bool val_;
    public:
        explicit MalBool(bool val);
        [[nodiscard]] bool& get_elem();

        bool equal(const MalType *type) const override;
        [[nodiscard]] MalBool* clone() const override;
        [[nodiscard]] std::string to_string(bool print_readably) const override;
};

class MalInt final : public MalAtom {
        int64_t val_;
    public:
        explicit MalInt(int64_t val);
        int64_t& get_elem();
        bool equal(const MalType *type) const override;
        [[nodiscard]] MalInt* clone() const override;
        [[nodiscard]] std::string to_string(bool print_readably) const override;
};

class MalString final : public MalAtom {
        std::string val_;
    public:
        explicit MalString(const std::string&  val);
        std::string& get_elem();
        bool equal(const MalType *type) const override;
        [[nodiscard]] MalString* clone() const override;
        [[nodiscard]] std::string to_string(bool print_readably) const override;
};

class MalSymbol final : public MalAtom {
        std::string name_;
    public:
        explicit MalSymbol(std::string name);
        [[nodiscard]] std::string name() const;
        bool equal(const MalType *type) const override;
        [[nodiscard]] MalSymbol* clone() const override;
        [[nodiscard]] std::string to_string(bool print_readably) const override;
};

class MalSequence : public MalStruct {
protected:
    std::vector<MalType*> elements_;

    [[nodiscard]] std::vector<MalType*> elem_clone() const;
    explicit MalSequence(std::vector<MalType*> elements);
    MalSequence(std::initializer_list<MalType*> elements);
    bool equal(const MalType* type) const override;
    [[nodiscard]] std::string to_string(bool print_readably) const override;
    [[nodiscard]] MalSequence* clone() const override = 0;
public:
    std::vector<MalType*>& get_elem();
    ~MalSequence() override;
};

class MalPair final : public MalStruct {
    std::pair<MalType*, MalType*> data_;

public:
    MalPair(MalType* key, MalType* value);

    [[nodiscard]] MalType* key() const;
    [[nodiscard]] MalType* value() const;
    void setValue(MalType* val);
    bool equal(const MalType* other) const override;
    [[nodiscard]] MalPair* clone() const override;
    [[nodiscard]] std::string to_string(bool print_readably) const override;
    ~MalPair() override;
};

class MalList final : public MalSequence {
    public:
        explicit MalList(std::vector<MalType*> elements);
        MalList(std::initializer_list<MalType*> elements);
        [[nodiscard]] std::string to_string(bool print_readably) const override;
        [[nodiscard]] MalList* clone() const override;
        ~MalList() override = default;
};

class MalVector final : public MalSequence {
    public:
        explicit MalVector(std::vector<MalType*> elements);
        MalVector(std::initializer_list<MalType*> elements);
        [[nodiscard]] std::string to_string(bool print_readably) const override;
        [[nodiscard]] MalVector* clone() const override;
        ~MalVector() override = default;
};

class MalKeyword final : public MalAtom {
        std::string name_;
    public:
        explicit MalKeyword(std::string name);
        [[nodiscard]] std::string name() const;
        bool equal(const MalType *type) const override;
        [[nodiscard]] std::string to_string(bool print_readably) const override;
        [[nodiscard]] MalKeyword* clone() const override;
        ~MalKeyword() override = default;
};

class MalMap final : public MalStruct {
    std::set<MalPair*> elements_;
    public:
    explicit MalMap(const std::set<MalPair*>& elements);
    std::set<MalPair*>& get_elem();
    MalType* get(MalType* key) const;
    void put(MalType* key, MalType* value);
    bool equal(const MalType *type) const override;
    [[nodiscard]] std::string to_string(bool print_readably) const override;
    [[nodiscard]] MalMap* clone() const override;
    ~MalMap() override;
};

class MalMetaData final : public MalType {
    MalMap* data_;
public:
    explicit MalMetaData(MalMap* map);
    ~MalMetaData() override;
    bool equal(const MalType *type) const override;
    [[nodiscard]] std::string to_string(bool print_readably) const override;
    [[nodiscard]] MalMetaData* clone() const override;
    MalMap* get_map();
    MalType* get(MalType* key);
    void put(MalType* key, MalType* val);
};

class MalSyntaxQuote : public MalStruct {
protected:
    MalType* expr_;
public:
    explicit MalSyntaxQuote(MalType* expr);
    [[nodiscard]] MalSyntaxQuote* clone() const override = 0;
    ~MalSyntaxQuote() override;
};

class MalQuote final : public MalSyntaxQuote {
public:
    explicit MalQuote(MalType* expr);
    bool equal(const MalType *type) const override;
    [[nodiscard]] MalQuote* clone() const override;
    [[nodiscard]] std::string to_string(bool print_readably) const override;
};

class MalQuasiQuote final : public MalSyntaxQuote{
public:
    explicit MalQuasiQuote(MalType* expr);
    bool equal(const MalType *type) const override;
    [[nodiscard]] MalQuasiQuote* clone() const override;
    [[nodiscard]] std::string to_string(bool print_readably) const override;
};

class MalUnQuote final : public MalSyntaxQuote{
public:
    explicit MalUnQuote(MalType* expr);
    bool equal(const MalType *type) const override;
    [[nodiscard]] MalUnQuote* clone() const override;
    [[nodiscard]] std::string to_string(bool print_readably) const override;
};

class MalUnQuoteSplicing final : public MalSyntaxQuote{
public:
    explicit MalUnQuoteSplicing(MalType* expr);
    bool equal(const MalType *type) const override;
    [[nodiscard]] MalUnQuoteSplicing* clone() const override;
    [[nodiscard]] std::string to_string(bool print_readably) const override;
};

class MalDeref final : public MalSyntaxQuote {
public:
    explicit MalDeref(MalType* expr);
    bool equal(const MalType *type) const override;
    [[nodiscard]] MalDeref* clone() const override;
    [[nodiscard]] std::string to_string(bool print_readably) const override;
};

class MalMetaSymbol final : public MalSyntaxQuote {
    MalType* meta_;
    MalType* value_;
public:
    explicit MalMetaSymbol(MalType* meta, MalType* value);
    [[nodiscard]] MalType* get_meta() const;
    [[nodiscard]] MalType* get_value() const;
    bool equal(const MalType *type) const override;
    [[nodiscard]] std::string to_string(bool print_readably) const override;
    [[nodiscard]] MalMetaSymbol* clone() const override;
    ~MalMetaSymbol() override;
};

class MalFunction final : public MalType {
public:
    using mal_func_args_list_type = const std::vector<MalType*>;
    using mal_func_return_type = MalType*;
    using mal_func_type = mal_func_return_type(mal_func_args_list_type);
private:
    std::function<mal_func_type> func_;
    bool is_builtin;
    MalList* args_list;
    MalType* body_;
    Env* env_;

public:
    explicit MalFunction(std::function<mal_func_type> fn);
    explicit MalFunction(MalList* args, MalType* body, Env* env);
    MalType* operator()(mal_func_args_list_type& params) const;
    [[nodiscard]] MalType* apply(mal_func_args_list_type& args) const;
    bool equal(const MalType *type) const override;
    [[nodiscard]] MalFunction* clone() const override;
    [[nodiscard]] std::string to_string(bool print_readably) const override;
};

#endif //TYPES_H
