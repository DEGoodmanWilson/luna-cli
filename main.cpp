#include <iostream>
#include <boost/filesystem.hpp>
#include <nlohmann/json.hpp>
#include <inja/inja.hpp>
#include <docopt.h>
#include <git2.h>

static const std::string luna_version{LUNA_VERSION};
static const std::string luna_name{LUNA_NAME};

int main(int argc, char **argv)
{
    std::cout << R"(       _
   ___/_)
  (, /      ,_   _
    /   (_(_/ (_(_(_
  CX________________
                    )

)";

    static const std::string usage{luna_name + " " + luna_version + R"(: The C++ web framework.

Usage:
  )" + luna_name + R"( <project_name> [--template=<template_name>]
  )" + luna_name + R"(  --refresh_templates

Options:
  <project_name>                The name of your project.
  --template=<template_name>    The project template to use. [default: basic].
  --refresh_templates           Redownload all the templates
)"};

    std::string project_name, template_name;
    const std::string version_string{luna_name + " " + luna_version};
    bool refresh_templates{false};

    std::map<std::string, docopt::value> args
            = docopt::docopt(usage,
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
    config_path /= "." + luna_name;
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
        std::cout << "Grabbing the latest Luna templates...";
        const char *url{"https://github.com/DEGoodmanWilson/luna-templates.git"};
        const char *path{template_path.string().c_str()};

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

        if (refresh_templates)
        {
            std::cout << " Done!" << std::endl;
            exit(0);
        }
    }


    // make the output directory
    std::cout << "Making folder " << project_name << "...";
    if (!boost::filesystem::create_directory(project_name))
    {
        std::cerr << "ERROR!! Directory " << project_name << " already exists. Exiting." << std::endl;
        exit(1);
    }
    std::cout << " Done!" << std::endl;

    // get the path to the requested template files
    auto requested_template_path = template_path / template_name;
    auto project_path = boost::filesystem::path{"."} / project_name;

    // Load up inja variables
    nlohmann::json template_data;
    template_data["project_name"] = project_name;
    auto project_name_cap = project_name;
    project_name_cap[0] = toupper(project_name[0]);
    template_data["project_name_cap"] = project_name_cap;
    inja::Environment env{""};

    // render templates
    for (auto &entry : boost::filesystem::recursive_directory_iterator(requested_template_path))
    {
        auto parent_path = entry.path().parent_path();
        auto relative_path = boost::filesystem::relative(entry.path(), requested_template_path);
        auto relative_path_parent = boost::filesystem::relative(parent_path, requested_template_path);
        auto filename = entry.path().filename();

        if (boost::filesystem::is_directory(entry.path()))
        {
            // make directory
            auto output_path = project_path / relative_path;
            std::cout << "Making dir " << output_path << std::endl;
            boost::filesystem::create_directory(output_path);
        }
        else //it's a file
        {
            if (filename.extension() == ".inja")
            {
                //it's a template, render it without the inja suffix
                filename = filename.stem().string();
            }

            auto out_filename = project_path / relative_path_parent / filename;
            if (relative_path_parent.string() == ".")
            {
                out_filename = project_path / filename;
            }
            std::cout << "Creating   " << out_filename << std::endl;

            // parse inja and output data
            auto templ = env.parse_template(entry.path().string());
            env.write(templ, template_data, out_filename.string());
        }
    }

    std::cout << project_name << R"( ready for use.
********

If you haven't yet installed conan, do so by typing

brew install conan

Then, configure conan by typing:

conan remote add vthiery https://api.bintray.com/conan/vthiery/conan-packages
conan remote add degoodmanwilson https://api.bintray.com/conan/degoodmanwilson/opensource
conan remote add bincrafters https://api.bintray.com/conan/bincrafters/public-conan

(you only need to do this once)

Finally, to build )" << project_name << R"( type:

cd )" << project_name << R"(
conan install .
conan build .
./bin/)" << project_name << "\n\n...et voila!" << std::endl;

    return 0;
}