# blocks_procs_lambdas.rb
# Ruby has three closely related but subtly different callable constructs:
# blocks, Procs, and Lambdas. Their differences are surprising and often confusing.

puts "=== Blocks vs Procs vs Lambdas ==="
puts

# --- Arity checking ---
puts "1. ARITY CHECKING — Lambdas are strict, Procs are loose:"
puts

my_lambda = lambda { |x, y| "lambda got #{x} and #{y}" }
my_proc = Proc.new { |x, y| "proc got #{x} and #{y}" }

puts "   Lambda with correct args:  #{my_lambda.call(1, 2)}"

begin
  my_lambda.call(1, 2, 3)
rescue ArgumentError => e
  puts "   Lambda with extra args:    ArgumentError — #{e.message}"
end

# Procs silently ignore extra args and fill missing ones with nil!
puts "   Proc with extra args:      #{my_proc.call(1, 2, 3)}"
puts "   Proc with missing args:    #{my_proc.call(1)}"
puts

# --- Return behavior (the BIG gotcha) ---
puts "2. RETURN BEHAVIOR — The most dangerous difference:"
puts

def test_lambda_return
  l = lambda { return 10 }  # return exits ONLY the lambda
  l.call
  "Lambda returned, method continues"  # This line executes!
end

def test_proc_return
  p = Proc.new { return 10 }  # return exits the ENCLOSING METHOD!
  p.call
  "Proc returned, method continues"  # This line NEVER executes!
end

puts "   test_lambda_return => #{test_lambda_return}"
puts "   test_proc_return   => #{test_proc_return}"
puts "   ^ Notice: Proc's return killed the whole method!"
puts

# --- The -> (stabby lambda) syntax ---
puts "3. STABBY LAMBDA syntax (->):"
square = ->(x) { x ** 2 }
puts "   ->(x) { x ** 2 }.call(5) => #{square.call(5)}"
puts "   Can also use square.(5)   => #{square.(5)}"
puts "   Or square[5]              => #{square[5]}"
puts

# --- Block to Proc conversion with & ---
puts "4. THE & OPERATOR — converting between blocks and procs:"
def capture_block(&block)
  puts "   block.class => #{block.class}"
  puts "   block.call  => #{block.call}"
end
capture_block { "I was a block, now I'm a Proc" }
puts

# Symbol#to_proc — the famous shorthand
puts "5. SYMBOL#to_proc — Ruby's most elegant trick:"
words = ["hello", "world", "ruby"]
puts "   words.map(&:upcase)    => #{words.map(&:upcase).inspect}"
puts "   words.map(&:length)    => #{words.map(&:length).inspect}"
puts "   [1,-2,3].select(&:positive?) => #{[1, -2, 3].select(&:positive?).inspect}"
puts
puts "   How it works: &:upcase calls :upcase.to_proc"
puts "   which creates: Proc.new { |obj| obj.upcase }"
puts

# --- Closures ---
puts "6. ALL THREE are closures (they capture their environment):"
counter = 0
incrementer = lambda { counter += 1 }
3.times { incrementer.call }
puts "   After 3 calls: counter = #{counter}"
puts

# --- Proc vs Lambda identity ---
puts "7. LAMBDA IS A PROC (but a special kind):"
l = lambda { |x| x }
p = Proc.new { |x| x }
puts "   lambda.class    => #{l.class}"
puts "   proc.class      => #{p.class}"
puts "   lambda.lambda?  => #{l.lambda?}"
puts "   proc.lambda?    => #{p.lambda?}"
puts "   Both are Proc objects, but lambda? distinguishes them!"
