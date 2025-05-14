#ifndef READER_H
#define READER_H
#include <string>
#include <vector>
#include "types.h"

class Reader {
    constexpr static auto token_pattern = R"([\s,]*(~@|[\[\]{}()'`~^@]|"(?:\\.|[^\\"])*"?|;.*|[^\s\[\]{}('"`,;)]*))";

    std::vector<std::string> tokens_;
    size_t pos_;
public:
    static std::vector<std::string> tokenize(std::string input);
    static MalType* read_str(std::string input);
    static MalType* read_form(Reader& reader);
    static MalStruct *read_struct(Reader &reader, const std::string &type);
    static MalMap* read_map(Reader& reader);
    static MalSyntaxQuote* read_syntax_quote(Reader &reader, const std::string &type);
    static MalAtom* read_atom(const Reader &reader);
    static std::string unescape_string(const std::string& str);
    static std::string remove_comments(const std::string& input);
    Reader(std::vector<std::string> tokens, size_t pos);
    [[nodiscard]] std::string peek() const;
    std::string next();
    [[nodiscard]] bool hasNext() const;
};

#endif //READER_H
