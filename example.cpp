#include "lilkitten.hpp"

int main() {
    auto env = lil::Environment()
        .set_lexer(KittenLexer()
            .add_ignore(' ')
            .add_linebreak('\n')
            .add_stringq('"')
            .erase_empty())
        .set_storage(lil::StorageContainer()
            .create_event("print",[](std::vector<KittenToken> args, lil::StorageContainer& storage)->std::string {
                for(auto i : args) std::cout << i.src << " ";
                std::cout << "\n";
                return "";
            })
            .create_event("set",[](std::vector<KittenToken> args, lil::StorageContainer& storage)->std::string {
                storage.basic_map[args[0].src] = args[1].src;
                return "";
            }))
        .set_token_eval([](const KittenToken& t, lil::StorageContainer& storage)->KittenToken {
            if(t.str) return t;
            else return KittenToken{.src = storage.basic_map[t.src], .str = false, .line = t.line};
        });
    
    env.eval(R"(
        set "a" "Hello"
        print "Hello World!"
        print a
    )");
}