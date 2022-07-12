struct Foo(T)
end

Foo(Int32)

# ---
struct Foo
end

# struct Bar < Foo
# end
# Error in ./struct/struct.cr:10: can't extend non-abstract struct Foo

abstract struct AbstractFoo
end

struct Bar < AbstractFoo
end

# ---
struct Test
  def initialize(@test : String)
  end
end

Test.new("foo")

# ---
struct User
  property name, age

  def initialize(@name : String, @age : Int32)
  end

  def print
    puts "#{age} - #{name}"
  end
end

puts User.new("osman", 3).name
User.new("ali", 9).print

# From https://github.com/askn/crystal-by-example/blob/master/struct/struct.cr