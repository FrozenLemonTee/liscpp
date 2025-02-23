#ifndef TYPES_H
#define TYPES_H
#include <map>
#include <string>
#include <vector>

class MalType {
    public:
        static bool isKeyword(const std::string& token);
        static bool isInt(const std::string& token);
        static bool isNil(const std::string& token);
        static bool isBool(const std::string& token);
        static bool isString(const std::string& token);

        virtual ~MalType() = default;
        virtual std::string to_string() = 0;
};

class MalAtom : public MalType {
    public:
        ~MalAtom() override = default;
};

class MalStruct : public MalType {
    public:
        ~MalStruct() override = default;
};

class MalNil final : public MalAtom {
    public:
        std::string to_string() override;
};

class MalBool final : public MalAtom {
        bool val_;
    public:
        explicit MalBool(bool val);
        std::string to_string() override;
};

class MalInt final : public MalAtom {
        int64_t val_;
    public:
        explicit MalInt(int64_t val);
        std::string to_string() override;
};

class MalChar final : public MalAtom {
        char val_;
    public:
        explicit MalChar(const std::string& val);
        std::string to_string() override;
};

class MalString final : public MalAtom {
        std::string val_;
    public:
        explicit MalString(const std::string&  val);
        std::string to_string() override;
};

class MalSymbol final : public MalAtom {
        std::string name_;
    public:
        explicit MalSymbol(std::string name);
        std::string to_string() override;
};

class MalList final : public MalStruct {
        std::vector<MalType*> elements_;

    public:
        explicit MalList(std::vector<MalType*> elements);

        std::string to_string() override;
        ~MalList() override;
};

class MalVector final : public MalStruct {
        std::vector<MalType*> elements_;
    public:
        explicit MalVector(std::vector<MalType*> elements);

        std::string to_string() override;
        ~MalVector() override;
};

class MalKeyword final : public MalAtom {
        std::string name_;
    public:
        explicit MalKeyword(std::string name);

        std::string to_string() override;
        ~MalKeyword() override = default;
};

class MalMap final : public MalStruct {
    std::map<MalType*, MalType*> elements_;
    public:
    explicit MalMap(const std::map<MalType*, MalType*>& elements);

    std::string to_string() override;
    ~MalMap() override;
};

#endif //TYPES_H
