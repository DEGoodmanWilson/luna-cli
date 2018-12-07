#include <iostream>
#include <nlohmann/json.hpp>
#include <inja/inja.hpp>
#include <docopt.h>

static const std::string version{"1.0.0"};
static const std::string luna{"luna"};

int main(int argc, char **argv)
{
    static const char USAGE[] =
            R"(luna.

    Usage:
      luna <project_name>

    Options:
      <project_name>    The name of your project.
    )";

    std::string project_name;
    bool test{false};

    std::map<std::string, docopt::value> args
            = docopt::docopt(USAGE,
                             {argv + 1, argv + argc},
                             true,               // show help if requested
                             luna + " " + version);  // version string

    for (auto const &arg : args)
    {
        if ((arg.first == "<project_name>") && arg.second)
        {
            project_name = arg.second.asString();
        }
    }

    std::cout << project_name << std::endl;
    return 0;
}