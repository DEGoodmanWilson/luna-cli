#include <iostream>
#include <boost/filesystem.hpp>
#include <nlohmann/json.hpp>
#include <inja/inja.hpp>
#include <docopt.h>
#include <git2.h>

static const std::string version{"1.0.0"};
static const std::string luna{"luna"};

int main(int argc, char **argv)
{
    static const char USAGE[] = R"(luna.

Usage:
  luna <project_name> [--template=<template_name>]
  luna --refresh_templates

Options:
  <project_name>                The name of your project.
  --template=<template_name>    The project template to use. [default: basic].
  --refresh_templates           Redownload all the templates
)";

    std::string project_name, template_name;
    const std::string version_string{std::string{LUNA_NAME} + " " + LUNA_VERSION};
    bool refresh_templates{false};

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
        else if ((arg.first == "--refresh_templates"))
        {
            refresh_templates = arg.second.asBool();
        }
    }


    boost::filesystem::path config_path{getenv("HOME")};
    config_path /= ".luna";
    auto template_path{config_path};
    template_path /= "templates";
    if (!boost::filesystem::exists(config_path))
    {
        boost::filesystem::create_directory(config_path);
    }

    if (refresh_templates)
    {
        boost::filesystem::remove_all(template_path);
    }

    if (!boost::filesystem::exists(template_path))
    {
        boost::filesystem::create_directory(template_path);

        // now, download the currently available templates.
        const char *url{"https://github.com/DEGoodmanWilson/luna-templates.git"};
        const char *path{template_path.string().c_str()};




        ////
        git_libgit2_init();
        git_repository *repo = NULL;
        int error = git_clone(&repo, url, path, NULL);
        if (error != 0)
        {
            const git_error *e = giterr_last();
            printf("Error %d/%d: %s\n", error, e->klass, e->message);
            exit(error);
        }

        // TODO do this with a progress bar https://libgit2.org/docs/guides/101-samples/#repositories_clone_progress

        if(refresh_templates)
        {
            std::cout << "Done!"<<std::endl;
            exit(0);
        }
    }


    // make the output directory
    if (!boost::filesystem::create_directory(project_name))
    {
        std::cerr << "ERROR!! Directory " << project_name << " already exists. Exiting." << std::endl;
        exit(1);
    }

    // get the path to the requested template files
    auto requested_template_path = template_path / template_name;
    auto project_path = boost::filesystem::path{"."} / project_name;

    // Load up inja variables
    nlohmann::json template_data;
    template_data["project_name"] = project_name;
    auto project_name_cap = project_name;
    project_name_cap[0] = toupper(project_name[0]);
    template_data["project_name_cap"] = project_name_cap;
    inja::Environment env{"", project_path.string() + boost::filesystem::path::preferred_separator};

    // render templates
    for (auto &entry : boost::filesystem::directory_iterator(requested_template_path))
    {
        std::cout << "    " << entry.path() << '\n';
        // extract filename
        auto filename = entry.path().filename();
        if (filename.extension() != ".inja") continue;

        // parse inja and output data
        auto templ = env.parse_template(entry.path().string());
        env.write(templ, template_data, filename.stem().string());
    }

    return 0;
}