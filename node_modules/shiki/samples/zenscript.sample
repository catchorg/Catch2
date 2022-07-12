import crafttweaker.api.BracketHandlers;

val air = <item:minecraft:air>;
val diamond = <item:minecraft:diamond>;
var woodTypes = ["oak","spruce","birch","jungle","acacia","dark_oak"];

for name in woodTypes {
    val thing = BracketHandlers.getItem("minecraft:" + name + "_planks");
    craftingTable.addShaped(name + "_diamond", diamond, [[air, thing], [thing, air]]);
}

function checkLeapYear(year as int) as bool {
    if(year % 4 == 0) {
        if(year % 100 == 0) {
            if(year % 400 == 0) {
                return true;
            } else {
                return false;
            }
        } else {
            return true;
        }
    } else {
        return false;
    }
}

print("Is 2000 a leap year: " ~ checkLeapYear(2000));
print("Is 2004 a leap year: " ~ checkLeapYear(2004));
print("Is 2100 a leap year: " ~ checkLeapYear(2100));
print("Is 2012 a leap year: " ~ checkLeapYear(2012));


//Note: this is a cleaner way
function checkLeapYear2(year as int) as bool {
    return year % 4 == 0 && (year % 100 != 0 || year % 400 == 0);
}

print("Is 2000 a leap year (2nd function): " ~ checkLeapYear2(2000));
print("Is 2004 a leap year (2nd function): " ~ checkLeapYear2(2004));
print("Is 2100 a leap year (2nd function): " ~ checkLeapYear2(2100));
print("Is 2012 a leap year (2nd function): " ~ checkLeapYear2(2012));

# From https://github.com/CraftTweaker/CraftTweaker-Examples