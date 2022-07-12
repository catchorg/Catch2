tell application "Address Book"

    set bDayList to name of every person whose birth date is not missing value

    choose from list bDayList with prompt "Whose birthday would you like?"

    if the result is not false then

        set aName to item 1 of the result

        set theBirthday to birth date of person named aName

        display dialog aName & "'s birthday is " & date string of theBirthday

    end if

end tell

-- From https://developer.apple.com/library/archive/documentation/AppleScript/Conceptual/AppleScriptLangGuide/reference/ASLR_cmds.html