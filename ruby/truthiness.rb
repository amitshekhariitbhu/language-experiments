# truthiness.rb
# Ruby's truthiness rules are simpler but MORE surprising than most languages.
# Only two values are falsy: false and nil. EVERYTHING else is truthy.
# This catches programmers from C, Python, JavaScript, etc. off guard.

puts "=== Ruby's Truthiness Rules ==="
puts

def truthy?(value)
  if value
    "TRUTHY"
  else
    "FALSY"
  end
end

# --- The only falsy values ---
puts "1. THE ONLY FALSY VALUES in Ruby:"
puts "   false => #{truthy?(false)}"
puts "   nil   => #{truthy?(nil)}"
puts

# --- Surprising truthy values ---
puts "2. SURPRISING TRUTHY values (falsy in other languages):"
puts "   0     => #{truthy?(0)}    (falsy in C, Python, JS!)"
puts "   0.0   => #{truthy?(0.0)}    (falsy in Python, JS!)"
puts "   ''    => #{truthy?('')}    (falsy in Python, JS!)"
puts "   []    => #{truthy?([])}    (falsy in Python, JS!)"
puts "   {}    => #{truthy?({})}    (falsy in Python!)"
puts "   '0'   => #{truthy?('0')}    (falsy in PHP, Perl!)"
puts "   :''   => #{truthy?(:'')}"
puts

# --- Direct comparison table ---
puts "3. COMPARISON with other languages:"
puts "   Value    Ruby     Python   JavaScript"
puts "   -----------------------------------"
puts "   0        truthy   falsy    falsy"
puts "   ''       truthy   falsy    falsy"
puts "   []       truthy   falsy    truthy"
puts "   {}       truthy   falsy    truthy"
puts "   nil      falsy    N/A      N/A"
puts "   None/null N/A     falsy    falsy"
puts "   NaN      truthy   truthy   falsy"
puts

# --- Practical implications ---
puts "4. PRACTICAL GOTCHAS:"
puts

# Checking for zero
count = 0
puts "   Gotcha 1: Checking if count has a value"
puts "   if count => enters the block! (count is 0, which is truthy)"
puts "   You must use: if count && count > 0" if count
puts

# Empty string check
name = ""
puts "   Gotcha 2: Empty string check"
if name
  puts "   if name => enters the block! ('' is truthy)"
  puts "   Use: if name && !name.empty?"
end
puts

# nil checks with the safe navigator
puts "5. THE SAFE NAVIGATOR (&.) — nil-aware method calls:"
arr = nil
puts "   arr = nil"
puts "   arr&.length  => #{arr&.length.inspect} (no NoMethodError!)"
puts "   arr&.map { } => #{(arr&.map { }).inspect}"
arr = [1, 2, 3]
puts "   arr = [1,2,3]"
puts "   arr&.length  => #{arr&.length}"
puts

# --- Boolean coercion ---
puts "6. DOUBLE BANG (!!) — force boolean coercion:"
puts "   !!nil    => #{!!nil}"
puts "   !!false  => #{!!false}"
puts "   !!0      => #{!!0}"
puts "   !!''     => #{!!''}"
puts "   !![]     => #{!![]}"
puts "   !!42     => #{!!42}"
puts

# --- Conditional assignment ---
puts "7. CONDITIONAL ASSIGNMENT (||=) — affected by truthiness:"
a = nil
a ||= "default"
puts "   nil ||= 'default'   => #{a.inspect}"

b = false
b ||= "default"
puts "   false ||= 'default' => #{b.inspect}"
puts "   ^ false gets overwritten! This is often a bug."

c = 0
c ||= "default"
puts "   0 ||= 'default'     => #{c.inspect}"
puts "   ^ 0 is NOT overwritten (it's truthy in Ruby)"
puts

# --- The === operator in case statements ---
puts "8. CASE/WHEN uses === which has special truthiness:"
value = 42
result = case value
         when Integer then "It's an integer (Integer === 42)"
         when String  then "It's a string"
         end
puts "   case 42; when Integer => #{result}"

value2 = "hello"
result2 = case value2
          when /^h/ then "Matches regex (Regexp === 'hello')"
          end
puts "   case 'hello'; when /^h/ => #{result2}"

value3 = 15
result3 = case value3
          when 1..10  then "in 1..10"
          when 11..20 then "in 11..20 (Range === 15)"
          end
puts "   case 15; when 11..20 => #{result3}"
