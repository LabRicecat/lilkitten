A smal DSL library, built for simplicity

## Installation 
Install this as dependency using the [catcaretaker](https://github.com/labricecat/catcaretaker) tool.
```
$ catcare get labricecat/lilkitten@main
```

## Usage
```cpp
#include "catpkgs/lilkitten/lilkitten.hpp"

int main() {
    // create a new environment
    auto env = lil::Environment() 
        .set_lexer(KittenLexer()
            .add_ignore(' ')
            .add_linebreak('\n')
            .add_stringq('"')
            .erase_empty()) // configure the lexer
        .set_storage(lil::StorageContainer() // configure the storage
            .create_event("print",[](std::vector<KittenToken> args, lil::StorageContainer& storage)->std::string {
                // test event which prints all arguments
                for(auto i : args) std::cout << i.src << " ";
                std::cout << "\n";
                return "";
            })
            .create_event("set",[](std::vector<KittenToken> args, lil::StorageContainer& storage)->std::string {
                // stores the second argument with the key being the first argument 
                // inside of the global storage
                storage.basic_map[args[0].src] = args[1].src;
                return ""; // return an error message here, if needed
            }))
        .set_token_eval([](const KittenToken& t, lil::StorageContainer& storage)->KittenToken {
            // explain how tokens shall be treated
            if(t.str) return t;
            else return KittenToken{.src = storage.basic_map[t.src], .str = false, .line = t.line};
        });
    
    // execute the given code and return 
    // an status message
    env.eval(R"(
        set "a" "Hello"
        print "Hello World!"
        print a
    )");
}
```