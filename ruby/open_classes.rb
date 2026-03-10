# open_classes.rb
# Ruby classes are never "closed." You can reopen ANY class — including
# core classes like String, Integer, Array — and add or modify methods.
# This is called "monkey patching." It's powerful but dangerous.

puts "=== Open Classes & Monkey Patching ==="
puts

# --- Reopening a built-in class ---
puts "1. MONKEY PATCHING Integer — add a method to ALL integers:"

class Integer
  def factorial
    return 1 if self <= 1
    self * (self - 1).factorial
  end
end

puts "   5.factorial  => #{5.factorial}"
puts "   10.factorial => #{10.factorial}"
puts "   0.factorial  => #{0.factorial}"
puts

# --- Adding methods to String ---
puts "2. MONKEY PATCHING String:"

class String
  def shout
    upcase + "!!!"
  end

  def word_count
    split.length
  end
end

puts "   'hello'.shout         => #{'hello'.shout}"
puts "   'one two three'.word_count => #{'one two three'.word_count}"
puts

# --- The dangerous side: overriding existing methods ---
puts "3. DANGER — Overriding existing methods:"
puts "   Before: 1 + 1 = #{1 + 1}"

# We'll be careful and save the original
class Integer
  alias_method :original_plus, :+

  def +(other)
    original_plus(other).original_plus(1)  # Adds 1 to every addition!
  end
end

puts "   After monkey patch: 1 + 1 = #{1 + 1}"
puts "   2 + 3 = #{2 + 3}"
puts "   ^ Every addition is off by 1! This breaks EVERYTHING."

# Restore it
class Integer
  alias_method :+, :original_plus
end
puts "   Restored: 1 + 1 = #{1 + 1}"
puts

# --- Refinements: scoped monkey patching (safer alternative) ---
puts "4. REFINEMENTS — Scoped monkey patching (Ruby 2.0+):"

module StringExtensions
  refine String do
    def palindrome?
      self == reverse
    end
  end
end

# Outside the refinement scope, palindrome? doesn't exist
begin
  "racecar".palindrome?
rescue NoMethodError => e
  puts "   Without 'using': NoMethodError — #{e.message}"
end

# Activate the refinement in this scope
using StringExtensions
puts "   With 'using':    'racecar'.palindrome? => #{'racecar'.palindrome?}"
puts "   With 'using':    'hello'.palindrome?   => #{'hello'.palindrome?}"
puts

# --- Reopening your own classes ---
puts "5. REOPENING classes — definition is additive:"

class Dog
  def bark
    "Woof!"
  end
end

class Dog
  def fetch
    "Fetching!"
  end
  # bark is still defined from the first opening!
end

dog = Dog.new
puts "   dog.bark  => #{dog.bark}"
puts "   dog.fetch => #{dog.fetch}"
puts

# --- freeze prevents modification of instances, not classes ---
puts "6. FREEZE — prevents object modification but not class reopening:"
str = "frozen"
str.freeze
begin
  str << " modified"
rescue => e
  puts "   str << 'modified' => #{e.class}: #{e.message}"
end

# But you can still add methods to String!
class String
  def frozen_status
    frozen? ? "I'm frozen" : "I'm mutable"
  end
end
puts "   str.frozen_status  => #{str.frozen_status}"
puts "   'new'.frozen_status => #{'new'.frozen_status}"
