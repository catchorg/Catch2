%% File: person.hrl

%%-----------------------------------------------------------
%% Data Type: person
%% where:
%%    name:  A string (default is undefined).
%%    age:   An integer (default is undefined).
%%    phone: A list of integers (default is []).
%%    dict:  A dictionary containing various information 
%%           about the person. 
%%           A {Key, Value} list (default is the empty list).
%%------------------------------------------------------------
-record(person, {name, age, phone = [], dict = []}).

-module(person).
-include("person.hrl").
-compile(export_all). % For test purposes only.

%% This creates an instance of a person.
%%   Note: The phone number is not supplied so the
%%         default value [] will be used.

make_hacker_without_phone(Name, Age) ->
   #person{name = Name, age = Age, 
           dict = [{computer_knowledge, excellent}, 
                   {drinks, coke}]}.

%% This demonstrates matching in arguments

print(#person{name = Name, age = Age,
              phone = Phone, dict = Dict}) ->
  io:format("Name: ~s, Age: ~w, Phone: ~w ~n" 
            "Dictionary: ~w.~n", [Name, Age, Phone, Dict]).

%% Demonstrates type testing, selector, updating.

birthday(P) when is_record(P, person) -> 
   P#person{age = P#person.age + 1}.

register_two_hackers() ->
   Hacker1 = make_hacker_without_phone("Joe", 29),
   OldHacker = birthday(Hacker1),
   % The central_register_server should have 
   % an interface function for this.
   central_register_server ! {register_person, Hacker1},
   central_register_server ! {register_person, 
             OldHacker#person{name = "Robert", 
                              phone = [0,8,3,2,4,5,3,1]}}.

%% From https://erlang.org/doc/programming_examples/records.html#a-longer-example