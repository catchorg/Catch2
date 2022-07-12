function fish_prompt
    # A simple prompt. Displays the current directory
    # (which fish stores in the $PWD variable)
    # and then a user symbol - a '►' for a normal user and a '#' for root.
    set -l user_char '►'
    if fish_is_root_user
        set user_char '#'
    end

    echo (set_color yellow)$PWD (set_color purple)$user_char
end

# From https://fishshell.com/docs/current/language.html#functions
