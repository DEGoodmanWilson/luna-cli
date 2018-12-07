#include <iostream>
#include <boost/filesystem.hpp>
#include <nlohmann/json.hpp>
#include <inja/inja.hpp>
#include <docopt.h>

static const std::string version{"1.0.0"};
static const std::string luna{"luna"};

int main(int argc, char **argv)
{
    static const char USAGE[] = R"(luna.

Usage: luna <project_name> [--template=<template_name>]

Options:
  <project_name>                The name of your project.
  --template=<template_name>    The project template to use. [default: basic].
)";

    std::string project_name, template_name;
    const std::string version_string{std::string{LUNA_NAME} + " " + LUNA_VERSION};

    std::map<std::string, docopt::value> args
            = docopt::docopt(USAGE,
                             {argv + 1, argv + argc},
                             true,               // show help if requested
                             version_string);  // version string

    for (auto const &arg : args)
    {
        if ((arg.first == "<project_name>") && arg.second)
        {
            project_name = arg.second.asString();
        }
        else if ((arg.first == "--template") && arg.second)
        {
            template_name = arg.second.asString();
        }
    }

    // make the output directory
    if (!boost::filesystem::create_directory(project_name))
    {
        std::cerr << "ERROR!! Directory " << project_name << " already exists. Exiting." << std::endl;
    }

    // get the path to the requested template files
    boost::filesystem::path template_path{"."};
    (template_path /= "templates") /= template_name;

    // Load up inja variables
    nlohmann::json template_data;
    template_data["project_name"] = project_name;
    auto project_name_cap = project_name;
    project_name_cap[0] = toupper(project_name[0]);
    template_data["project_name_cap"] = project_name_cap;
    inja::Environment env{};

    // render templates
    for (auto &entry : boost::filesystem::directory_iterator(template_path))
    {
        std::cout << "    " << entry.path() << '\n';
        // extract filename
        auto filename = entry.path().filename();
        if (filename.extension() != ".inja") continue;

        // parse inja
        auto temp = env.parse_template(entry.path().string());
        auto rendered_file = env.render_template(temp, template_data);

        // output file
        auto output_path = boost::filesystem::path(project_name) /= filename.stem();
        std::ofstream output_file;
        output_file.open(output_path.string(), std::ios::trunc);
        output_file << rendered_file;
        output_file.close();
    }

    return 0;
}