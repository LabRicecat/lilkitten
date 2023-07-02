#ifndef LIL_KITTEN_HPP
#define LIL_KITTEN_HPP

#include "catpkgs/kittenlexer/kittenlexer.hpp"
#include <functional>
#include <vector>
#include <map>
#include <string>

namespace lil {

struct StorageContainer;

struct Event {
    using fn = std::function<std::string(std::vector<KittenToken>,StorageContainer&)>;
    fn event;
    /* later data may follow */
};

struct StorageContainer {
    std::map<std::string,Event> events;
    std::map<std::string,std::string> basic_map;

    StorageContainer& create_event(std::string name, Event::fn fn) {
        events[name] = Event{fn};
        return *this;
    }
};

enum class Message {
    Ok,
    Error,
    LexError,
    EventError,
    TokenEvalError,
    UnknownNameError,
};

struct Environment {
    using token_eval = std::function<KittenToken(const KittenToken&,StorageContainer&)>;
    using name_resolve = std::function<Event(const KittenToken&,StorageContainer&)>;
    using message_handler = std::function<Message(const std::string&,StorageContainer&)>;
    using token_preproccess = std::function<KittenToken(const KittenToken&,StorageContainer&)>;

    StorageContainer storage;
    KittenLexer lexer;
    /* default procedures */
    token_eval teval            = [](const KittenToken& t,StorageContainer&  ){ return t; };
    name_resolve nresolve       = [](const KittenToken& t,StorageContainer& s){ return s.events[t.src]; };
    message_handler mhandler    = [](const std::string& m,StorageContainer&  ){ return m != "" ? Message::EventError : Message::Ok; };
    token_preproccess tpp       = [](const KittenToken& t,StorageContainer&  ){ return t; };

    Environment& set_lexer(const KittenLexer& lexer) {
        this->lexer = lexer;
        return *this;
    }
    Environment& set_storage(const StorageContainer& storage) {
        this->storage = storage;
        return *this;
    }
    Environment& set_token_eval(const token_eval& eval) {
        teval = eval;
        return *this;
    }
    Environment& set_name_resolve(const name_resolve& resolve) {
        nresolve = resolve;
        return *this;
    }
    Environment& set_message_handler(const message_handler& handler) {
        mhandler = handler;
        return *this;
    }
    Environment& set_token_preproccess(const token_preproccess& pp) {
        tpp = pp;
        return *this;
    }
    

    Message eval(const std::string& source) {
        auto lexed = lexer.lex(source);
        if(!lexer) return Message::LexError;
        std::vector<std::vector<KittenToken>> lines;
        int line = 0;
        for(auto& i : lexed) {
            if(line != i.line) {
                line = i.line;
                lines.push_back({});
            }
            lines.back().push_back(tpp(i,storage));
        }

        for(auto& i : lines) {
            KittenToken name = i[0];
            i.erase(i.begin());
            for(auto& j : i) j = teval(j,storage);
            auto msg = mhandler(nresolve(name,storage).event(i,storage),storage);
            if(msg != Message::Ok) return msg;
        }
        return Message::Ok;
    }
};

}

#endif