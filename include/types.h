#ifndef TYPES_H
#define TYPES_H
#include <map>
#include <string>
#include <vector>
#include <cstdint>
#include <functional>


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
        [[nodiscard]] virtual MalType* clone() const = 0;
        [[nodiscard]] virtual std::string to_string() const = 0;
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
    public:
        explicit MalNil(std::nullptr_t val = std::nullptr_t{});
        [[nodiscard]] std::nullptr_t& get_elem();
        [[nodiscard]] MalNil* clone() const override;
        [[nodiscard]] std::string to_string() const override;
};

class MalBool final : public MalAtom {
        bool val_;
    public:
        explicit MalBool(bool val);
        [[nodiscard]] bool& get_elem();
        [[nodiscard]] MalBool* clone() const override;
        [[nodiscard]] std::string to_string() const override;
};

class MalInt final : public MalAtom {
        int64_t val_;
    public:
        explicit MalInt(int64_t val);
        int64_t& get_elem();
        [[nodiscard]] MalInt* clone() const override;
        [[nodiscard]] std::string to_string() const override;
};

class MalString final : public MalAtom {
        std::string val_;
    public:
        explicit MalString(const std::string&  val);
        std::string& get_elem();
        [[nodiscard]] MalString* clone() const override;
        [[nodiscard]] std::string to_string() const override;
};

class MalSymbol final : public MalAtom {
        std::string name_;
    public:
        explicit MalSymbol(std::string name);
        [[nodiscard]] std::string name() const;
        [[nodiscard]] MalSymbol* clone() const override;
        [[nodiscard]] std::string to_string() const override;
};

class MalSequence : public MalStruct {
protected:
    std::vector<MalType*> elements_;

    [[nodiscard]] std::vector<MalType*> elem_clone() const;
    explicit MalSequence(std::vector<MalType*> elements);
    MalSequence(std::initializer_list<MalType*> elements);
    [[nodiscard]] std::string to_string() const override;
    [[nodiscard]] MalSequence* clone() const override = 0;
public:
    std::vector<MalType*>& get_elem();
    ~MalSequence() override;
};

class MalList final : public MalSequence {
    public:
        explicit MalList(std::vector<MalType*> elements);
        MalList(std::initializer_list<MalType*> elements);
        [[nodiscard]] std::string to_string() const override;
        [[nodiscard]] MalList* clone() const override;
        ~MalList() override = default;
};

class MalVector final : public MalSequence {
    public:
        explicit MalVector(std::vector<MalType*> elements);
        MalVector(std::initializer_list<MalType*> elements);
        [[nodiscard]] std::string to_string() const override;
        [[nodiscard]] MalVector* clone() const override;
        ~MalVector() override = default;
};

class MalKeyword final : public MalAtom {
        std::string name_;
    public:
        explicit MalKeyword(std::string name);
        [[nodiscard]] std::string name() const;
        [[nodiscard]] std::string to_string() const override;
        [[nodiscard]] MalKeyword* clone() const override;
        ~MalKeyword() override = default;
};

class MalMap final : public MalStruct {
    std::map<MalType*, MalType*> elements_;
    public:
    explicit MalMap(const std::map<MalType*, MalType*>& elements);
    std::map<MalType*, MalType*>& get_elem();
    [[nodiscard]] std::string to_string() const override;
    [[nodiscard]] MalMap* clone() const override;
    ~MalMap() override;
};

class MalMetaData final : public MalType {
    MalMap* data_;
public:
    explicit MalMetaData(MalMap* map);
    ~MalMetaData() override;
    [[nodiscard]] std::string to_string() const override;
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
    [[nodiscard]] MalQuote* clone() const override;
    [[nodiscard]] std::string to_string() const override;
};

class MalQuasiQuote final : public MalSyntaxQuote{
public:
    explicit MalQuasiQuote(MalType* expr);
    [[nodiscard]] MalQuasiQuote* clone() const override;
    [[nodiscard]] std::string to_string() const override;
};

class MalUnQuote final : public MalSyntaxQuote{
public:
    explicit MalUnQuote(MalType* expr);
    [[nodiscard]] MalUnQuote* clone() const override;
    [[nodiscard]] std::string to_string() const override;
};

class MalUnQuoteSplicing final : public MalSyntaxQuote{
public:
    explicit MalUnQuoteSplicing(MalType* expr);
    [[nodiscard]] MalUnQuoteSplicing* clone() const override;
    [[nodiscard]] std::string to_string() const override;
};

class MalDeref final : public MalSyntaxQuote {
public:
    explicit MalDeref(MalType* expr);
    [[nodiscard]] MalDeref* clone() const override;
    [[nodiscard]] std::string to_string() const override;
};

class MalMetaSymbol final : public MalSyntaxQuote {
    MalType* meta_;
    MalType* value_;
public:
    explicit MalMetaSymbol(MalType* meta, MalType* value);
    [[nodiscard]] MalType* get_meta() const;
    [[nodiscard]] MalType* get_value() const;
    [[nodiscard]] std::string to_string() const override;
    [[nodiscard]] MalMetaSymbol* clone() const override;
    ~MalMetaSymbol() override;
};

class MalFunction : public MalType {
    std::function<MalType*(std::vector<MalType*>)> func_;
public:
    explicit MalFunction(std::function<MalType*(std::vector<MalType*>)> fn);
    MalType* operator()(const std::vector<MalType*>& args) const;
    [[nodiscard]] MalType* apply(const std::vector<MalType*>& args) const;
    [[nodiscard]] MalFunction* clone() const override;
    [[nodiscard]] std::string to_string() const override;
};

#endif //TYPES_H
