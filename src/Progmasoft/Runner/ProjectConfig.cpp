// Copyright (c) 2026 Progmasoft.
// SPDX-License-Identifier: MPL-2.0 WITH AdditionRef-Progmasoft-Exception-2.0
#include <cmath>
#include <cstdint>
#include <cstdio>
#include <set>
#include <system_error>
#include <utility>

#include "Files.hpp"
#include "Json.hpp"
#include "ProjectConfig.hpp"

namespace Progmasoft::Catch3::Runner
{
    namespace
    {
        /// The version of the file format this runner reads.
        constexpr std::uint64_t kConfigVersion = 1;

        /// What the value of a key is, and how it becomes arguments.
        enum class ValueKind : std::uint8_t
        {
            /// true gives the option; false, the default, gives nothing.
            kFlag,
            /// false gives the negating option; true is the default.
            kNegated,
            /// A string, given as the value of the option.
            kWord,
            /// A file or directory, relative to the file's directory.
            kPath,
            /// A whole number.
            kCount,
            /// A whole number or a word, such as "auto".
            kCountOrWord,
            /// A number with a fraction, such as seconds.
            kNumber,
            /// An array of strings, each given to the option.
            kWords,
            /// A directory, or false for none.
            kPathOrFalse,
            /// "quiet", "normal" or "verbose".
            kVerbosity,
            /// An array of test specs.
            kTests,
        };

        struct ConfigKey final
        {
            std::string_view name;
            ValueKind kind;
            /// The option it stands for, when it is not spelled like the key.
            std::string_view option = {};
        };

        const std::vector<ConfigKey> &
        Keys()
        {
            static const auto *keys = new std::vector<ConfigKey>{
                { "tests", ValueKind::kTests },
                // Selecting tests.
                { "select", ValueKind::kWord },
                { "changes-since", ValueKind::kWord },
                { "incremental", ValueKind::kFlag },
                { "shard-durations", ValueKind::kPath },
                { "filenames-as-tags", ValueKind::kFlag },
                // Running.
                { "jobs", ValueKind::kCount },
                { "in-process", ValueKind::kFlag },
                { "isolate", ValueKind::kFlag },
                { "pass-env", ValueKind::kWords },
                { "timeout", ValueKind::kNumber },
                { "adaptive-timeout",
                  ValueKind::kNegated,
                  "no-adaptive-timeout" },
                { "retries", ValueKind::kCountOrWord },
                { "repeat", ValueKind::kCount },
                { "fail-fast", ValueKind::kCount },
                { "order", ValueKind::kWord },
                { "seed", ValueKind::kCountOrWord },
                { "bisect", ValueKind::kNegated, "no-bisect" },
                // How tests behave.
                { "success", ValueKind::kFlag },
                { "no-throw", ValueKind::kFlag },
                { "warn", ValueKind::kWords },
                { "allow-no-tests", ValueKind::kFlag },
                { "skip-benchmarks", ValueKind::kFlag },
                { "benchmark-samples", ValueKind::kCount },
                { "benchmark-resamples", ValueKind::kCount },
                { "benchmark-confidence-interval", ValueKind::kNumber },
                { "benchmark-no-analysis", ValueKind::kFlag },
                { "benchmark-warmup-time", ValueKind::kCount },
                // History and analysis.
                { "history", ValueKind::kPathOrFalse },
                { "quarantine", ValueKind::kWord },
                // Output.
                { "verbosity", ValueKind::kVerbosity },
                { "color", ValueKind::kWord },
                { "junit", ValueKind::kPath },
                { "json", ValueKind::kPath },
                { "trace", ValueKind::kPath },
                { "slowest", ValueKind::kCount },
            };
            return *keys;
        }

        /// Options that describe one run rather than a project.
        constexpr std::string_view kCommandLineOnly[] = {
            "section",
            "generator-index",
            "path-filter",
            "input-file",
            "shard",
            "break",
            "replay",
            "analytics",
            "import",
            "import-format",
            "import-name",
            "import-revision",
            "verbose",
            "quiet",
            "premature-exit-guard-file",
            "list",
            "list-tags",
            "help",
            "config",
            "no-config",
            "catch3-worker",
        };

        std::string
        Quoted(std::string_view key)
        {
            return "\"" + std::string{ key } + "\"";
        }

        std::string
        UnknownKey(std::string_view key)
        {
            for (const std::string_view option : kCommandLineOnly)
            {
                if (key == option)
                {
                    return Quoted(key)
                           + " belongs on the command line, not in a "
                             "catch3.json";
                }
            }
            std::string message = Quoted(key) + " is not a key of catch3.json";
            std::string_view closest;
            std::size_t best = 3;
            const auto consider = [&](std::string_view name) {
                const std::size_t distance = EditDistance(key, name);
                if (distance < best)
                {
                    best = distance;
                    closest = name;
                }
            };
            consider("version");
            for (const auto &entry : Keys())
            {
                consider(entry.name);
            }
            if (!closest.empty())
            {
                message += "; did you mean " + Quoted(closest) + "?";
            }
            return message;
        }

        /// A whole number JSON can say exactly, or nothing.
        std::optional<std::uint64_t>
        WholeNumber(const JsonValue &value)
        {
            if (!value.IsNumber())
            {
                return std::nullopt;
            }
            const double number = value.AsNumber();
            if (!std::isfinite(number) || number < 0
                || number > std::floor(number) || number > 9007199254740992.0)
            {
                return std::nullopt;
            }
            return static_cast<std::uint64_t>(number);
        }

        /// A number in the form the command line reads: digits and a point.
        std::string
        DecimalText(double number)
        {
            char text[400];
            std::snprintf(text, sizeof(text), "%.17f", number);
            std::string result{ text };
            for (char &character : result)
            {
                // The decimal separator of the locale a test may have set.
                if (character == ',')
                {
                    character = '.';
                }
            }
            return result;
        }

        std::string
        PathText(const std::string &value,
                 const std::filesystem::path &directory)
        {
            const std::filesystem::path path{ value };
            if (path.is_absolute() || directory.empty())
            {
                return path.lexically_normal().string();
            }
            return (directory / path).lexically_normal().string();
        }

        /// The arguments that one key and its value stand for.
        Expected<std::vector<std::string>>
        KeyArguments(const ConfigKey &key,
                     const JsonValue &value,
                     const std::filesystem::path &directory)
        {
            const std::string option
                = "--"
                  + std::string{ key.option.empty() ? key.name : key.option };
            const auto wrong = [&](std::string_view expected) {
                return Error{ Quoted(key.name) + " takes "
                              + std::string{ expected } + ", not "
                              + value.ToString() };
            };
            switch (key.kind)
            {
                case ValueKind::kFlag:
                    if (value.GetKind() != JsonValue::Kind::kBoolean)
                    {
                        return wrong("true or false");
                    }
                    if (value.AsBoolean())
                    {
                        return std::vector<std::string>{ option };
                    }
                    return std::vector<std::string>{};
                case ValueKind::kNegated:
                    if (value.GetKind() != JsonValue::Kind::kBoolean)
                    {
                        return wrong("true or false");
                    }
                    if (!value.AsBoolean())
                    {
                        return std::vector<std::string>{ option };
                    }
                    return std::vector<std::string>{};
                case ValueKind::kWord:
                    if (!value.IsString())
                    {
                        return wrong("a string");
                    }
                    return std::vector<std::string>{ option + "="
                                                     + value.AsString() };
                case ValueKind::kPath:
                    if (!value.IsString() || value.AsString().empty())
                    {
                        return wrong("a path");
                    }
                    return std::vector<std::string>{
                        option + "=" + PathText(value.AsString(), directory)
                    };
                case ValueKind::kCount:
                    if (const auto number = WholeNumber(value))
                    {
                        return std::vector<std::string>{
                            option + "=" + std::to_string(*number)
                        };
                    }
                    return wrong("a whole number");
                case ValueKind::kCountOrWord:
                    if (value.IsString())
                    {
                        return std::vector<std::string>{ option + "="
                                                         + value.AsString() };
                    }
                    if (const auto number = WholeNumber(value))
                    {
                        return std::vector<std::string>{
                            option + "=" + std::to_string(*number)
                        };
                    }
                    return wrong("a whole number or a string");
                case ValueKind::kNumber:
                    if (!value.IsNumber() || !std::isfinite(value.AsNumber())
                        || value.AsNumber() < 0)
                    {
                        return wrong("a number of at least 0");
                    }
                    return std::vector<std::string>{
                        option + "=" + DecimalText(value.AsNumber())
                    };
                case ValueKind::kWords:
                {
                    if (!value.IsArray())
                    {
                        return wrong("an array of strings");
                    }
                    std::vector<std::string> arguments;
                    for (const auto &item : value.Items())
                    {
                        if (!item.IsString())
                        {
                            return wrong("an array of strings");
                        }
                        arguments.push_back(option + "=" + item.AsString());
                    }
                    return arguments;
                }
                case ValueKind::kPathOrFalse:
                    if (value.GetKind() == JsonValue::Kind::kBoolean
                        && !value.AsBoolean())
                    {
                        return std::vector<std::string>{
                            "--no-" + std::string{ key.name }
                        };
                    }
                    if (!value.IsString() || value.AsString().empty())
                    {
                        return wrong("a path or false");
                    }
                    return std::vector<std::string>{
                        option + "=" + PathText(value.AsString(), directory)
                    };
                case ValueKind::kVerbosity:
                    if (value.IsString())
                    {
                        if (value.AsString() == "quiet")
                        {
                            return std::vector<std::string>{ "--quiet" };
                        }
                        if (value.AsString() == "verbose")
                        {
                            return std::vector<std::string>{ "--verbose" };
                        }
                        if (value.AsString() == "normal")
                        {
                            return std::vector<std::string>{};
                        }
                    }
                    return wrong("\"quiet\", \"normal\" or \"verbose\"");
                case ValueKind::kTests:
                {
                    if (!value.IsArray())
                    {
                        return wrong("an array of test specs");
                    }
                    // After --, every argument is a test spec, even one that
                    // starts with a dash.
                    std::vector<std::string> arguments{ "--" };
                    for (const auto &item : value.Items())
                    {
                        if (!item.IsString() || item.AsString().empty())
                        {
                            return wrong("an array of test specs");
                        }
                        arguments.push_back(item.AsString());
                    }
                    return arguments;
                }
            }
            return wrong("a known kind of value");
        }
    } // namespace

    std::optional<std::filesystem::path>
    FindProjectConfig(const std::filesystem::path &start)
    {
        std::error_code error;
        std::filesystem::path directory
            = std::filesystem::absolute(start, error).lexically_normal();
        if (error)
        {
            return std::nullopt;
        }
        if (directory.filename().empty() && directory.has_relative_path())
        {
            directory = directory.parent_path();
        }
        std::vector<std::filesystem::path> candidates;
        for (std::filesystem::path current = directory;;
             current = current.parent_path())
        {
            candidates.push_back(current);
            if (std::filesystem::exists(current / ".git", error))
            {
                break;
            }
            if (!current.has_relative_path())
            {
                // No repository holds the directory.
                candidates = { directory };
                break;
            }
        }
        for (const auto &candidate : candidates)
        {
            const auto file = candidate / kProjectConfigName;
            if (std::filesystem::is_regular_file(file, error))
            {
                return file;
            }
        }
        return std::nullopt;
    }

    Expected<Options>
    ApplyProjectConfig(std::string_view text,
                       const std::filesystem::path &directory,
                       std::string_view file,
                       Options options)
    {
        const std::string prefix = std::string{ file } + ": ";
        auto document = ParseJson(text);
        if (!document)
        {
            return Error{ prefix + document.GetError().Message() };
        }
        if (!document->IsObject())
        {
            return Error{ prefix + "the file holds an object, not "
                          + document->ToString() };
        }
        std::set<std::string_view> seen;
        bool versioned = false;
        for (const auto &[name, value] : document->Members())
        {
            if (!seen.insert(name).second)
            {
                return Error{ prefix + Quoted(name) + " appears twice" };
            }
            if (name == "version")
            {
                if (WholeNumber(value) != kConfigVersion)
                {
                    return Error{
                        prefix + "version " + value.ToString()
                        + " is not known; this Catch3 reads version 1"
                    };
                }
                versioned = true;
                continue;
            }
            if (name == "$schema")
            {
                // For editors; the runner knows its own format.
                continue;
            }
            const ConfigKey *key = nullptr;
            for (const auto &entry : Keys())
            {
                if (entry.name == name)
                {
                    key = &entry;
                    break;
                }
            }
            if (key == nullptr)
            {
                return Error{ prefix + UnknownKey(name) };
            }
            auto arguments = KeyArguments(*key, value, directory);
            if (!arguments)
            {
                return Error{ prefix + arguments.GetError().Message() };
            }
            auto applied = ParseOptions(*arguments, std::move(options));
            if (!applied)
            {
                return Error{ prefix + Quoted(name) + ": "
                              + applied.GetError().Message() };
            }
            options = std::move(*applied);
        }
        if (!versioned)
        {
            return Error{ prefix + "\"version\": 1 is missing" };
        }
        return options;
    }

    Expected<Options>
    ParseCommandLine(const std::vector<std::string> &arguments,
                     const std::filesystem::path &workingDirectory)
    {
        auto commandLine = ParseOptions(arguments);
        if (!commandLine)
        {
            return commandLine;
        }
        if (commandLine->noConfig || commandLine->mode == Mode::kWorker
            || commandLine->mode == Mode::kHelp
            || commandLine->mode == Mode::kVersion)
        {
            // A worker gets everything it needs from its runner.
            return commandLine;
        }
        std::filesystem::path file;
        if (!commandLine->configPath.empty())
        {
            file = commandLine->configPath;
            if (file.is_relative())
            {
                file = workingDirectory / file;
            }
        }
        else if (auto found = FindProjectConfig(workingDirectory))
        {
            file = std::move(*found);
        }
        else
        {
            return commandLine;
        }
        file = file.lexically_normal();
        auto text = ReadFile(file);
        if (!text)
        {
            return Error{ "cannot read the configuration " + file.string()
                          + ": " + text.GetError().Message() };
        }
        auto configured
            = ApplyProjectConfig(*text, file.parent_path(), file.string());
        if (!configured)
        {
            return configured;
        }
        Options base = std::move(*configured);
        // Specs on the command line replace those of the file rather than
        // widening them with "or".
        std::vector<std::string> fileSpecs = std::move(base.testSpecs);
        base.testSpecs.clear();
        auto parsed = ParseOptions(arguments, std::move(base));
        if (!parsed)
        {
            return parsed;
        }
        if (parsed->testSpecs.empty())
        {
            parsed->testSpecs = std::move(fileSpecs);
        }
        parsed->configPath = file.string();
        return parsed;
    }
} // namespace Progmasoft::Catch3::Runner
