#include "reader.h"
#include "error.h"
#include <utility>
#include <regex>

auto Reader::tokenize(std::string input) -> std::vector<std::string> {
    std::vector<std::string> tokens;
    const std::regex tokens_pattern(token_pattern);
    const std::sregex_iterator it_begin(input.begin(), input.end(), tokens_pattern);
    const std::sregex_iterator it_end{};
    for (auto it = it_begin; it != it_end; ++it) {
        std::string token = it->str();
        token.erase(0, token.find_first_not_of(" \t\n\r,"));
        token.erase(token.find_last_not_of(" \t\n\r,") + 1);

        if (!token.empty()) {
            tokens.emplace_back(token);
        }
    }
    return tokens;
}

auto Reader::read_str(std::string input) -> MalType* {
    const auto tokens = tokenize(std::move(input));
    Reader reader(tokens, 0);
    return read_form(reader);
}

auto Reader::read_form(Reader &reader) -> MalType* {
    if (reader.hasNext()) {
        const std::string token = reader.peek();
        if (token == "(" || token == "[") {
            return read_struct(reader, token == "(" ? ")" : "]");
        }
        if (token == "{") {
            return read_struct(reader, "}");
        }
    }else {
        return new MalNil();
    }

    return read_atom(reader);
}

auto Reader::read_struct(Reader &reader, const std::string& type) -> MalStruct* {
    std::string token;

    if (!reader.hasNext()) {
        throw syntaxError("unbalanced");
    }

    if (reader.peek() == "(" || reader.peek() == "[") {
        std::vector<MalType*> list;
        while (reader.hasNext()) {
            reader.next();
            token = reader.peek();
            if (token == type) {
                break;
            }
            list.emplace_back(read_form(reader));
        }

        if (token == ")") {
            return new MalList(list);
        }
        if (token == "]") {
            return new MalVector(list);
        }

    } else if (reader.peek() == "{") {
        std::map<MalType*, MalType*> map;
        MalType* key = nullptr;
        MalType* value = nullptr;

        while (reader.hasNext()) {
            reader.next();
            token = reader.peek();

            if (token == "}") {
                break;
            }

            if (key == nullptr) {
                key = read_form(reader);
            } else {
                value = read_form(reader);
                map.insert({key, value});
                key = nullptr;
            }
        }

        if (key != nullptr)
            throw syntaxError("unbalanced");

        return new MalMap(map);
    }

    throw syntaxError("unbalanced");
}

auto Reader::read_atom(const Reader &reader) -> MalAtom* {
    const auto token = reader.peek();
    if (MalType::isInt(token)) {
        return new MalInt(std::stoll(token));
    }
    if (MalType::isNil(token)) {
        return new MalNil();
    }
    if (MalType::isBool(token)) {
        return new MalBool(token == "true");
    }
    if (MalType::isString(token))
        return new MalString(token);
    if (MalType::isKeyword(token))
        return new MalKeyword(token.substr(1));

    return new MalSymbol(token);
}

Reader::Reader(std::vector<std::string> tokens, const size_t pos)
    : tokens_(std::move(tokens)), pos_(pos) {}

auto Reader::peek() const -> std::string {
    return tokens_[pos_];
}

auto Reader::next() -> std::string {
    if (!this->hasNext())
        return {};

    auto token = this->peek();
    ++pos_;
    return token;
}

auto Reader::hasNext() const -> bool {
    return pos_ < tokens_.size();
}
