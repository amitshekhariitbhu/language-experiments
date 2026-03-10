# mixin_modules.rb
# Ruby uses modules as mixins instead of multiple inheritance.
# include, extend, and prepend each work differently and affect
# the Method Resolution Order (MRO) in distinct ways.

puts "=== Mixins: include vs extend vs prepend ==="
puts

# --- include: adds module methods as instance methods ---
puts "1. INCLUDE — adds instance methods:"

module Greetable
  def greet
    "Hello, I'm #{name}"
  end
end

class Person
  include Greetable

  def name
    "Alice"
  end
end

puts "   Person.new.greet => #{Person.new.greet}"
puts "   Method is on instances, not the class itself"
begin
  Person.greet
rescue NoMethodError => e
  puts "   Person.greet => NoMethodError: #{e.message}"
end
puts

# --- extend: adds module methods as class methods ---
puts "2. EXTEND — adds class methods:"

module ClassInfo
  def info
    "I am the #{name} class with #{instance_methods(false).length} instance methods"
  end
end

class Animal
  extend ClassInfo

  def speak
    "..."
  end
end

puts "   Animal.info => #{Animal.info}"
begin
  Animal.new.info
rescue NoMethodError => e
  puts "   Animal.new.info => NoMethodError (not on instances!)"
end
puts

# --- extend on instances ---
puts "3. EXTEND on a single object — singleton methods:"

module ExtraPowers
  def fly
    "I can fly!"
  end
end

a = Person.new
b = Person.new
a.extend(ExtraPowers)

puts "   a.fly => #{a.fly}"
begin
  b.fly
rescue NoMethodError
  puts "   b.fly => NoMethodError (only a was extended!)"
end
puts

# --- prepend: inserts BEFORE the class in the MRO ---
puts "4. PREPEND — the method resolution twist:"

module Logging
  def work
    puts "   [LOG] Before work"
    result = super  # Calls the original class method!
    puts "   [LOG] After work"
    result
  end
end

class Worker
  prepend Logging  # Logging goes BEFORE Worker in the chain

  def work
    puts "   Doing actual work..."
    "done"
  end
end

puts "   Worker.new.work:"
Worker.new.work
puts
puts "   With include, Logging would go AFTER Worker (unreachable)"
puts "   With prepend, Logging goes BEFORE Worker (wraps it!)"
puts

# --- Method Resolution Order ---
puts "5. METHOD RESOLUTION ORDER (ancestors chain):"

module M1; end
module M2; end
module M3; end

class Base
  include M1
end

class Child < Base
  include M2
  prepend M3
end

puts "   Child.ancestors:"
Child.ancestors.each_with_index do |a, i|
  puts "     #{i}: #{a}"
end
puts
puts "   Notice: M3 (prepend) is BEFORE Child"
puts "   M2 (include) is AFTER Child but BEFORE Base"
puts "   M1 (included in Base) is AFTER Base"
puts

# --- Diamond problem resolution ---
puts "6. DIAMOND PROBLEM — Ruby linearizes the MRO:"

module Serializable
  def serialize
    "serialized"
  end
end

module Cacheable
  include Serializable

  def serialize
    "cached: #{super}"
  end
end

module Loggable
  include Serializable

  def serialize
    "logged: #{super}"
  end
end

class Service
  include Cacheable
  include Loggable  # Last include wins in the chain
end

puts "   Service includes Cacheable and Loggable (both include Serializable)"
puts "   Service.new.serialize => #{Service.new.serialize}"
puts "   Ancestors: #{Service.ancestors.inspect}"
puts "   ^ Serializable appears only ONCE (linearization removes duplicates)"
puts

# --- The included/extended/prepended hooks ---
puts "7. HOOKS — Modules know when they're mixed in:"

module SelfAware
  def self.included(base)
    puts "   #{name} was included into #{base}"
  end

  def self.extended(base)
    puts "   #{name} was extended into #{base}"
  end

  def self.prepended(base)
    puts "   #{name} was prepended to #{base}"
  end
end

class Demo
  include SelfAware
end

class Demo2
  extend SelfAware
end

class Demo3
  prepend SelfAware
end
