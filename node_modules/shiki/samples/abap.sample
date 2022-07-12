report zldbread no standard page heading.
tables: lfa1.
data: begin of t occurs 0,
linfr like lfa1-lifnr,
name1 like lfa1-name1,
end of t.

start-of-selection.

get lfa1.
clear t.
move-corresponding lfa1 to t.
append t.
end-of-selection.
sort t by name1.
loop at t.

write: / t-name1, t-lifnr.
endloop.

*- From https://sapbrainsonline.com/abap-tutorial/codes/reading-logical-database-using-abap-program.html -*